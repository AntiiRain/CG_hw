
cmake_minimum_required(VERSION 3.10)
project(02)
set(CMAKE_CXX_STANDARD 14)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(02 main.cpp)

add_executable(animator  animation.cpp)

target_compile_definitions(animator PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(02 PRIVATE
        glfw
        GLEW::glew
        glm::glm
)

target_link_libraries(animator PRIVATE
        glm::glm
)