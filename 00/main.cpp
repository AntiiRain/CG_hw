// Based on templates from learnopengl.com
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>


using namespace glm;
struct Sphere {
  glm::vec3 center;   // 中心位置
  float radius;       // 半径
  glm::vec3 color;    // 颜色 (例如 R,G,B值范围在0.0到1.0)
};



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
float interaction(const Sphere& S, const vec3& ray_origin, const vec3& ray_direction);

// 定义一个球体结构体


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
	"gl_Position = vec4(aPos, 1.0);\n"
	"ourColor = aColor;\n"
	"TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";






int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    Sphere  test;

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Display RGB Array", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // GLEW: load all OpenGL function pointers
    glewInit();

    // build and compile the shaders
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create the image (RGB Array) to be displayed
    const int width  = 256; // keep it in powers of 2!
    const int height = 256; // keep it in powers of 2!
    unsigned char image[width*height*3];



    Sphere sphere1;
    sphere1.center = glm::vec3(-40.0f, 0.0f, -3.0f); // 示例位置
    sphere1.radius = 30.0f;
    sphere1.color = glm::vec3(0.0f, 1.0f, 0.0f);   // 绿色

    Sphere sphere2;
    sphere2.center = glm::vec3(20.0f, 0.0f, -3.0f);  // 示例位置
    sphere2.radius = 20.0f;                         // 不同半径
    sphere2.color = glm::vec3(0.0f, 0.0f, 1.0f);   // 蓝色



    glm::vec3 camera_viewpoint(0.0f, 0.0f, 0.0f);
//    glm::vec3 camera_viewpoint(0, 50, 0);//test 1

//    glm::vec3 camera_direction(0.0f, 0.0f, -1.0f);
//  glm::vec3 camera_direction((-0.2, 0, -1.0)); //test 2
  glm::vec3 camera_direction_unnormalized(-0.2f, 0.0f, -1.0f);


    for(int i = 0; i < height; i++)
    {

        for (int j = 0; j < width; j++)
        {
          float y = (height / 2.0f) - i;
          float x = (width / 2.0f) - j;

          glm::vec3 camera_direction = glm::normalize(camera_direction_unnormalized);

          vec3 tmp (x,y,0);

          vec3 ray_o = camera_viewpoint +vec3(x,y,0);

          vec3 pixel_color(0.1f, 0.1f, 0.1f);

          float closest_t = std::numeric_limits<float>::max();



          float t1 = interaction(sphere1,ray_o,camera_direction);
          if (t1 > 0.0f && t1 <= closest_t) {
            closest_t = t1;
            pixel_color = sphere1.color;
          }

          float t2= interaction(sphere2,ray_o,camera_direction);
          if(t2>=0 && t2<closest_t){
            closest_t = t2;
            pixel_color = sphere2.color;
          }

          int idx = (i * width + j) * 3;
          image[idx]   = (unsigned char)(pixel_color.r * 255.999f);
          image[idx+1] = (unsigned char)(pixel_color.g * 255.999f);
          image[idx+2] = (unsigned char)(pixel_color.b * 255.999f);
        }
    }

//  for(int i = 0; i < height; i++)
//  {
//    for (int j = 0; j < width; j++)
//    {
//      // --- 将像素坐标(j,i)映射到[-1, 1]的标准化平面 ---
//      float world_x = 4*(j - width / 2.0f) / (width / 2.0f);
//      float world_y = 4*-(i - height / 2.0f) / (height / 2.0f);
//
//      // 使用标准化坐标生成光线起点
//      vec3 ray_o = camera_viewpoint + vec3(world_x, world_y, 0);
//      // ---------------------------------------------------
//
//      // (后面的相交和颜色设置逻辑保持不变)
//      vec3 pixel_color(0.1f, 0.1f, 0.1f);
//      float closest_t = std::numeric_limits<float>::max();
//
//      float t1 = interaction(sphere1, ray_o, camera_direction);
//      if (t1 > 0.0f && t1 < closest_t) {
//        closest_t = t1;
//        pixel_color = sphere1.color;
//      }
//
//      // 之后您可以取消对 sphere2 的注释来看到两个球体
//       float t2 = interaction(sphere2, ray_o, camera_direction);
//       if (t2 > 0.0f && t2 < closest_t) {
//           closest_t = t2;
//           pixel_color = sphere2.color;
//       }
//
//      int idx = (i * width + j) * 3;
//      image[idx]   = (unsigned char)(pixel_color.r * 255.999f);
//      image[idx+1] = (unsigned char)(pixel_color.g * 255.999f);
//      image[idx+2] = (unsigned char)(pixel_color.b * 255.999f);
//    }
//  }

    unsigned char *data = &image[0];
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}



float interaction(const Sphere& S, const vec3& ray_origin, const vec3& ray_direction) {
  vec3 oc = ray_origin - S.center;
  float a = dot(ray_direction, ray_direction);
  float b = 2.0f * dot(oc, ray_direction);
  float c = dot(oc, oc) - S.radius * S.radius;

  float discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0f;
  } else {
    // 求解一元二次方程
    float t0 = (-b - sqrt(discriminant)) / (2.0f * a);
    float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
    if (t0 > 0.001f && t0 < t1) {
      return t0;
    }
    if (t1 > 0.001f) {
      return t1;
    }
    return -1.0f;
  }
}