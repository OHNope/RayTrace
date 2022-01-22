cmake_minimum_required(VERSION 3.13)

project(RayTrace)

set(CMAKE_CXX_STANDARD 11)

# 添加头文件

set(GLEW_H /usr/local/Cellar/glew/2.2.0_1/include)#GL
set(FREEGLUT_H /usr/local/Cellar/freeglut/3.2.1_1/include)#FREEGLUT
set(GLFW_H /usr/local/cellar/glfw/3.3.6/include)#GLFW

include_directories(${GLEW_H} ${FREEGLUT_H} ${GLFW_H})

# 添加目标链接

set(GLEW_LINK /usr/local/Cellar/glew/2.2.0_1//lib/libGLEW.dylib)
set(FREEGLUT_LINK /usr/local/Cellar/freeglut/3.2.1_1/lib/libglut.dylib)
set(GLFW_LINK /usr/local/cellar/glfw/3.3.6/lib/libglfw.dylib)

link_libraries(${OPENGL} ${GLEW_LINK} ${FREEGLUT_LINK} ${GLFW_LINK})

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})
link_directories(${GLM_LIBRARY_DIR})

if(NOT GLM_FOUND)
    message(Error "GLM not found")
endif(NOT GLM_FOUND)
#执行编译命令

set(SOURCE_FILES lib/hdrloader.cpp main.cpp)

add_executable(RayTrace ${SOURCE_FILES})

if (APPLE)
    target_link_libraries(RayTrace "-framework OpenGL")
    target_link_libraries(RayTrace "-framework GLUT")
endif()

target_link_libraries(RayTrace glm::glm)