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

SET(_glm_HEADER_SEARCH_DIRS
		"/usr/include"
		"/usr/local/include")

# check environment variable
SET(_glm_ENV_ROOT_DIR "$ENV{GLM_ROOT_DIR}")

IF(NOT GLM_ROOT_DIR AND _glm_ENV_ROOT_DIR)
	SET(GLM_ROOT_DIR "${_glm_ENV_ROOT_DIR}")
ENDIF(NOT GLM_ROOT_DIR AND _glm_ENV_ROOT_DIR)

# put user specified location at beginning of search
IF(GLM_ROOT_DIR)
	SET(_glm_HEADER_SEARCH_DIRS "${GLM_ROOT_DIR}"
			"${GLM_ROOT_DIR}/include"
			${_glm_HEADER_SEARCH_DIRS})
ENDIF(GLM_ROOT_DIR)

# locate header
FIND_PATH(GLM_INCLUDE_DIR "glm/glm.hpp"
		PATHS ${_glm_HEADER_SEARCH_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLM DEFAULT_MSG
		GLM_INCLUDE_DIR)

IF(GLM_FOUND)
	SET(GLM_INCLUDE_DIRS "${GLM_INCLUDE_DIR}")

	MESSAGE(STATUS "GLM_INCLUDE_DIR = ${GLM_INCLUDE_DIR}")
ENDIF(GLM_FOUND)
include_directories(${GLM_INCLUDE_DIR})
#执行编译命令

set(SOURCE_FILES lib/hdrloader.cpp main.cpp)

add_executable(RayTrace ${SOURCE_FILES})

if (APPLE)
	target_link_libraries(RayTrace "-framework OpenGL")
	target_link_libraries(RayTrace "-framework GLUT")
endif()

#target_link_libraries(RayTrace glm::glm)