// Kody Manastyrski
// Display.cpp
//

#include "ogl.hpp"
#include "Shader.hpp"
#include "Equation.hpp"
#include "Enums.hpp"

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <numeric>

using Eigen::VectorXd;
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix4f;

#ifndef DISPLAY
#include "Display.hpp"

bool write = false;

void Display::draw(){

	draw_state types[4] = {point,line,line,point};
	Vector4f colour[2] = {{255,255,0,255}, {255,0,255,1}};



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int i = 0; i < 4; i += 1){

		shader.activate();
		if(i < 2){
			shader.setUniform(matrixID, (void *)primal.data(), 1);
			shader.setUniform(colourID, (void *)colour[0].data(), 2);
		}
		else{
			shader.setUniform(matrixID, (void *)dual.data(), 1);
			shader.setUniform(colourID, (void *)colour[1].data(), 2);
		}

		ogl::draw(
				window, // screen
				buffers[i], // buffer
			   	counts[i], // count
				types[i], // type
				indices[i],  // indices
				0); // layout variable
	}

	glfwSwapBuffers(window);
	glfwPollEvents();
}

Display::Display(DataModel * m){

	width = 1080;
	height = 608;
	window = setup(width, height);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader = Shader("primal");

	model = m;

	backgroundColour = VectorXd(4);
	backgroundColour << 1, 1, 1, 1;
	shader.activate();
	for(int i = 0; i < 4; i += 1){
		glGenBuffers(1, &buffers[i]);
		glGenBuffers(1, &indices[i]);
	}

	matrixID = shader.getUniform("mvp");
	colourID = shader.getUniform("colour");

	recreateMatrices();

	glViewport(0,0, width, height);
	glPointSize(4);
}


void Display::updatePoints(){
	write = true;

	GLuint pointBuffer = buffers[0];
	GLuint pointIndices = indices[0];

	ogl::bind(pointBuffer, model->getDataPointer(0),
			pointIndices, model->getIndexPointer(0));

	GLuint dualPointBuffer = buffers[2];
	GLuint dualIndices = indices[2];

	ogl::bind(dualPointBuffer, model->getDataPointer(2),
			dualIndices, model->getIndexPointer(2));
	counts[0] = model->getPointCount();
	counts[2] = 2*model->getPointCount();
	redraw();
}

void Display::updateEquations(){
	write = true;

	GLuint eqnBuffer = buffers[1];
	GLuint eqnIndices = indices[1];
	ogl::bind(eqnBuffer, model->getDataPointer(1), 
			eqnIndices, model->getIndexPointer(1));

	GLuint dualEqnBuffer = buffers[3];
	GLuint dualEqnIndices = indices[3];

	ogl::bind(dualEqnBuffer, model->getDataPointer(3),
			dualEqnIndices, model->getIndexPointer(3));
	
	counts[1] = 2*model->getEquationCount();
	counts[3] = model->getEquationCount();
	redraw();
}


void Display::redraw(){
	draw();
}

void Display::mainLoop(){
	do{
		draw();
	}while( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
}


GLFWwindow * Display::getWindow(){
	return window;
}


void Display::recreateMatrices(){
	Matrix4f model_left, model_right;
	model_left 		<< 	width,0,0,0,
			   			0,height/2.f,0,0,
			   			0,0,1,0,
			   			0,0,0,1;

	model_right 	<< 	width/4.f,0,0,width/16.f,
						0,height/4.f,0,-height / 4.f,
						0,0,0,0,
						0,0,0,1;


	Vector3f position, up, target;
	position << 0,0,-1;
	up << 0,1,0;
	target << 0,0,1;

	Vector3f rgt = position.cross(up).normalized();


	Matrix4f view;
	view << 
		rgt(0), 	rgt(1), 	rgt(2), 	-rgt.dot(position),
		up(0), 		up(1), 		up(2), 		-up.dot(position),
		target(0), 	target(1), 	target(2), 	target.dot(position),
		0, 			0, 			0, 			1;


	
	float left = 	-(float)width/2.f;
	float right = 	(float)width/2.f;

	float bottom = 	-(float)height/2.f;
	float top = 	(float)height/2.f;

	float near = 	-0.5f;
	float far = 	1.f;

	float a = -(right+left)/(right-left);
	float b = -(bottom+top)/(bottom - top);
	float c = -(near + far)/(near-far);


	Matrix4f projection;
	projection 	<< 			2.f/(right - left),		0, 	0, 	a,
							0, 	2.f/(top - bottom), 	0, 	b,
							0, 	0, 	1.f/(near - far), 	c,
							0, 	0, 	0, 	1;

							

	primal 	= projection 	* view	* model_left;
	dual 	= projection 	* view 	* model_right;


}


void Display::updateWidth(int w){
	width = w;
	recreateMatrices();
}

void Display::updateHeight(int h){
	height = h;
	recreateMatrices();
}

Point Display::getMousePosition(){
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return Point(x, y);
}

#endif
