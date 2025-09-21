
cmake_minimum_required(VERSION 3.10)
project(02)
set(CMAKE_CXX_STANDARD 14)

# 使用 vcpkg 查找所有需要的库
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(02 main.cpp)


target_link_libraries(02 PRIVATE
        glfw
        GLEW::glew
        glm::glm
)
