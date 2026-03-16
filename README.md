# Jalapeno

`Jalapeno` is a render engine built on OpenGL from scratch. It is in very early development stage, still a lot to do.
It’s designed for experiments and real-time graphics research.

## Features

- **Forward Pipeline**
  - At the moment only a simple forward pass has been implemented
  
- **Models Importation**
  - Support for loading any model supported by Assimp library.

- **Anti-Aliasing**
  - Configurable MSAA

- **Lighting & Shadows**
  - Directional Lights with Shadow Mapping
  - Point Lights with Shadow Mapping (Cubemaps and Geometry shader)
  - Support for multiple lights

- **Materials**
  - Lambert Materials
  - Phong Materials
  - PBR Materials (Metallic-Roughness)

- **Built-in Models**
  - Sphere

## Screenshots

![Screenshot](resources/screenshots/screenshot.png)
![Screenshot](resources/screenshots/screenshot4.png)
![Screenshot](resources/screenshots/screenshot2.png)
![Screenshot](resources/screenshots/screenshot3.png)

## Controls
| Key        | Action                   |
|------      |--------------------------|
| WASD       | Move                     |
| Alt + LMB  | Camera rotation          |
