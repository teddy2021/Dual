cmake_minimum_required(VERSION 3.8)

project(DualPlane)

set(CMAKE_MODULE_PATH /home/kody/Libs/cmake)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(OpenGL_GL_PREFERENCE GLVND)
set(CMAKE_C_FLAGS -fprofile-arcs )
set(CMAKE_C_FLAGS -ftest-coverage)
set(CMAKE_EXE_LINKER_FLAGS -lgcov )
set(CMAKE_EXE_LINKER_FLAGS --coverage)
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )


add_executable(test
	test.cpp
	Equation.hpp Equation.cpp LinearEquation.cpp
	Point.hpp Point.cpp
	Util.hpp Util.cpp	
)

add_executable(dual
	dual.cpp
	DataModel.hpp DataModel.cpp
	Display.hpp Display.cpp
	ogl.hpp ogl.cpp
	Point.hpp Point.cpp
	Equation.hpp Equation.cpp LinearEquation.cpp
	Axis.hpp Axis.cpp
	Control.hpp Control.cpp
	Util.hpp Util.cpp
	Shader.hpp Shader.cpp
)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(dual ${GLFW_LIBRARIES})
message("GLFW variables: " ${GLFW_LIBRARIES} ${GLFW_INCLUDE_DIRS})

find_package(glm)
include_directories(${GLM_INCLUDE_DIRS})
target_link_libraries(dual ${GLM_LIBRARIES})
message("GLM variables: " ${GLM_LIBRARIES} ${GLM_INCLUDE_DIRS})

find_package(GLEW)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(dual ${GLEW_LIBRARIES})
message("GLEW variables: " ${GLEW_LIBRARIES} ${GLEW_INCLUDE_DIRS})

find_package(OpenGL)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(dual OpenGL::GL)
target_link_libraries(dual GLU)
message("OpenGL variables: " ${OPENGL_INCLUDE_DIRS})

find_package(Eigen3 3.3)
include_directories(${EIGEN_INCLUDE_DIRS})
target_link_libraries(dual Eigen3::Eigen)
message("Eigen variables: " ${EIGEN_INCLUDE_DIRS} Eigen3::Eigen)


add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Headers.txt, 
					COMMAND for x in ${CMAKE_CURRENT_SOURCE_DIR}/* do echo "$x" done,
						DEPENDS Axis.hpp Control.hpp DataModel.hpp Display.hpp Equation.hpp Point.hpp Shader.hpp Util.hpp ogl.hpp,
						USES_TERMINAL)
