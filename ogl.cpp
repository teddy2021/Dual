// Kody Manastyrski
//./ogl.cpp
//

#include "Enums.hpp"
#include "Equation.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>
#include <vector>
#include <fstream>

#include <eigen3/Eigen/Dense>


using std::vector;

using Eigen::Vector2f;
using Eigen::Vector4f;

#ifndef ogl
#include "ogl.hpp"





GLFWwindow * setup(int width, int height){

	if(!glfwInit()){
		perror("Failed to initialize glfw3");
		return NULL;
	}


	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window;
	window = glfwCreateWindow(width, height, "Dual Visualizer V 1.0", 
			NULL, NULL);
	if(NULL == window){
		perror("Failed to create window");
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if(glewInit() != GLEW_OK){
		perror("Failed to init glew");
		return NULL;
	}


	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0,0,0,1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return window;
}

std::vector<GLuint> index(int count){
	std::vector<GLuint> indices;
	for(int i = 0; i < count; i += 1){
		indices.push_back(i);	
	}
	return indices;
}

void genVBO(GLuint &buffer, GLuint &idxs, void * data, 
		std::vector<GLuint> indices, int count, short type){

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	unsigned int size;
	switch(type){
		case 1:
			size = sizeof(GLuint *);
			break;
		case 2:
			size = sizeof(Vector2f);
			break;
		case 3:
			size = sizeof(GLfloat *);
			break;
	}
	glBufferData(GL_ARRAY_BUFFER, size * count, data, GL_STATIC_DRAW);


	glGenBuffers(1, &idxs);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idxs);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);
}

int draw(GLFWwindow * window, GLuint buffer, 
		GLuint count, draw_state type, GLuint indices,
		int array_no){

	

	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
	
	glEnableVertexAttribArray(array_no);
	switch(type){
		case point:
			glVertexAttribPointer(
					array_no, 
					2,
					GL_FLOAT,
					GL_FALSE,
					0,
					(void*)0
					);
			glDrawElements(
				GL_POINTS,
				count,
				GL_UNSIGNED_INT,
				(void*)0
				);
			break;
		case line:
			glVertexAttribPointer(
					array_no, 
					2,
					GL_FLOAT,
					GL_FALSE,
					0,
					(void*)0
					);
			glDrawElements(
				GL_LINES,
				count,
				GL_UNSIGNED_INT,
				(void*)0
				);
			break;
		case polygon:
			glVertexAttribPointer(
					array_no, 
					3,
					GL_FLOAT,
					GL_FALSE,
					0,
					(void*)0
					);
			glDrawElements(
				GL_TRIANGLES,
				count,
				GL_UNSIGNED_INT,
				(void*)0
				);
			break;
	}
	

	glDisableVertexAttribArray(array_no);

	return 0;
}


void bind(GLuint v_buffer, vector<Vector2f> * vertices, 
		GLuint i_buffer, vector<GLuint> *indices){

	glBindBuffer(GL_ARRAY_BUFFER, v_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * vertices->size(), 
			vertices->data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(),
			indices->data(), GL_STATIC_DRAW);
}


#endif
