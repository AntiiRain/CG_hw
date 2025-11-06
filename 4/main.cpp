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

//resvise your basepath
const std::string BASE_PATH = "I:/Code/CG/CG_hw/4/";
glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
glm::vec3 objectRotation(0.0f, 0.0f, 0.0f); // 存储欧拉角
float cameraMoveSpeed = 0.05f;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool loadOBJ(const char* path, std::vector<float>& out_vertices);

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

  glfwSwapInterval(0);
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

  // load OBJ -> interleaved positions (3) + colors (3)
  std::string objPath = BASE_PATH + "data/cube.obj";
//  std::vector<float> vertices;
  std::vector<float> vertices1;
  std::vector<float> vertices2;


  // 加载第一个对象
  std::string objPath1 = BASE_PATH + "data/cube.obj"; // 确保文件名正确
//  std::string objPath1 = BASE_PATH + "data/f-16.obj"; //for cpu vs gpu testing
  bool success1 = loadOBJ(objPath1.c_str(), vertices1);
  if (!success1) return -1;
  int numVertices1 = static_cast<int>(vertices1.size() / 6);
  std::cout << "Successfully loaded object 1 with " << numVertices1 << " vertices." << std::endl;

// 加载第二个对象
  std::string objPath2 = BASE_PATH + "data/cubetest.obj"; // 确保文件名正确
//  std::string objPath2 = BASE_PATH + "data/f-16.obj"; // for cpu vs gpu testing
  bool success2 = loadOBJ(objPath2.c_str(), vertices2);
  if (!success2) return -1;
  int numVertices2 = static_cast<int>(vertices2.size() / 6);
  std::cout << "Successfully loaded object 2 with " << numVertices2 << " vertices." << std::endl;


//  bool load_success = loadOBJ(objPath.c_str(), vertices);
//  if (!load_success) {
//    return -1;
//  }
//  int numVertices = static_cast<int>(vertices.size() / 6);
//  std::cout << "Successfully loaded " << numVertices << " vertices." << std::endl;
//  if (numVertices == 0) {
//    std::cout << "No vertices parsed. Exiting." << std::endl;
//    return -1;
//  }

//  // create VBO/VAO
//  unsigned int VBO, VAO;
//  glGenVertexArrays(1, &VAO);
//  glGenBuffers(1, &VBO);
//
//  glBindVertexArray(VAO);
//  glBindBuffer(GL_ARRAY_BUFFER, VBO);
//  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
//
//  // position attribute
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//  glEnableVertexAttribArray(0);
//  // color attribute
//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//  glEnableVertexAttribArray(1);
//
//  glBindBuffer(GL_ARRAY_BUFFER, 0);
//  glBindVertexArray(0);


// (在 main 函数中，加载完数据之后)

// --- 为对象1创建 VAO/VBO ---
  unsigned int VBO1, VAO1;
  glGenVertexArrays(1, &VAO1);
  glGenBuffers(1, &VBO1);

  glBindVertexArray(VAO1);
  glBindBuffer(GL_ARRAY_BUFFER, VBO1);
  glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(float), vertices1.data(), GL_STATIC_DRAW);

// 设置顶点属性指针 (位置和颜色)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);


// --- 为对象2创建 VAO/VBO ---
  unsigned int VBO2, VAO2;
  glGenVertexArrays(1, &VAO2);
  glGenBuffers(1, &VBO2);

  glBindVertexArray(VAO2);
  glBindBuffer(GL_ARRAY_BUFFER, VBO2);
  glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(float), vertices2.data(), GL_STATIC_DRAW);

// 设置顶点属性指针 (位置和颜色)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

// 解绑
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);


  // uniform location
  GLint uMVPLoc = glGetUniformLocation(shaderProgram, "uMVP");


  double lastTime = glfwGetTime();
  int nbFrames = 0;
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

//    // build MVP
//    glm::mat4 model = glm::mat4(1.0f);
//    // 1. 应用平移
//    model = glm::translate(model, objectPosition);
//    // 2. 应用旋转 (注意顺序)
//    model = glm::rotate(model, glm::radians(objectRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
//    model = glm::rotate(model, glm::radians(objectRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(0.5f));
//
////    model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);


    glm::vec3 p0(-1.5f, 0.0f, 0.0f); // 物体1的中心
    glm::vec3 p1(1.5f, 0.0f, 0.0f);  // 物体2的中心

    //计算旋转轴 (从 p0 指向 p1 的归一化向量)
    glm::vec3 rotationAxis = glm::normalize(p1 - p0);

    // 获取随时间变化的旋转角度
    float angle = static_cast<float>(glfwGetTime()) * 2.0f; // 乘以2.0f让它转得快一点

    // 构建共享的旋转矩阵 (Axis-Angle Rotation)
    // 这就是您在线性代数部分推导的矩阵
    glm::mat4 sharedRotation = glm::rotate(glm::mat4(1.0f), angle, rotationAxis);






//    glm::mat4 mvp = proj * view * model;



    // --- 绘制对象1 ---
    // 先定位（平移和缩放），但不旋转
    glm::mat4 placement1 = glm::translate(glm::mat4(1.0f), p0);
    placement1 = glm::scale(placement1, glm::vec3(0.5f));
    // 应用共享的旋转
    glm::mat4 model1 = sharedRotation * placement1;

    glm::mat4 mvp1 = proj * view * model1;

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp1));

    glBindVertexArray(VAO1);
    glDrawArrays(GL_TRIANGLES, 0, numVertices1);


    // --- 绘制对象2 ---
    // 先定位（平移和缩放），但不旋转
    glm::mat4 placement2 = glm::translate(glm::mat4(1.0f), p1);
    placement2 = glm::scale(placement2, glm::vec3(0.5f));
    // 应用同一个共享的旋转
    glm::mat4 model2 = sharedRotation * placement2;

    glm::mat4 mvp2 = proj * view * model2;

    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp2));

    glBindVertexArray(VAO2);
    glDrawArrays(GL_TRIANGLES, 0, numVertices2);




//    // draw
//    glUseProgram(shaderProgram);
//    glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp));
//
//    glBindVertexArray(VAO);
//    glDrawArrays(GL_TRIANGLES, 0, numVertices);

    glfwSwapBuffers(window);
    glfwPollEvents();

    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){ // 每秒打印一次
      std::cout << nbFrames << " FPS" << std::endl;
      nbFrames = 0;
      lastTime += 1.0;
    }
  }

  // cleanup
//  glDeleteVertexArrays(1, &VAO);
//  glDeleteBuffers(1, &VBO);
//  glDeleteProgram(shaderProgram);

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
          // take the substring before the first '/'
          face_indices_per_line.push_back(static_cast<unsigned int>(std::stoul(vertex_str.substr(0, pos))));
        } else {
          face_indices_per_line.push_back(static_cast<unsigned int>(std::stoul(vertex_str)));
        }
      }
      if (face_indices_per_line.size() == 4) {
        // triangulate quad: (v0, v1, v2) and (v0, v2, v3)
        position_indices.push_back(face_indices_per_line[0] - 1);
        position_indices.push_back(face_indices_per_line[1] - 1);
        position_indices.push_back(face_indices_per_line[2] - 1);
        position_indices.push_back(face_indices_per_line[0] - 1);
        position_indices.push_back(face_indices_per_line[2] - 1);
        position_indices.push_back(face_indices_per_line[3] - 1);
      } else if (face_indices_per_line.size() == 3) {
        // triangle
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