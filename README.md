# FPS-Ray-Casting-Engine
An ASCII-based FPS Ray Casting Engine made with C++. It uses Windows API to write Unicode characters to the console screen buffer. Inspired by Javidx9 (2017), who produced an insightful video on first-person shooter programming.

<img width="1000" src="https://user-images.githubusercontent.com/74445404/230852648-bcf3bba8-26fa-425d-960b-274df3a211c3.png">

## Controls
| Key | Action |
| :-- | :------------- |
| W | Move Forward |
| S | Move Backward |
| A | Rotate Left |
| S | Rotate Right |
| Q | Strafe Left |
| R | Strafe Right |

## The Minimap (top left)
| Symbol | Description |
| :-- | :------------- |
| # | Wall |
| O | Player |
| . | Field of View* |

*FOV displays which parts of the map the player can currently see via ray cast.

## Stats (bottom right)
| Symbol | Description |
| :-- | :------------- |
| X | Player X position |
| Y | Player Y position |
| A | Player angle (radians) |
| FPS | Framerate |

## Before you play
Set your console window size property to 120 x 40. 
