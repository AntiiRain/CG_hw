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

// --- 用户配置 ---
// 请务必将此路径修改为您项目所在的绝对路径
const std::string BASE_PATH = "I:/Code/CG/CG_hw/3/";


// --- 函数声明 ---
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool loadOBJ(const char* path, std::vector<float>& out_vertices);
std::string readShaderFile(const std::string& filePath);

// --- 全局变量 ---
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
  // --- 1. 初始化 GLFW, GLAD ---
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CPU Performance Test", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSwapInterval(0);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // --- 2. 编译着色器 ---
  // 使用专为 CPU 变换设计的顶点着色器
  std::string vsPath = BASE_PATH + "shader_cpu.vs";
  std::string fsPath = BASE_PATH + "shader.fs";
  std::string vertexShaderCode = readShaderFile(vsPath);
  std::string fragmentShaderCode = readShaderFile(fsPath);
  if (vertexShaderCode.empty() || fragmentShaderCode.empty()) {
    return -1;
  }

  const char* vertexShaderSource = vertexShaderCode.c_str();
  const char* fragmentShaderSource = fragmentShaderCode.c_str();

  // 编译过程...
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
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


  // --- 3. 加载模型数据 ---
  std::vector<float> vertices1, vertices2;
  // 加载第一个对象
  std::string objPath1 = BASE_PATH + "data/f-16.obj";
  if (!loadOBJ(objPath1.c_str(), vertices1)) return -1;
  int numVertices1 = static_cast<int>(vertices1.size() / 6);
  std::cout << "Successfully loaded object 1 with " << numVertices1 << " vertices." << std::endl;

  // 加载第二个对象
  std::string objPath2 = BASE_PATH + "data/f-16.obj";
  if (!loadOBJ(objPath2.c_str(), vertices2)) return -1;
  int numVertices2 = static_cast<int>(vertices2.size() / 6);
  std::cout << "Successfully loaded object 2 with " << numVertices2 << " vertices." << std::endl;


  // --- 4. 创建 VAO 和 VBO ---
  // 对象1
  unsigned int VBO1, VAO1;
  glGenVertexArrays(1, &VAO1);
  glGenBuffers(1, &VBO1);
  glBindVertexArray(VAO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  // 注意这里我们使用 GL_DYNAMIC_DRAW，因为 VBO 的内容会每帧都更新
  glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(float), vertices1.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // 对象2
  unsigned int VBO2, VAO2;
  glGenVertexArrays(1, &VAO2);
  glGenBuffers(1, &VBO2);
  glBindVertexArray(VAO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), vertices2.data(), GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // 解绑
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // 用于计算FPS
  double lastTime = glfwGetTime();
  int nbFrames = 0;

  // --- 5. 渲染循环 ---
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 计算相机和投影矩阵
    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    float aspect = (fbh > 0) ? (static_cast<float>(fbw) / static_cast<float>(fbh)) : 1.3333f;
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    // 计算模型矩阵 (共享旋转部分)
    glm::vec3 p0(-1.5f, 0.0f, 0.0f);
    glm::vec3 p1(1.5f, 0.0f, 0.0f);
    glm::vec3 rotationAxis = glm::normalize(p1 - p0);
    float angle = static_cast<float>(glfwGetTime()) * 2.0f;
    glm::mat4 sharedRotation = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);

    // 构建两个物体的完整 MVP 矩阵
    glm::mat4 placement1 = glm::translate(glm::mat4(1.0f), p0);
    placement1 = glm::scale(placement1, glm::vec3(0.5f));
    glm::mat4 model1 = sharedRotation * placement1;
    glm::mat4 mvp1 = proj * view * model1;

    glm::mat4 placement2 = glm::translate(glm::mat4(1.0f), p1);
    placement2 = glm::scale(placement2, glm::vec3(0.5f));
    glm::mat4 model2 = sharedRotation * placement2;
    glm::mat4 mvp2 = proj * view * model2;


    // === CPU 性能测试核心部分 ===

    std::vector<float> transformed_vertices1(vertices1.size());
    for (int i = 0; i < numVertices1; ++i) {
      glm::vec4 original_pos(vertices1[i * 6], vertices1[i * 6 + 1], vertices1[i * 6 + 2], 1.0f);
      glm::vec4 transformed_pos = mvp1 * original_pos;

      // !!! --- 关键修复：在这里执行透视除法 --- !!!
      if (transformed_pos.w != 0.0f) {
        transformed_pos.x /= transformed_pos.w;
        transformed_pos.y /= transformed_pos.w;
        transformed_pos.z /= transformed_pos.w;
      }

      // 将变换并经过透视除法后的 NDC 坐标存入
      transformed_vertices1[i * 6] = transformed_pos.x;
      transformed_vertices1[i * 6 + 1] = transformed_pos.y;
      transformed_vertices1[i * 6 + 2] = transformed_pos.z;

      // 复制颜色数据
      transformed_vertices1[i * 6 + 3] = vertices1[i * 6 + 3];
      transformed_vertices1[i * 6 + 4] = vertices1[i * 6 + 4];
      transformed_vertices1[i * 6 + 5] = vertices1[i * 6 + 5];
    }

// --- 处理对象 2 ---
    std::vector<float> transformed_vertices2(vertices2.size());
    for (int i = 0; i < numVertices2; ++i) {
      glm::vec4 original_pos(vertices2[i * 6], vertices2[i * 6 + 1], vertices2[i * 6 + 2], 1.0f);
      glm::vec4 transformed_pos = mvp2 * original_pos;

      // !!! --- 关键修复：在这里执行透视除法 --- !!!
      if (transformed_pos.w != 0.0f) {
        transformed_pos.x /= transformed_pos.w;
        transformed_pos.y /= transformed_pos.w;
        transformed_pos.z /= transformed_pos.w;
      }

      transformed_vertices2[i * 6] = transformed_pos.x;
      transformed_vertices2[i * 6 + 1] = transformed_pos.y;
      transformed_vertices2[i * 6 + 2] = transformed_pos.z;
      transformed_vertices2[i * 6 + 3] = vertices2[i * 6 + 3];
      transformed_vertices2[i * 6 + 4] = vertices2[i * 6 + 4];
      transformed_vertices2[i * 6 + 5] = vertices2[i * 6 + 5];
    }

    glUseProgram(shaderProgram);

    // --- 绘制对象 1 ---
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    // 将变换后的顶点数据上传到 VBO
    glBufferSubData(GL_ARRAY_BUFFER, 0, transformed_vertices1.size() * sizeof(float), transformed_vertices1.data());
    glDrawArrays(GL_TRIANGLES, 0, numVertices1);

    // --- 绘制对象 2 ---
    glBindVertexArray(VAO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferSubData(GL_ARRAY_BUFFER, 0, transformed_vertices2.size() * sizeof(float), transformed_vertices2.data());
    glDrawArrays(GL_TRIANGLES, 0, numVertices2);

    // 解绑
    glBindVertexArray(0);


    // 交换缓冲区并检查事件
    glfwSwapBuffers(window);
    glfwPollEvents();

    // 计算并打印 FPS
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){
      std::cout << nbFrames << " FPS" << std::endl;
      nbFrames = 0;
      lastTime += 1.0;
    }
  }

  // --- 6. 清理资源 ---
  glDeleteVertexArrays(1, &VAO1);
  glDeleteBuffers(1, &VBO1);
  glDeleteVertexArrays(1, &VAO2);
  glDeleteBuffers(1, &VBO2);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}


// --- 辅助函数定义 ---

// (processInput, framebuffer_size_callback, loadOBJ, readShaderFile 函数与 main.cpp 中的完全相同)
// ... 请将 main.cpp 中这些函数的代码复制到这里 ...

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  // 此处可以添加其他输入控制
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

std::string readShaderFile(const std::string& filePath) {
  std::ifstream shaderFile;
  shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    shaderFile.open(filePath);
    std::stringstream shaderStream;
    shaderStream << shaderFile.rdbuf();
    shaderFile.close();
    return shaderStream.str();
  } catch (std::ifstream::failure& e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << filePath << std::endl;
    return "";
  }
}


bool loadOBJ(const char* path, std::vector<float>& out_vertices) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cerr << "Error: Could not open OBJ file at " << path << std::endl;
    return false;
  }
  std::vector<glm::vec3> temp_positions;
  std::vector<unsigned int> position_indices;
  std::string line;

  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string header;
    ss >> header;
    if (header == "v") {
      glm::vec3 position;
      ss >> position.x >> position.y >> position.z;
      temp_positions.push_back(position);
    } else if (header == "f") {
      std::string vertex_str;
      std::vector<unsigned int> face_indices_per_line;
      while (ss >> vertex_str) {
        size_t pos = vertex_str.find('/');
        if (pos != std::string::npos) {
          face_indices_per_line.push_back(static_cast<unsigned int>(std::stoul(vertex_str.substr(0, pos))));
        } else {
          face_indices_per_line.push_back(static_cast<unsigned int>(std::stoul(vertex_str)));
        }
      }
      if (face_indices_per_line.size() == 4) {
        position_indices.push_back(face_indices_per_line[0] - 1);
        position_indices.push_back(face_indices_per_line[1] - 1);
        position_indices.push_back(face_indices_per_line[2] - 1);
        position_indices.push_back(face_indices_per_line[0] - 1);
        position_indices.push_back(face_indices_per_line[2] - 1);
        position_indices.push_back(face_indices_per_line[3] - 1);
      } else if (face_indices_per_line.size() == 3) {
        position_indices.push_back(face_indices_per_line[0] - 1);
        position_indices.push_back(face_indices_per_line[1] - 1);
        position_indices.push_back(face_indices_per_line[2] - 1);
      }
    }
  }

  out_vertices.clear();
  out_vertices.reserve(position_indices.size() * 6);

  for (unsigned int position_index : position_indices) {
    if (position_index < temp_positions.size()) {
      glm::vec3 position = temp_positions[position_index];
      // position
      out_vertices.push_back(position.x);
      out_vertices.push_back(position.y);
      out_vertices.push_back(position.z);
      // simple position-based color [0,1]
      float r = (position.x + 1.0f) * 0.5f;
      float g = (position.y + 1.0f) * 0.5f;
      float b = (position.z + 1.0f) * 0.5f;
      out_vertices.push_back(r);
      out_vertices.push_back(g);
      out_vertices.push_back(b);
    }
  }
  return true;
}