cmake_minimum_required(VERSION 3.10)
project(MyOpenGLProject)
set(CMAKE_CXX_STANDARD 14)


# 使用 vcpkg 查找所有需要的库
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(MyOpenGLProject main.cpp)

add_executable(test test.cpp)



target_link_libraries(MyOpenGLProject PRIVATE
        glfw
        GLEW::glew
        glm::glm
)

target_link_libraries(test PRIVATE
        glfw
        GLEW::glew
        glm::glm
)
