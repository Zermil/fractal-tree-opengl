cmake_minimum_required(VERSION 3.5)

project(fractal-tree-opengl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
link_directories(${OPENGL_INCLUDE_DIR})

find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
link_directories(${GLFW_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})
link_directories(${GLEW_INCLUDE_DIR})

list(APPEND LIB_LIST
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${GLFW_LIBRARIES})

add_executable(fractal-tree-opengl code/main.cpp)

target_link_libraries(fractal-tree-opengl ${LIB_LIST} glfw)

install(TARGETS fractal-tree-opengl
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR})