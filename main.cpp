#include <iostream> // gives wide string (more symbols supported), sine, and cosine functions
#include <Windows.h> // gives handle and functions to create and set console screen buffer
#include <chrono> // gives system clock to calculate elapsed time 
#include <string> // gives wstring

#define PI 3.14159f

// Screen
int screenWidth = 120;
int screenHeight = 40;
int numPixels = screenWidth * screenHeight;

// Player
float playerX = 8.0f;
float playerY = 8.0f;
float playerA = 0.0f;

// Map
int mapWidth = 16;
int mapHeight = 16;

// Camera
float FOV = PI / 4.0f;
float maxDepth = 16.0f; // cuz map size is 16

int main() {


	wchar_t *screenBuffer = new wchar_t[numPixels];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	// Map
	std::wstring map;
	
	map += L"################";
	map += L"#              #";
	map += L"#  ###         #";
	map += L"#              #";
	map += L"#         ######";
	map += L"#  #           #";
	map += L"#  #           #";
	map += L"#  #           #";
	map += L"#              #";
	map += L"#              #";
	map += L"######         #";
	map += L"#              #";
	map += L"#              #";
	map += L"#    ######### #";
	map += L"#              #";
	map += L"################";

	auto previousTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();

	// Game Loop
	while (true) {
		// Update elapsed time every frame
		currentTime = std::chrono::system_clock::now();
		std::chrono::duration<float> deltaTime = currentTime - previousTime;
		previousTime = currentTime;
		float elapsedTime = deltaTime.count();
		
		#pragma region INPUT
		// Rotate player left and right
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			playerA -= 1.0f * elapsedTime;
			if (playerA < 0) {
				playerA = 2 * PI;
			}
			
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			playerA += 1.0f * elapsedTime;
			if (playerA >= 2 * PI) {
				playerA = 0;
			}
		}


		// Move player forward and back
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			playerX += cosf(playerA) * 5.0f * elapsedTime;
			playerY += sinf(playerA) * 5.0f * elapsedTime;

			if (map[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX -= cosf(playerA) * 5.0f * elapsedTime;
				playerY -= sinf(playerA) * 5.0f * elapsedTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			playerX -= cosf(playerA) * 5.0f * elapsedTime;
			playerY -= sinf(playerA) * 5.0f * elapsedTime;

			if (map[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX += cosf(playerA) * 5.0f * elapsedTime;
				playerY += sinf(playerA) * 5.0f * elapsedTime;
			}
		}

		// Strafe side left and right
		if (GetAsyncKeyState((unsigned short)'Q') & 0x8000) {
			float tempX = playerX, tempY = playerY;
			if (playerA <= PI / 2.0f) {
				playerX += abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY -= abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			else if (playerA <= PI) {
				playerX += abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY += abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			else if (playerA <= 3 * PI / 2.0f) {
				playerX -= abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY += abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			else {
				playerX -= abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY -= abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			if (map[int(playerY) * mapWidth + playerX] == '#') {
				playerX = tempX;
				playerY = tempY;
			}
		}

		if (GetAsyncKeyState((unsigned short)'E') & 0x8000) {
			float tempX = playerX, tempY = playerY;
			if (playerA <= PI / 2.0f) {
				playerX -= abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY += abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			else if (playerA <= PI) {
				playerX -= abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY -= abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			else if (playerA <= 3 * PI / 2.0f) {
				playerX += abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY -= abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			else {
				playerX += abs(cosf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
				playerY += abs(sinf(PI / 2.0f + playerA) * 5.0f * elapsedTime);
			}
			if (map[int(playerY) * mapWidth + playerX] == '#') {
				playerX = tempX;
				playerY = tempY;
			}
		}

		
		#pragma endregion 

		#pragma region RENDERER
		// The last character in the buffer is the escape character 
		screenBuffer[numPixels - 1] = '\0';

		for (int x = 0; x < screenWidth; x++) {
			// For each column x, calculate projected ray angle
			float startingAngle = playerA - FOV / 2.0f;
			float angleChange = ((float)x / (float)screenWidth) * FOV;
			float rayAngle = startingAngle + angleChange;

			// Cast ray until it hits a wall
			float distanceToWall = 0.0f;
			bool hitWall = false;

			// Unit circle: find x, y coord along direction of current ray
			float eyeX = cosf(rayAngle);
			float eyeY = sinf(rayAngle);

			while (!hitWall && distanceToWall < maxDepth) {
				distanceToWall += 0.1f;

				int testX = (int)(playerX + eyeX * distanceToWall);
				int testY = (int)(playerY + eyeY * distanceToWall);

				// If ray is out of bounds, say it hits a wall
				if (testX < 0 || testY < 0 || testX >= mapWidth || testY >= mapHeight) {
					hitWall = true;
					distanceToWall = maxDepth;
				}

				else {
					// In 2D array, y * array width + x converts 2D location (x,y) to 1D index
					// Compare the element at the current index with the wall character
					if (map[testY * mapWidth + testX] == '#') {
						hitWall = true;
					}
					// Visualize player FOV on the map
					else if (!(testX == (int)playerX && testY == (int)playerY)){
						screenBuffer[testY * screenWidth + testX] = '.';
					}

				}
			}

			float midpoint = (float)screenHeight / 2.0f;

			// As distance to wall increases, height of the wall decreases
			float wallHeight = (float)screenHeight / distanceToWall;

			// A higher wall makes a smaller ceiling 
			// Starts at top of screen and ends at ceilingEnd (top to bottom)
			int ceilingEnd = midpoint - wallHeight;

			// The floor is a mirror of the ceiling starting at the bottom of the screen
			// Starts at floorStart and ends at bottom of screen (top to bottom)
			int floorStart = screenHeight - ceilingEnd;

			short shading = ' ';
			// From close to far
			if (distanceToWall <= maxDepth / 4.0f) { shading = 0x2588; } // full block 
			else if (distanceToWall < maxDepth / 3.0f) { shading = 0x2593; } // dark shade 
			else if (distanceToWall < maxDepth / 2.0f) { shading = 0x2592; } // medium shade 
			else if (distanceToWall < maxDepth) { shading = 0x2591; } // light shade 
			else { shading = ' '; } // space 

			// Everything in between the end of the ceiling and start of the floor is wall
			for (int y = 0; y < screenHeight; y++) {
				int index = y * screenWidth + x;
				
				if (y <= ceilingEnd) {
					screenBuffer[index] = ' ';
				}
				else if (y > ceilingEnd && y <= floorStart) {
					screenBuffer[index] = shading;
				}
				else if (y > floorStart) {
					float floorCloseness = ((float)y - midpoint) / midpoint;
					if (floorCloseness <= .25f) { shading = ' '; }
					else if (floorCloseness <= .5f) { shading = '-'; }
					else if (floorCloseness <= .75f) { shading = '='; }
					else if (floorCloseness <= .9f) { shading = '#'; }
					screenBuffer[index] = shading;
				}
			}
		}
		#pragma endregion 

		// Display a map on the screen
		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {
				
				// Do not overwrite ray cast / FOV visualization on map
				if (screenBuffer[y * screenWidth + x] == '.') {
					continue;
				}

				screenBuffer [y*screenWidth + x] = map[y * mapWidth + x];

				// Draw player on top of map
				if ((int)playerX == x && (int)playerY == y) {
					screenBuffer[y * screenWidth + x] = 'O';
				}
			}
		}

		// Display stats
		std::wstring stats;
		stats += L" X: " + std::to_wstring(playerX);
		stats += L" Y: " + std::to_wstring(playerY);
		stats += L" A: " + std::to_wstring(playerA);
		stats += L" FPS: " + std::to_wstring((float)1/elapsedTime) + L" ";

		for (int i = stats.length()-1; i >= 0; i--) {
			screenBuffer[screenWidth * (screenHeight - 1) + (screenWidth - i)] = stats[stats.length()-1-i];
		}

		// Update frame
		// Write to location {0, 0} [top left corner] to stop console from scrolling down
		WriteConsoleOutputCharacter(console, screenBuffer, numPixels, { 0,0 }, &bytesWritten);
		
		
	}
	return 0;
}