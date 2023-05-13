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

	GLuint s_buf, s_idx;
	
	Vector2f square[4] = {
		{1.f,1.f},
		{1.f,-1.f},
		{-1.f,-1.f},
		{-1.f,1.f}
	};
	GLuint squareIdcs[8] = {
		0, 1,
		1, 2,
		2, 3,
		3, 0
	};

bool write = false;

void Display::draw(){
	if(write){
		Eigen::IOFormat fmt(4,0," ",",", "[", "]");
		if(model->getEquationCount() != 0){
			std::cout << (* model->getDataPointer(1) )[0].format(fmt) << std::endl;
			std::cout << (* model->getDataPointer(1) )[1].format(fmt) << std::endl << std::endl;
		}
		if(model->getPointCount() != 0){
			std::cout << (* model->getDataPointer(0) )[0].format(fmt) << std::endl;
		}
		write = false;
	}

	draw_state types[4] = {point,line,line,point};
	Vector4f colour[2] = {{255,255,0,255}, {255,0,255,1}};


	shader.activate();
	shader.setUniform(matrixID, (void*)dual.data(), 1);
	shader.setUniform(colourID, (void*)colour[1].data(), 2);
	ogl::draw(window, s_buf, 8, line, s_idx, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Eigen::IOFormat fmt(4, 0, ", ",  "", "[", "]");
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

	glGenBuffers(1, &s_buf);
	glBindBuffer(GL_ARRAY_BUFFER, s_buf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2f) * 4, &square[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &s_idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 8, &squareIdcs[0], GL_STATIC_DRAW);
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

	Eigen::IOFormat fmt(4, 0, ", ", "", "[", "]");
	std::cout << "Primal Before:\n" << primal << std::endl;
	std::cout << "Dual Before:\n" << dual << std::endl;
	recreateMatrices();

	std::cout << "Primal After:\n" << primal << std::endl;
	std::cout << "Dual After:\n" << dual << std::endl;


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
	redraw();
}

void Display::updateEquations(){
	write = true;

	GLuint eqnBuffer = buffers[1];
	GLuint eqnIndices = indices[1];
	ogl::bind(eqnBuffer, model->getDataPointer(1), 
			eqnIndices, model->getIndexPointer(1));

	GLuint dualEqnBuffer = buffers[3];
	GLuint dualEqnIndices = buffers[3];

	ogl::bind(dualEqnBuffer, model->getDataPointer(3),
			dualEqnIndices, model->getIndexPointer(3));
	
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
	model_left 		<< 	1,0,0,width,
			   			0,1,0,0,
			   			0,0,1,0,
			   			0,0,0,1;

	model_right 	<< 	width/2.f,0,0,width/4.f,
						0,1,0,0,
						0,0,0,0,
						0,0,0,1;



	// l = left
	// r = right
	// p = position
	// u = up
	// t = target
	// f = forward
	Vector3f pl, pr, ul, ur, rl, rr, tl, tr, fl, fr; 

	pl << 0,0,-1;
	pr << 0,0,-1;

	ul << 0,1,0;
	ur << 0,1,0;

	fl << 0,0,1;
	fr << 0,0,1;

	tl = (pl - fl).normalized();
	tr = (pr - fr).normalized();

	rl = ul.cross(fl).normalized();
	rr = ur.cross(fr).normalized();

	if(ul != tl.cross(rl).normalized()){
		ul = tl.cross(rl).normalized();
	}

	if(ur != tr.cross(rr).normalized()){
		ur = tr.cross(rr).normalized();
	}


	Matrix4f view_left, view_right;
	view_left 	<< 	rl(0), ul(0), -pl(0), 0,
					rl(1), ul(1), -pl(1), 0,
					rl(2), ul(2), -pl(2), 0,
					-rl.dot(pl), -ul.dot(pl), -fl.dot(pl), 1;

	view_right 	<< 	rr(0), ur(0), -pr(0), 0,
					rr(1), ur(1), -pr(1), 0,
					rr(2), ur(2), -pr(2), 0,
					-rr.dot(pr), -ur.dot(pr), -fr.dot(pr), 1;
   

	
	float left = 	(float)-width/2.f - 2;
	float right = 	(float)width/2.f + 2;

	float bottom = 	(float)-height/2.f - 2;
	float top = 	(float)height/2.f + 2;

	float near = 	-0.5f;
	float far = 	1.f;

	Matrix4f projection_left, projection_right;
	projection_left 	<< 	2.f/(right - left), 0, 0, 0,
							0, 2.f/(top - bottom), 0, 0,
							0, 0, 1.f/(near - far), 0,
							(right+left)/(right-left),
							(bottom+top)/(bottom - top),
						 	(near + far)/(near-far), 1;

	projection_right 	<< 2.f/(right - left), 0, 0, 0,
                           0, 2.f/(top - bottom), 0, 0,
                           0, 0, 1.f/(near - far), 0,
                           (right+left)/(right-left),
						   (bottom+top)/(bottom - top), 
						   (near + far)/(near-far), 1; 
							

	primal 	= projection_left 	* view_left 	* model_left;
	dual 	= projection_right 	* view_right 	* model_right;


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
