// Kody Manastyrski
// ogl.hpp
//

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#ifndef ogl
#define ogl

std::vector<GLuint> index(int count);

void genVBO(GLuint &buffer, GLuint &idxs, void * data, 
		std::vector<GLuint> indices, int count, short type);

GLFWwindow * setup(int width, int height);

int draw(GLFWwindow * window, GLuint buffer, 
		GLuint count, short type, GLuint indices, int array_no);

#endif
