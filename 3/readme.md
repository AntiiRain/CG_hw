# **OpenGL Model Viewer \- CIS4930/CAP5705 Assignment 3**

This is a simple 3D model viewer written in C++ and modern OpenGL (3.3+), created as part of the coursework for CIS4930/CAP5705. The application is capable of loading, displaying, and manipulating 3D models from .obj files.

## **Features**

* **Model Loading**: Supports loading vertex data from Wavefront .obj files, with proper handling for models containing quadrilateral faces (via automatic triangulation).
* **Per-Vertex Coloring**: Generates smooth, interpolated colors for models based on the position coordinates of their vertices.
* **External Shaders**: GLSL shader code (vertex and fragment) is loaded dynamically from external files (.vs, .fs) for easier modification and management.
* **3D Transformations & Interaction**:
  * Utilizes the **GLM** library for all 3D math, including Model-View-Projection (MVP) matrix transformations.
  * Allows for real-time user interaction with a single model via keyboard controls.
* **Multiple Object Rendering**: Capable of loading and managing multiple 3D objects independently within the same scene.
* **Advanced Animation**: Implements an animation mode where two objects rotate synchronously around the axis connecting their centers.
* **Performance Analysis**: Includes implementations for both CPU-side and GPU-side model transformations to facilitate performance comparison.

## **How to Build and Run**

This project is configured to be built with CLion and uses vcpkg for dependency management.

### **Dependencies**

* [**Glad**](https://glad.dav1d.de/): OpenGL Loading Library.
* [**GLFW**](https://www.glfw.org/): For window creation and input handling.
* [**GLM**](https://glm.g-truc.net/0.9.9/index.html): For 3D mathematics (vectors and matrices).

All dependencies are managed via **vcpkg**.

### **Build Steps**

1. **Extract Project**: Unzip the submitted project folder.  
2. Configure vcpkg (if not already set up):
   Ensure your CLion is properly integrated with vcpkg. To use the MinGW compiler toolchain, specify the correct vcpkg triplet in CLion's CMake settings:
   * Go to File \-\> Settings \-\> Build, Execution, Deployment \-\> CMake.
   * In the CMake options field, add: \-DVCPKG\_TARGET\_TRIPLET=x64-mingw-static
3. Open the Project:
   Open the project folder with CLion. It will automatically detect the CMakeLists.txt file, configure the project, and allow vcpkg to build the required dependencies.
4. Build:
   Click the build button (hammer icon) in the top-right of the CLion window or use the shortcut Ctrl \+ F9. This will generate two executables.
5. **Run**:
   * Ensure that the shader files (shader.vs, shader.fs, shader\_cpu.vs) and the data folder are located in the correct path as defined by the BASE\_PATH variable in the code.
   * You can run the main application by selecting the 3 target in CLion and clicking the run button (green play icon).
   * A separate executable, cube\_ctr, is built from ctr.cpp to demonstrate the control scheme.

## **Controls**

The keyboard controls for interacting with the models are detailed in the **ctr.cpp** source file. A summary is provided below:

* **W / S Keys**: Rotate the model(s) around the X-axis.
* **A / D Keys**: Rotate the model(s) around the Y-axis.
* **Up/Down/Left/Right Arrow Keys**: Translate the model(s) along the X and Y axes.
* **Esc Key**: Close the application.

## **Performance Analysis**
We are using f16 model to analysis performance in different device.
cpu_testing.cpp is the programe using cpu to render model.
For gpu plz switch model in main.cpp


The keyboard controls for interacting with the models are detailed in the **ctr.cpp** source file. A summary is provided below:

* **W / S Keys**: Rotate the model(s) around the X-axis.
* **A / D Keys**: Rotate the model(s) around the Y-axis.
* **Up/Down/Left/Right Arrow Keys**: Translate the model(s) along the X and Y axes.
* **Esc Key**: Close the application.

## **File Structure**

.
├── data/                  \# Contains .obj model files
│   ├── cube.obj
│   └── cube2.obj
├── shader.fs              \# Fragment shader
├── shader.vs              \# Vertex shader for GPU transforms
├── shader\_cpu.vs          \# Vertex shader for CPU transforms
├── CMakeLists.txt         \# CMake configuration file
├── main.cpp               \# Main application source code
├── cpu_testing.cpp        \# Performance Analysis for cpu application source code 
├── ctr.cpp                \# Standalone code demonstrating controls
└── report.md              \# Project report document

