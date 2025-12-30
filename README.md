# MineC++

A voxel-based 3D sandbox game written in C++ using OpenGL, inspired by Minecraft. Build, break, and explore procedurally generated terrain with real-time physics and collision detection.

I wanted to pull back the curtain on C++ game development, so I challenged myself to replicate the core systems of Minecraft to see how far I could push my programming logic.

## Features

- **Voxel-Based Rendering**: Fast block-based world rendering with 24×24×10 chunk
- **Procedural Terrain**: Sine/cosine-based terrain generation for varied landscapes
- **First-Person Camera**: Smooth mouse-look controls with full 360° viewing
- **Physics & Gravity**: Realistic gravity, jumping, and velocity-based movement
- **Collision Detection**: Wall and ground collision for the player character
- **Block Interaction**: 
  - Left-click to break blocks
  - Right-click to place blocks
  - Select block types with number keys (1-2)
- **Sprint Mode**: Hold Shift to move faster
- **Textured Blocks**: Different textures for block sides and top surfaces

## Controls

| Key | Action |
|-----|--------|
| **W/A/S/D** | Move forward/left/backward/right |
| **Space** | Jump |
| **Shift** | Sprint |
| **Mouse** | Look around (disabled cursor) |
| **Left Click** | Break block |
| **Right Click** | Place block |
| **1/2** | Select block type |

## Requirements

- **C++11** or later
- **OpenGL 3.3+**
- **GLFW3** - Window and input handling
- **GLAD** - OpenGL loader
- **GLM** - Math library
- **stb_image** - Image loading

## Building

### Using the VS Code Configuration
This project includes `.vscode` settings for easy compilation in Visual Studio Code.

### Manual Compilation (Windows - MSVC)
```bash
cl /EHsc main.cpp /I include /link /LIBPATH:lib glfw3.lib opengl32.lib /OUT:output\MineC++.exe
```

### Manual Compilation (Windows - MinGW/G++)
```bash
g++ -o output/MineC++ main.cpp -I include -L lib -lglfw3 -lopengl32 -std=c++11
```

### Linux/macOS
Adapt paths to your system's library locations:
```bash
g++ -o output/MineC++ main.cpp -I include -L lib -lglfw -lGL -std=c++11
```

**Note:** Make sure `grass.png` and `top_grass.jpg` are in the working directory, or update the texture paths in `main.cpp`.

## File Structure

```
MineC++/
├── main.cpp              # Main game logic
├── include/              # Header files
│   ├── glad/             # GLAD OpenGL loader
│   ├── GLFW/             # GLFW window library
│   ├── glm/              # GLM math library
│   └── KHR/              # Khronos headers
├── lib/                  # Compiled libraries
├── output/               # Build output directory
├── .vscode/              # VS Code settings
├── grass.png             # Block side texture
├── top_grass.jpg         # Block top texture
└── README.md
```

## How It Works

### World System
The world is stored as a 3D array (`worldData[24][10][24]`) where each cell contains a block type (0 = empty, 1+ = solid blocks).

### Physics
- **Gravity**: Applied continuously at -12 units/s²
- **Jumping**: Grants 6.5 units/s upward velocity
- **Collision**: Checks block intersections at player's waist and head height

### Block Placement & Breaking
- **Raycast**: Casts a ray from camera position along the view direction (5-unit reach)
- **Breaking**: Removes the first solid block hit
- **Placement**: Places a block in the last empty space before hitting a solid block

### Rendering
- Uses vertex and fragment shaders for block rendering
- Renders blocks conditionally based on `worldData` state
- UI crosshair rendered with orthographic projection

## Future Improvements

- Chunk-based rendering for larger worlds
- Inventory system
- Multiple block types with distinct textures
- Lighting system
- Sound effects
- Network multiplayer support


## Author

**Amaanish** - [github.com/Amaanish](https://github.com/Amaanish)

---

Enjoy building! 🎮
