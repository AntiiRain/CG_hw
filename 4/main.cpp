// template based on material from learnopengl.com
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



//resvise your basepath
const std::string BASE_PATH = "I:/Code/CG/CG_hw/4/";
glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
glm::vec3 objectRotation(0.0f, 0.0f, 0.0f); // 存储欧拉角
float cameraMoveSpeed = 0.05f;

float fov = 45.0f;
float nearPlane = 0.1f;
float farPlane = 100.0f;


unsigned int gouraudShaderProgram; // Gouraud 着色器
unsigned int phongShaderProgram;   // Phong 着色器
unsigned int phongTextureShaderProgram; // --- 新增 ---


bool showZBuffer = false; // 用于切换 Z-Buffer 视图的布尔值

enum ShadingMode { SHADE_NORMAL, SHADE_ZBUFFER, SHADE_GOURAUD, SHADE_PHONG,SHADE_PHONG_TEXTURE};
ShadingMode currentShadingMode = SHADE_NORMAL; // 默认是你的原始着色器



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool loadOBJ(const char* path, std::vector<float>& out_vertices);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//// Vertex shader: add MVP matrix
//const char *vertexShaderSource = "#version 330 core\n"
//                                 "layout (location = 0) in vec3 aPos;\n"
//                                 "layout (location = 1) in vec3 aColor;\n"
//                                 "out vec3 ourColor;\n"
//                                 "uniform mat4 uMVP;\n"
//                                 "void main(){\n"
//                                 "   gl_Position = uMVP * vec4(aPos, 1.0);\n"
//                                 "   ourColor = aColor;\n"
//                                 "}\n";
//
//// Fragment shader
//const char *fragmentShaderSource = "#version 330 core\n"
//                                   "out vec4 FragColor;\n"
//                                   "in vec3 ourColor;\n"
//                                   "void main(){\n"
//                                   "   FragColor = vec4(ourColor, 1.0);\n"
//                                   "}\n";



std::string readShaderFile(const std::string& filePath) {
  std::ifstream shaderFile;
  // 开启 ifstream 异常
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // 打开文件
    shaderFile.open(filePath);
    std::stringstream shaderStream;
    // 读取文件缓冲到数据流中
    shaderStream << shaderFile.rdbuf();
    // 关闭文件处理器
    shaderFile.close();
    // 转换数据流到字符串
    return shaderStream.str();
  } catch (std::ifstream::failure& e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
    return "";
  }
}



int main()
{
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "viewGL", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // set initial viewport and enable depth test
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader program


  std::string vsPath = BASE_PATH + "shader.vs";
  std::string fsPath = BASE_PATH + "shader.fs";
  std::string vertexShaderCode = readShaderFile(vsPath);
  std::string fragmentShaderCode = readShaderFile(fsPath);
  if (vertexShaderCode.empty() || fragmentShaderCode.empty()) {
    // 如果加载失败，程序退出
    return -1;
  }

  const char* vertexShaderSource = vertexShaderCode.c_str();
  const char* fragmentShaderSource = fragmentShaderCode.c_str();




  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  unsigned int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  //z-buffer shader
  // ------------------------------------------------------------------
  // --- 新增：编译 Z-Buffer 着色器 ---
  // ------------------------------------------------------------------
  std::string zvsPath = BASE_PATH + "z_buffer.vs";
  std::string zfsPath = BASE_PATH + "z_buffer.fs";
  std::string zVertexShaderCode = readShaderFile(zvsPath);
  std::string zFragmentShaderCode = readShaderFile(zfsPath);
  if (zVertexShaderCode.empty() || zFragmentShaderCode.empty()) {
    return -1;
  }

  const char* zVertexShaderSource = zVertexShaderCode.c_str();
  const char* zFragmentShaderSource = zFragmentShaderCode.c_str();

  unsigned int zVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(zVertexShader, 1, &zVertexShaderSource, NULL);
  glCompileShader(zVertexShader);
  // --- 添加检查 ---
  glGetShaderiv(zVertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(zVertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::Z_VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }


  unsigned int zFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(zFragmentShader, 1, &zFragmentShaderSource, NULL);
  glCompileShader(zFragmentShader);
  // --- 添加检查 ---
  glGetShaderiv(zFragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(zFragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::Z_FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }


  unsigned int zBufferShaderProgram = glCreateProgram();
  glAttachShader(zBufferShaderProgram, zVertexShader);
  glAttachShader(zBufferShaderProgram, zFragmentShader);
  glLinkProgram(zBufferShaderProgram);
  // --- 添加检查 ---
  glGetProgramiv(zBufferShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(zBufferShaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::Z_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }


  glDeleteShader(zVertexShader);
  glDeleteShader(zFragmentShader);
  // --- Z-Buffer 着色器编译结束 ---
  // ------------------------------------------------------------------
// --- 新增：编译 Gouraud 着色器 ---
// ------------------------------------------------------------------
  std::string gouraudVSPath = BASE_PATH + "gouraud.vs";
  std::string gouraudFSPath = BASE_PATH + "gouraud.fs";
  std::string gouraudVertexCode = readShaderFile(gouraudVSPath);
  std::string gouraudFragmentCode = readShaderFile(gouraudFSPath);


  const char* gVertexSource = gouraudVertexCode.c_str();
  const char* gFragmentSource = gouraudFragmentCode.c_str();

  unsigned int gVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(gVertexShader, 1, &gVertexSource, NULL);
  glCompileShader(gVertexShader);


  unsigned int gFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(gFragmentShader, 1, &gFragmentSource, NULL);
  glCompileShader(gFragmentShader);


  gouraudShaderProgram = glCreateProgram();
  glAttachShader(gouraudShaderProgram, gVertexShader);
  glAttachShader(gouraudShaderProgram, gFragmentShader);
  glLinkProgram(gouraudShaderProgram);

  glDeleteShader(gVertexShader);
  glDeleteShader(gFragmentShader);

// ------------------------------------------------------------------
// --- 新增：编译 Phong 着色器 ---
// ------------------------------------------------------------------
  std::string phongVSPath = BASE_PATH + "phong.vs";
  std::string phongFSPath = BASE_PATH + "phong.fs";
  std::string phongVertexCode = readShaderFile(phongVSPath);
  std::string phongFragmentCode = readShaderFile(phongFSPath);
// (省略了空检查)

  const char* pVertexSource = phongVertexCode.c_str();
  const char* pFragmentSource = phongFragmentCode.c_str();

  unsigned int pVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(pVertexShader, 1, &pVertexSource, NULL);
  glCompileShader(pVertexShader);


  unsigned int pFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(pFragmentShader, 1, &pFragmentSource, NULL);
  glCompileShader(pFragmentShader);


  phongShaderProgram = glCreateProgram();
  glAttachShader(phongShaderProgram, pVertexShader);
  glAttachShader(phongShaderProgram, pFragmentShader);
  glLinkProgram(phongShaderProgram);

  glDeleteShader(pVertexShader);
  glDeleteShader(pFragmentShader);


//ToDo: adding SHADE_PHONG_TEXTURE compelling
  std::string ptVSPath = BASE_PATH + "phong_texture.vs";
  std::string ptFSPath = BASE_PATH + "phong_texture.fs";
  std::string ptVertexCode = readShaderFile(ptVSPath);
  std::string ptFragmentCode = readShaderFile(ptFSPath);
  if (ptVertexCode.empty() || ptFragmentCode.empty()) {
    return -1;
  }

  const char* ptVertexSource = ptVertexCode.c_str();
  const char* ptFragmentSource = ptFragmentCode.c_str();

  unsigned int ptVertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(ptVertexShader, 1, &ptVertexSource, NULL);
  glCompileShader(ptVertexShader);
  // 添加编译检查
  glGetShaderiv(ptVertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(ptVertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PHONG_TEXTURE_VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  unsigned int ptFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(ptFragmentShader, 1, &ptFragmentSource, NULL);
  glCompileShader(ptFragmentShader);
  // 添加编译检查
  glGetShaderiv(ptFragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(ptFragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PHONG_TEXTURE_FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  phongTextureShaderProgram = glCreateProgram();
  glAttachShader(phongTextureShaderProgram, ptVertexShader);
  glAttachShader(phongTextureShaderProgram, ptFragmentShader);
  glLinkProgram(phongTextureShaderProgram);
  // 添加链接检查
  glGetProgramiv(phongTextureShaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(phongTextureShaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PHONG_TEXTURE_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }
  glDeleteShader(ptVertexShader);
  glDeleteShader(ptFragmentShader);




  // load OBJ -> interleaved positions (3) + colors (3)
  std::string objPath = BASE_PATH + "data/spot_triangulated_good.obj";
  std::vector<float> vertices;
  bool load_success = loadOBJ(objPath.c_str(), vertices);
  if (!load_success) {
    return -1;
  }
  int numVertices = static_cast<int>(vertices.size() / 8);
  std::cout << "Successfully loaded " << numVertices << " vertices." << std::endl;
  if (numVertices == 0) {
    std::cout << "No vertices parsed. Exiting." << std::endl;
    return -1;
  }

  // create VBO/VAO
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // uniform location
  GLint uMVPLoc = glGetUniformLocation(shaderProgram, "uMVP");

  // --- 新增：获取 Z-Buffer 着色器的 uniform location ---
  GLint uMVPLoc_zbuffer = glGetUniformLocation(zBufferShaderProgram, "uMVP");


  // --- 新增：加载纹理 ---
  // 确保你有一个 "uv_test.png" 或 "texture.jpg" 之类的文件
  std::string texPath = BASE_PATH + "data/spot_texture.png";
  unsigned int textureID = loadTexture(texPath.c_str());

  // render loop
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    // clear buffers
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // compute aspect from current framebuffer size (handles resizes)
    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    float aspect = (fbh > 0) ? (static_cast<float>(fbw) / static_cast<float>(fbh)) : 1.3333f;


    // --- 定义光照和相机 ---
    glm::vec3 lightPos(0.0f, 3.0f, 4.0f); // 一个简单的光源位置
    glm::vec3 viewPos(0.0f, 0.0f, 3.0f);   // 相机位置 (同 glm::lookAt 的 'from'
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // 白光
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f); // 类似珊瑚的颜色




    // build MVP
    glm::mat4 model = glm::mat4(1.0f);
    // 1. 应用平移
    model = glm::translate(model, objectPosition);
    // 2. 应用旋转 (注意顺序)
    model = glm::rotate(model, glm::radians(objectRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(objectRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));

//    model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

//    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    glm::mat4 proj = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);

    glm::mat4 mvp = proj * view * model;

//    if (showZBuffer) {
//      glUseProgram(zBufferShaderProgram);
//      glUniformMatrix4fv(uMVPLoc_zbuffer, 1, GL_FALSE, glm::value_ptr(mvp));
//    } else {
//      glUseProgram(shaderProgram);
//      glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp));
//    }
    switch (currentShadingMode)
    {
      case SHADE_NORMAL:
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        break;

      case SHADE_ZBUFFER:
        glUseProgram(zBufferShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(zBufferShaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        // (如果你做了线性化，这里还要传 near/far)
        // glUniform1f(glGetUniformLocation(zBufferShaderProgram, "near"), nearPlane);
        // glUniform1f(glGetUniformLocation(zBufferShaderProgram, "far"), farPlane);
        break;

      case SHADE_GOURAUD:
        glUseProgram(gouraudShaderProgram);
        // 设置所有 Gouraud uniform
        glUniformMatrix4fv(glGetUniformLocation(gouraudShaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(gouraudShaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(gouraudShaderProgram, "uLightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(gouraudShaderProgram, "uViewPos"), 1, glm::value_ptr(viewPos));
        glUniform3fv(glGetUniformLocation(gouraudShaderProgram, "uLightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(gouraudShaderProgram, "uObjectColor"), 1, glm::value_ptr(objectColor));
        break;

      case SHADE_PHONG:
        glUseProgram(phongShaderProgram);
        // 设置所有 Phong uniform (注意 uMVP 和 uModel 还是需要的)
        glUniformMatrix4fv(glGetUniformLocation(phongShaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(phongShaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(phongShaderProgram, "uLightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(phongShaderProgram, "uViewPos"), 1, glm::value_ptr(viewPos));
        glUniform3fv(glGetUniformLocation(phongShaderProgram, "uLightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(phongShaderProgram, "uObjectColor"), 1, glm::value_ptr(objectColor));
        break;

      case SHADE_PHONG_TEXTURE:
        glUseProgram(phongTextureShaderProgram);

        // 1. 设置与 Phong 相同的 Uniforms
        glUniformMatrix4fv(glGetUniformLocation(phongTextureShaderProgram, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(glGetUniformLocation(phongTextureShaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(glGetUniformLocation(phongTextureShaderProgram, "uLightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(phongTextureShaderProgram, "uViewPos"), 1, glm::value_ptr(viewPos));
        glUniform3fv(glGetUniformLocation(phongTextureShaderProgram, "uLightColor"), 1, glm::value_ptr(lightColor));

        // 告诉着色器 uTex 采样器使用纹理单元 0
        glUniform1i(glGetUniformLocation(phongTextureShaderProgram, "uTex"), 0);

        // 激活纹理单元 0
        glActiveTexture(GL_TEXTURE0);

        // 将我们加载的纹理绑定到纹理单元 0
        glBindTexture(GL_TEXTURE_2D, textureID);

        break;

    }



    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // cleanup
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  // --- 位置控制 ---
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    objectPosition.y += cameraMoveSpeed;
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    objectPosition.y -= cameraMoveSpeed;
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    objectPosition.x -= cameraMoveSpeed;
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    objectPosition.x += cameraMoveSpeed;

  // --- 旋转控制 ---
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    objectRotation.y += 1.0f; // 绕 Y 轴旋转
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    objectRotation.y -= 1.0f; // 绕 Y 轴旋转
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    objectRotation.x += 1.0f; // 绕 X 轴旋转
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    objectRotation.x -= 1.0f; // 绕 X 轴旋转

  // --- 新增：透视控制 (Perspective Control) ---

  // FOV (F1: 减小, F2: 增大)
  if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
    fov -= 0.5f;
    if (fov < 1.0f) fov = 1.0f;
  }
  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
    fov += 0.5f;
    if (fov > 120.0f) fov = 120.0f;
  }

  // Near Plane (F3: 减小, F4: 增大)
  if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
    nearPlane -= 0.1f;
    if (nearPlane < 0.1f) nearPlane = 0.1f;
  }
  if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS) {
    nearPlane += 0.1f;
    if (nearPlane > farPlane - 1.0f) nearPlane = farPlane - 1.0f; // 确保不
  }

  // Far Plane (F5: 减小, F6: 增大)
  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
    farPlane -= 0.5f;
    if (farPlane < nearPlane + 1.0f) farPlane = nearPlane + 1.0f; // 确保不
  }
  if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS) {
    farPlane += 0.5f;
  }

  //在控制台打印变化
  static float lastFov = fov, lastNear = nearPlane, lastFar = farPlane;
  if (fov != lastFov || nearPlane != lastNear || farPlane != lastFar) {
    std::cout << "FOV: " << fov << " | Near: " << nearPlane << " | Far: " << farPlane << std::endl;
    lastFov = fov;
    lastNear = nearPlane;
    lastFar = farPlane;
  }
  // 使用一个静态变量来防止按键抖动 (按一次只触发一次)
  static bool zKeyPressed = false;
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
    if (!zKeyPressed) {
      // --- 修正：修改 currentShadingMode ---
      if (currentShadingMode != SHADE_ZBUFFER) {
        currentShadingMode = SHADE_ZBUFFER;
        std::cout << "Shading Mode: Z-Buffer" << std::endl;
      } else {
        // 如果已经在 Z-Buffer 模式，按 Z 键切换回 Phong 模式
        currentShadingMode = SHADE_PHONG;
        std::cout << "Shading Mode: Phong" << std::endl;
      }
      // --- 修正结束 ---
      zKeyPressed = true;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE) {
    zKeyPressed = false;
  }
  
  // 'N' 键 - 切换回法线 (你的原始着色器)
  static bool nKeyPressed = false;
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
    if (!nKeyPressed) {
      currentShadingMode = SHADE_NORMAL;
      std::cout << "Shading Mode: Normal (Original)" << std::endl;
      nKeyPressed = true;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) nKeyPressed = false;

  // 'G' 键 - Gouraud 着色
  static bool gKeyPressed = false;
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
    if (!gKeyPressed) {
      currentShadingMode = SHADE_GOURAUD;
      std::cout << "Shading Mode: Gouraud" << std::endl;
      gKeyPressed = true;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) gKeyPressed = false;

  // 'P' 键 - Phong 着色
  static bool pKeyPressed = false;
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
    if (!pKeyPressed) {
      currentShadingMode = SHADE_PHONG;
      std::cout << "Shading Mode: Phong" << std::endl;
      pKeyPressed = true;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) pKeyPressed = false;

// 'T' 键 - Phong + Texture 着色
  static bool tKeyPressed = false;
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    if (!tKeyPressed) {
      currentShadingMode = SHADE_PHONG_TEXTURE;
      std::cout << "Shading Mode: Phong + Texture" << std::endl;
      tKeyPressed = true;
    }
  }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) tKeyPressed = false;

}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}
bool loadOBJ(const char* path, std::vector<float>& out_vertices) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open OBJ file at " << path << std::endl;
    return false;
  }

  std::vector<glm::vec3> temp_positions;
  std::vector<glm::vec3> temp_normals;

  // 我们将在下一步中使用它，但现在先声明
  std::vector<glm::vec2> temp_uvs;

  std::vector<unsigned int> position_indices, normal_indices, uv_indices;

  std::string line;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string header;
    ss >> header;

    if (header == "v") {
      glm::vec3 position;
      ss >> position.x >> position.y >> position.z;
      temp_positions.push_back(position);
    } else if (header == "vn") {
      glm::vec3 normal;
      ss >> normal.x >> normal.y >> normal.z;
      temp_normals.push_back(normal);
    } else if (header == "vt") {
      // (为步骤 4 做准备，现在先读进来)
      glm::vec2 uv;
      ss >> uv.x >> uv.y;
      temp_uvs.push_back(uv);
    } else if (header == "f") {
      std::string vertex_str;
      unsigned int v_idx, vt_idx, vn_idx;
      char slash;

      std::vector<std::tuple<int, int, int>> face_vertices;

      while (ss >> vertex_str) {
        std::stringstream v_ss(vertex_str);

        // 尝试解析 v/vt/vn
        if (vertex_str.find("//") != std::string::npos) {
          // 格式: v//vn
          v_ss >> v_idx >> slash >> slash >> vn_idx;
          vt_idx = 0; // 没有 vt
        } else if (std::count(vertex_str.begin(), vertex_str.end(), '/') == 2) {
          // 格式: v/vt/vn
          v_ss >> v_idx >> slash >> vt_idx >> slash >> vn_idx;
        } else if (std::count(vertex_str.begin(), vertex_str.end(), '/') == 1) {
          // 格式: v/vt
          v_ss >> v_idx >> slash >> vt_idx;
          vn_idx = 0; // 没有 vn
        } else {
          // 格式: v
          v_ss >> v_idx;
          vt_idx = 0;
          vn_idx = 0;
        }

        // OBJ 索引从 1 开始，C++ 索引从 0 开始
        face_vertices.emplace_back(v_idx - 1, vt_idx - 1, vn_idx - 1);
      }

      // 将 N 边形三角化 (假设是凸多边形)
      // 我们只处理 v 和 vn
      if (face_vertices.size() >= 3) {
        // v0, v1, v2
        position_indices.push_back(std::get<0>(face_vertices[0]));
        position_indices.push_back(std::get<0>(face_vertices[1]));
        position_indices.push_back(std::get<0>(face_vertices[2]));

        normal_indices.push_back(std::get<2>(face_vertices[0]));
        normal_indices.push_back(std::get<2>(face_vertices[1]));
        normal_indices.push_back(std::get<2>(face_vertices[2]));

        // --- 新增：添加 UV 索引 ---
        uv_indices.push_back(std::get<1>(face_vertices[0]));
        uv_indices.push_back(std::get<1>(face_vertices[1]));
        uv_indices.push_back(std::get<1>(face_vertices[2]));

        // 如果是四边形 (quad)，添加第二个三角形
        if (face_vertices.size() == 4) {
          // v0, v2, v3
          position_indices.push_back(std::get<0>(face_vertices[0]));
          position_indices.push_back(std::get<0>(face_vertices[2]));
          position_indices.push_back(std::get<0>(face_vertices[3]));

          normal_indices.push_back(std::get<2>(face_vertices[0]));
          normal_indices.push_back(std::get<2>(face_vertices[2]));
          normal_indices.push_back(std::get<2>(face_vertices[3]));

          uv_indices.push_back(std::get<1>(face_vertices[0]));
          uv_indices.push_back(std::get<1>(face_vertices[2]));
          uv_indices.push_back(std::get<1>(face_vertices[3]));
        }
      }
    }
  }

  out_vertices.clear();
  // 步长是 8 (Pos, Norm, UV)
  out_vertices.reserve(position_indices.size() * 8);

  for (size_t i = 0; i < position_indices.size(); ++i) {
    unsigned int pos_idx = position_indices[i];
    unsigned int norm_idx = normal_indices[i];
    unsigned int uv_idx = uv_indices[i];

    // 1. 位置是必需的
    if (pos_idx >= temp_positions.size()) {
      continue; // 如果位置索引无效，跳过这个顶点
    }
    glm::vec3 position = temp_positions[pos_idx];

    // 2. 检查法线索引是否有效
    glm::vec3 normal;
    if (norm_idx < temp_normals.size()) {
      normal = temp_normals[norm_idx];
    } else {
      normal = glm::vec3(0.0f, 1.0f, 0.0f); // 提供一个默认法线（朝上）
    }

    // 3. 检查 UV 索引是否有效
    glm::vec2 uv;
    if (uv_idx < temp_uvs.size()) {
      uv = temp_uvs[uv_idx];
    } else {
      uv = glm::vec2(0.0f, 0.0f); // 提供一个默认 UV 坐标
    }

    // --- 现在安全地添加所有数据 ---

    // 1. 位置 (Position)
    out_vertices.push_back(position.x);
    out_vertices.push_back(position.y);
    out_vertices.push_back(position.z);

    // 2. 法线 (Normal)
    out_vertices.push_back(normal.x);
    out_vertices.push_back(normal.y);
    out_vertices.push_back(normal.z);

    // 3. UV 坐标
    out_vertices.push_back(uv.x);
    out_vertices.push_back(uv.y);
  }

  // --- 同时，修正你的打印信息，使其更准确 ---
  std::cout << "Loaded " << out_vertices.size() / 8 << " vertices (Pos+Norm+UV)" << std::endl;
  return true;
}


//
//bool loadOBJ(const char* path, std::vector<float>& out_vertices) {
//  std::ifstream file(path);
//  if (!file.is_open()) {
//    std::cerr << "Error: Could not open OBJ file at " << path << std::endl;
//    return false;
//  }
//  std::vector<glm::vec3> temp_positions;
//  std::vector<unsigned int> position_indices;
//  std::string line;
//
//  while (std::getline(file, line)) {
//    std::stringstream ss(line);
//    std::string header;
//    ss >> header;
//    if (header == "v") {
//      glm::vec3 position;
//      ss >> position.x >> position.y >> position.z;
//      temp_positions.push_back(position);
//    } else if (header == "f") {
//      std::string vertex_str;
//      std::vector<unsigned int> face_indices_per_line;
//      while (ss >> vertex_str) {
//        size_t pos = vertex_str.find('/');
//        if (pos != std::string::npos) {
//          // take the substring before the first '/'
//          face_indices_per_line.push_back(static_cast<unsigned int>(std::stoul(vertex_str.substr(0, pos))));
//        } else {
//          face_indices_per_line.push_back(static_cast<unsigned int>(std::stoul(vertex_str)));
//        }
//      }
//      if (face_indices_per_line.size() == 4) {
//        // triangulate quad: (v0, v1, v2) and (v0, v2, v3)
//        position_indices.push_back(face_indices_per_line[0] - 1);
//        position_indices.push_back(face_indices_per_line[1] - 1);
//        position_indices.push_back(face_indices_per_line[2] - 1);
//        position_indices.push_back(face_indices_per_line[0] - 1);
//        position_indices.push_back(face_indices_per_line[2] - 1);
//        position_indices.push_back(face_indices_per_line[3] - 1);
//      } else if (face_indices_per_line.size() == 3) {
//        // triangle
//        position_indices.push_back(face_indices_per_line[0] - 1);
//        position_indices.push_back(face_indices_per_line[1] - 1);
//        position_indices.push_back(face_indices_per_line[2] - 1);
//      }
//    }
//  }
//
//  out_vertices.clear();
//  out_vertices.reserve(position_indices.size() * 6);
//
//  for (unsigned int position_index : position_indices) {
//    if (position_index < temp_positions.size()) {
//      glm::vec3 position = temp_positions[position_index];
//      // position
//      out_vertices.push_back(position.x);
//      out_vertices.push_back(position.y);
//      out_vertices.push_back(position.z);
//      // simple position-based color [0,1]
//      float r = (position.x + 1.0f) * 0.5f;
//      float g = (position.y + 1.0f) * 0.5f;
//      float b = (position.z + 1.0f) * 0.5f;
//      out_vertices.push_back(r);
//      out_vertices.push_back(g);
//      out_vertices.push_back(b);
//    }
//  }
//  return true;
//}

// --- 新增：纹理加载函数 ---
unsigned int loadTexture(const char* path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  stbi_set_flip_vertically_on_load(true); // 看你的UV坐标是否需要翻转
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID); //
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); //
    glGenerateMipmap(GL_TEXTURE_2D); //

    // 设置纹理环绕和过滤参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    std::cout << "Successfully loaded texture at " << path << std::endl;
  }
  else
  {
    std::cout << "Texture failed to load at " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;
}