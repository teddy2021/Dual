// Kody Manastyrski
// ogl.hpp
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <eigen3/Eigen/Dense>
#include <vector>
#include "Enums.hpp"

#ifndef ogl
#define ogl

std::vector<GLuint> index(int count);

void genVBO(GLuint &buffer, GLuint &idxs, void * data, 
		std::vector<GLuint> indices, int count, short type);

GLFWwindow * setup(int width, int height);

int draw(GLFWwindow * window, GLuint buffer, 
		GLuint count, draw_state type, GLuint indices, int array_no);

void bind(GLuint v_buffer, std::vector<Eigen::Vector2f> * vertices,
		GLuint i_buffer, std::vector<GLuint> * indices);

#endif
