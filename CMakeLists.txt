cmake_minimum_required(VERSION 2.6)
project (OpenGL)

set (CMAKE_CXX_STANDARD 11)

# include directory
include_directories(
	src/include
	/usr/local/include/bullet
)

# source directory
file(GLOB_RECURSE SRCS src/*.cpp)

# 3rd-party library
find_library(GLFW glfw3)
find_library(IOKIT IOKit)
find_library(COCOA Cocoa)
find_library(COREVIDEO CoreVideo)
find_library(GLEW glew)
find_library(OPENGL OpenGL)
find_library(ASSIMP assimp)
find_library(ANTTWEAKBAR anttweakbar)
find_package(Bullet REQUIRED)

SET(EXTRA_LIBS 	
	${GLFW}
	${IOKIT}
	${COCOA}
	${COREVIDEO}
	${GLEW}
	${OPENGL}
	${ASSIMP}
	${ANTTWEAKBAR}
	${BULLET_LIBRARIES}
)

# compile
add_executable(OpenGL ${SRCS})

# link
TARGET_LINK_LIBRARIES(OpenGL ${EXTRA_LIBS})

FILE(GLOB GLSL shaders/*)
FILE(COPY ${GLSL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/shaders)

FILE(GLOB GLSL textures/*)
FILE(COPY ${GLSL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/textures)

FILE(GLOB GLSL models/*)
FILE(COPY ${GLSL} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/models)

# add_definitions(-DDEBUG)