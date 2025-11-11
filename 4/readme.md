
# CIS4930/CAP5705 - Assignment 4: OpenGL Viewer

This project is an interactive OpenGL viewer built for Assignment 4. It loads a 3D model (`.obj`) and demonstrates several core computer graphics rendering techniques, including perspective projection, various shading models, and texture mapping.

## 🚀 Features
* **Modelview Transformation:** Renders a 3D model with a full perspective projection matrix.
* **Interactive Projection:** Allows real-time modification of the **FOV**, **Near Plane**, and **Far Plane** to observe their effects on the scene.
* **Z-Buffer Visualization:** Includes a toggleable mode to render the scene's depth buffer as a grayscale image.
* **Gouraud Shading:** Implements a per-vertex Blinn-Phong lighting model.
* **Phong Shading:** Implements a per-fragment (pixel) Blinn-Phong lighting model.
* **Texture Mapping:** Loads and applies a 2D texture (`spot_texture.png`) to the model using its UV coordinates, combining it with Phong shading.
* **Bonus: Flat Shading:** Implements a flat shading model using the `flat` GLSL qualifier to produce a faceted, low-poly look.


## 🔧 Compilation Instructions

This project uses CMake and requires **GLM**, **GLFW**, and **GLAD** to be installed in a location findable by CMake.
This project relies on a specific directory structure due to the hardcoded paths in `main.cpp`. Please ensure your files are organized as follows:

```

──── 4 
│   │
│   ├── main.cpp                 # Main C++ source file
│   ├── glad.c                   
│   ├── stb_image.h              # Texture loading library
│   │
│   ├── data/
│   │   ├── spot_triangulated_good.obj
│   │   └── spot_texture.png
│   │
│   ├── shader.vs                # Original vertex shader
│   ├── shader.fs                # Original fragment shader
│   │
│   ├── z_buffer.vs              # Z-Buffer shaders
│   ├── z_buffer.fs
│   │
│   ├── gouraud.vs               # Gouraud shading shaders
│   ├── gouraud.fs
│   │
│   ├── phong.vs                 # Phong shading shaders
│   ├── phong.fs
│   │
│   ├── phong_texture.vs         # Phong + Texture shaders
│   ├── phong_texture.fs
│   │
│   ├── flat.vs                  # Flat shading shaders (Bonus)
│   └── flat.fs

```
### Dependencies

* **CMake** (version 3.27 or higher)
* A C++17 compatible compiler (e.g., GCC, Clang, MSVC)
* **GLFW3**
* **GLAD**
* **GLM**
* **stb\_image.h** (included in the root directory)

### Build Steps

These instructions assume a Unix-like environment (Linux/macOS) or a terminal with CMake and a C++ compiler (like Visual Studio Developer Command Prompt).

1.  **Clone the repository** (or ensure all files are in the project directory).

2.  **Create a build directory** and navigate into it:

    ```bash
    mkdir build
    cd build
    ```

3.  **Run CMake** to configure the project. This will find your installed libraries.

    ```bash
    cmake ..
    ```

    > **Note:** If your libraries (GLFW, GLAD, GLM) are not found, you may need to help CMake by setting `CMAKE_PREFIX_PATH` or providing their root directories (e.g., `-DGLM_ROOT_DIR=/path/to/glm`).

4.  **Compile the project:**

    ```bash
    cmake --build .
    ```

    (Or, on Linux/macOS, you can simply run `make`).

5.  This will create an executable named **hw4** inside the `build` directory.

-----

## ⌨️ How to Use

### Running the Program

You must run the executable from the **project's root directory** (the one containing this README and the `data` folder), *not* from inside the `build` directory.

From the root directory, run:

```bash
./build/hw4
```

### Controls

* **Move Model:** **Arrow Keys** (Up, Down, Left, Right)
* **Rotate Model:** **W** (Pitch Up), **S** (Pitch Down), **A** (Yaw Left), **D** (Yaw Right)
* **Quit:** **Escape**

#### Projection Controls

* **F1 / F2:** Decrease / Increase **Field of View (FOV)**
* **F3 / F4:** Decrease / Increase **Near Plane** distance
* **F5 / F6:** Decrease / Increase **Far Plane** distance

#### Shading Model Controls

* **P:** Switch to **Phong Shading** (Default)
* **G:** Switch to **Gouraud Shading**
* **T:** Switch to **Phong Shading with Texture**
* **F:** Switch to **Flat Shading (Bonus)**
* **Z:** Toggle **Z-Buffer Visualization**
* **N:** Switch to the original simple shader