cmake_minimum_required(VERSION 3.27)
project(hw4)

set(CMAKE_CXX_STANDARD 17)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)


add_executable(hw4 main.cpp
)

target_link_libraries(hw4 PRIVATE
        glad::glad
        glfw
        glm::glm
)
