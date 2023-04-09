#include <iostream> // gives wide string (more symbols supported), sine, and cosine functions
#include <Windows.h> // gives handle and functions to create and set console screen buffer
#include <chrono> // gives system clock to calculate elapsed time 
#include <string>
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
float FOV = 3.14159f / 4.0f;
float maxDepth = 16.0f; // cuz map size is 16

int main() {


	wchar_t *screenBuffer = new wchar_t[numPixels];
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD bytesWritten = 0;

	// Map
	std::wstring map;
	
	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
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
		}
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			playerA += 1.0f * elapsedTime;
		}

		// Move player forward and back
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			playerX += cosf(playerA) * 5.0f * elapsedTime;
			playerY += sinf(playerA) * 5.0f * elapsedTime;
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			playerX -= cosf(playerA) * 5.0f * elapsedTime;
			playerY -= sinf(playerA) * 5.0f * elapsedTime;
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

			// Everything in between the end of the ceiling and start of the floor is wall
			for (int y = 0; y < screenHeight; y++) {
				int index = y * screenWidth + x;

				if (y <= ceilingEnd) {
					screenBuffer[index] = ' ';
				}
				else if (y > ceilingEnd && y <= floorStart) {
					screenBuffer[index] = '#';
				}
				else if (y > floorStart) {
					screenBuffer[index] = ' ';
				}
			}
		}
		#pragma endregion 

		// Adds a map on the screen
		for (int y = 0; y < mapHeight; y++) {
			for (int x = 0; x < mapWidth; x++) {
				screenBuffer [y*screenWidth + x] = map[y * mapWidth + x];
				if ((int)playerX == x && (int)playerY == y) {
					screenBuffer[y * screenWidth + x] = 'O';
				}
			}
		}

		std::wstring stats;
		stats += L" X: " + std::to_wstring((int)playerX);
		stats += L" Y: " + std::to_wstring((int)playerY);
		stats += L" A: " + std::to_wstring((int)playerA) + L" ";

		for (int i = stats.length()-1; i >= 0; i--) {
			screenBuffer[screenWidth * (screenHeight - 1) + (screenWidth - i)] = stats[stats.length()-1-i];
		}
		// Update frame
		// Write to location {0, 0} [top left corner] to stop console from scrolling down
		WriteConsoleOutputCharacter(console, screenBuffer, numPixels, { 0,0 }, &bytesWritten);
		
		
	}
	return 0;
}