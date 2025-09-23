# Simple Ray Tracer for CAP5705 / CIS4930

This is a simple "first-hit" ray tracer built from scratch in C++. It renders a scene composed of spheres and implements several key computer graphics concepts. The project builds two separate executable programs: an interactive viewer and an animation frame generator.

## Features Implemented

* **Ray Tracing Engine**: CPU-based rendering by casting rays for each pixel.
* **Camera Models**: Supports both **Perspective** and **Orthographic** camera projections in the interactive viewer.
* **Phong Illumination**: Implements the full Phong local illumination model.
* **Recursive Reflections**: Simulates mirror-like ("glazed") surfaces.
* **Animation**: The `animator` program renders a 120-frame sequence with a moving camera and objects.
* **Interactive Controls**: The `raytracer` program allows for real-time camera switching.

## Dependencies

You will need a C++ compiler and **CMake**. The following libraries are required and can be installed via **vcpkg**:
* **GLEW**
* **GLFW**
* **GLM**
* **stb_image_write**: Included as a header in the project.

Instructions for installing vcpkg can be found on the [official vcpkg GitHub repository](https://github.com/microsoft/vcpkg). Once vcpkg is set up, install the required packages:
```bash
./vcpkg install glew glfw3 glm
```

## How to Run and Test
1. Interactive Viewer (raytracer)
Run the interactive program from the build directory:

```Bash

./raytracer
A window will appear displaying the rendered scene.
```
Controls:

P key: Switch between Perspective and Orthographic camera views.

ESC key: Close the application.

2. Animation Renderer (animator)
First, create a directory to store the output frames:

```Bash

# from the project root directory
mkdir output
```
Then, run the animator program from the build directory:

```bash

./animator
```
The program will render 120 frames and save them as PNG images in the output directory. It will print its progress to the console.

After it finishes, use FFmpeg to assemble the frames into a video file:

```Bash

# from the project root directory
ffmpeg -framerate 30 -i output/frame_%03d.png -c:v libx264 -pix_fmt yuv420p animation.m
```