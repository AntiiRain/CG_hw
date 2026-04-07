# Computer Graphics Homework Project (CG_hw)

This repository contains a collection of computer graphics assignments covering fundamental concepts from basic OpenGL rendering to ray tracing and advanced shading techniques.

## Project Structure

### 1.1 Basic OpenGL & Texturing
- Implementation of the core OpenGL rendering pipeline.
- Demonstrates 2D/3D object rendering with texture mapping using `GLFW` and `GLEW`.

### 1.2 CPU Ray Tracer
- A custom "first-hit" ray tracing engine built in C++.
- **Features**: Sphere intersection testing, basic lighting, shadows, and reflections.
- **Components**:
  - `viewer`: An interactive ray-traced scene viewer.
  - `animator`: A tool to batch-render frames for animated sequences.

### 3. 3D Model Loader & Transformations
- Integrated an OBJ loader to render complex meshes (Teapot, Cow, Venus, etc.).
- Implemented view and projection matrices for interactive 3D navigation.
- Includes a comparison between standard GPU vertex shaders and CPU-side vertex processing.

### 4. Shading Models & Z-Buffer
- Comprehensive implementation of standard local illumination models:
  - **Flat Shading**: Per-face constant coloring.
  - **Gouraud Shading**: Per-vertex lighting with color interpolation.
  - **Phong Shading**: Per-pixel lighting with normal interpolation.
- **Z-Buffer Visualization**: Implementation of depth testing algorithms and visual representation of the depth buffer.

## Dependencies
- **OpenGL 3.3+**
- **GLFW 3**: Window and context management.
- **GLEW / GLAD**: OpenGL function loading.
- **GLM**: Mathematics library for graphics.
- **stb_image**: Image loading and writing.

## How to Build
This project uses CMake. 
1. Create a build directory: `mkdir build && cd build`
2. Configure the project: `cmake ..`
3. Build: `make` or build via your preferred IDE (CLion/Visual Studio).
