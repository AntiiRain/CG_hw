
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

int main() {
  glfwInit();
  // ... 你的 OpenGL 代码
  std::cout << "GLFW 和 GLM 初始化成功!" << std::endl;
  glfwTerminate();
  return 0;
}