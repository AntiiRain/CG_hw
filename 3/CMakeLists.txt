cmake_minimum_required(VERSION 3.27)
project(3)

set(CMAKE_CXX_STANDARD 17)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)


add_executable(3 main.cpp)

target_link_libraries(3 PRIVATE
        glad::glad
        glfw
        glm::glm
)






