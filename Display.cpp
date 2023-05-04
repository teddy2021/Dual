// Kody Manastyrski
// Display.cpp
//

#include "ogl.hpp"
#include "Shader.hpp"
#include "Equation.hpp"
#include <vector>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <numeric>

using std::vector;
using Eigen::VectorXd;
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix4f;

#ifndef DISPLAY
#include "Display.hpp"

void Display::addVector(int set_index, Eigen::Vector2f v) {
    objectVertices[set_index].vertices.push_back(v);
    objectVertices[set_index].indices.
		push_back(
			objectVertices[set_index].
			vertices.size() - 1);
}

void Display::draw(){

	short types[4] = {1,0,1,0};
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
	height = 607;
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
	glPointSize(4);
}


void Display::updatePoints(){

	Eigen::IOFormat fmt(4, 0, ", ", "", "[", "]");
	
	GLuint pointBuffer = buffers[0];
	GLuint pointIndices = indices[0];


	glBindBuffer(GL_ARRAY_BUFFER, pointBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointIndices);
	
	vector<Point>::iterator  pnts = model->getPIterator();

	while(!model->iteratorAtEnd(pnts)){
		Vector2f p(pnts->getX(), pnts->getY());
		if(std::find(
				objectVertices[0].vertices.begin(), 
				objectVertices[0].vertices.end(), p)
			 == objectVertices[0].vertices.end()){
			addVector(0, p);
		}
		pnts += 1;
	}

	glBufferData(GL_ARRAY_BUFFER, 
				sizeof(
					objectVertices[0].vertices) * 
					objectVertices[0].vertices.size(), 
				objectVertices[0].vertices.data(), 
				GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(objectVertices[0].indices) * 
							objectVertices[0].indices.size(), 
					objectVertices[0].indices.data(), 
					GL_STATIC_DRAW);
	


	GLuint dualPointBuffer = buffers[2];
	GLuint dualIndices = indices[2];
	glBindBuffer(GL_ARRAY_BUFFER, dualPointBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dualIndices);

	vector<LinearEquation>::iterator dPnts = model->getDPIterator();
	while(!model->iteratorAtEnd(dPnts)){
		Vector2f p(0,dPnts->calculateY(0));
		Vector2f q(1,dPnts->calculateY(1));
		if(std::find(
				objectVertices[2].vertices.begin(),
				objectVertices[2].vertices.end(), 
				p) == objectVertices[0].vertices.end()
			||
			std::find(
				objectVertices[2].vertices.begin(), 
				objectVertices[2].vertices.end(), q) == 
			objectVertices[0].vertices.end()){

			addVector(2, p);
			addVector(2, q);
		}
		dPnts += 1;
	}
	glBufferData(GL_ARRAY_BUFFER, 
					sizeof(objectVertices[2].vertices) * 
						objectVertices[2].vertices.size(), 
					objectVertices[2].vertices.data(),
					GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					sizeof(objectVertices[2].indices) * 
						objectVertices[2].indices.size(), 
					objectVertices[2].indices.data(), 
					GL_STATIC_DRAW);
	
	counts[0] = objectVertices[0].indices.size();
	counts[2] = objectVertices[2].indices.size();

	redraw();

}


void Display::updateEquations(){

	// Get buffers for updating. Equations and dual points are oddly numbered
	GLuint eqnBuffer = buffers[1];
	GLuint eqnIndices = indices[1];
	
	glBindBuffer(GL_ARRAY_BUFFER, eqnBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eqnIndices);

	// get necessary variables for iterating through equation list
	vector<LinearEquation>::iterator eqns = model->getEIterator();
	float ** intervals = model->getIntervals();

	// storage for vertices and indices
	int i = 0;
	Eigen::IOFormat fmt(4, 0, ", ", "", "[", "]");
	while(!model->iteratorAtEnd(eqns)){
		Vector2f p(intervals[i][0], 
					eqns->calculateY(intervals[i][0]));
		Vector2f q(intervals[i][1], 
					eqns->calculateY(intervals[i][1]));
		if( std::find(objectVertices[1].vertices.begin(), objectVertices[1].vertices.end(), q) == objectVertices[1].vertices.end()
		 	||
		 	std::find(objectVertices[1].vertices.begin(), objectVertices[1].vertices.end(), p) == objectVertices[1].vertices.end()){

			addVector(1, p);
			addVector(1, q);
		}
		eqns += 1;
		i += 1;
	}

	glBufferData(GL_ARRAY_BUFFER, objectVertices[1].vertices.size() * sizeof(objectVertices[1].vertices), objectVertices[1].vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objectVertices[1].indices.size() * sizeof(objectVertices[1].indices), objectVertices[1].indices.data(), GL_STATIC_DRAW);



	GLuint dualPointBuffer = buffers[3];
	GLuint dualIndices = indices[3];
	glBindBuffer(GL_ARRAY_BUFFER,dualPointBuffer);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dualIndices);

	// Storage for dual points
	vector<Point>::iterator dEqns = model->getDEIterator();
	while(!model->iteratorAtEnd(dEqns)){
		Vector2f p(dEqns->getX(), dEqns->getY());
		if( std::find(objectVertices[3].vertices.begin(), objectVertices[3].vertices.end(), p) == objectVertices[3].vertices.end()){
			
			addVector(3, p);
		}
		dEqns += 1;
	}

	counts[ 1 ] = objectVertices[1].indices.size();
	counts[3] = objectVertices[3].indices.size();

	glBufferData(GL_ARRAY_BUFFER, objectVertices[3].vertices.size() * sizeof(objectVertices[3].vertices), objectVertices[3].vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, objectVertices[3].indices.size() * sizeof(objectVertices[3].indices), objectVertices[3].indices.data(), GL_STATIC_DRAW);
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
	float left = 0.0f;
	float right = width / 2.0f;
	float bottom = 0.0f;
	float top = height;

	// Set up the orthographic projection matrix
	Eigen::Matrix4f projection_left = Eigen::Matrix4f::Identity();
	projection_left(0, 0) = 2.0f / (right - left);
	projection_left(1, 1) = 2.0f / (top - bottom);
	projection_left(2, 2) = -1.0f;
	projection_left(3, 0) = -(right + left) / (right - left);
	projection_left(3, 1) = -(top + bottom) / (top - bottom);

	// Set up the view matrix
	Eigen::Vector3f camera_pos_left(0.0f, 0.0f, 0.0f);
	Eigen::Vector3f camera_target_left(0.0f, 0.0f, -1.0f);
	Eigen::Vector3f camera_up_left(0.0f, 1.0f, 0.0f);
	Eigen::Matrix4f view_left = Eigen::Matrix4f::Identity();
	view_left.block<3, 1>(0, 0) = (camera_up_left.cross(camera_target_left)).normalized();
	view_left.block<3, 1>(0, 1) = (camera_up_left).normalized();
	view_left.block<3, 1>(0, 2) = -(camera_target_left).normalized();
	view_left(0, 3) = -camera_pos_left(0);
	view_left(1, 3) = -camera_pos_left(1);
	view_left(2, 3) = -camera_pos_left(2);

	// Set up the model matrix (assuming your user items are centered at (0, 0))
	Eigen::Matrix4f model_left = Eigen::Matrix4f::Identity();
	model_left(0, 3) = -width / 4.0f;
	model_left(1, 3) = height / 2.0f;

	// Combine the matrices
	primal = projection_left * view_left * model_left;
	left = width / 2.0f;
	right = width;
	bottom = 0.0f;
	top = height;

	// Set up the orthographic projection matrix
	Matrix4f projection_right = Matrix4f::Identity();
	projection_right(0, 0) = 2.0f / (right - left);
	projection_right(1, 1) = 2.0f / (top - bottom);
	projection_right(2, 2) = -2.0f;
	projection_right(3, 0) = -(right + left) / (right - left);
	projection_right(3, 1) = -(top + bottom) / (top - bottom);
	projection_right(3, 2) = -1.0f;

	// Set up the view matrix
	Vector3f camera_pos_right = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f camera_target_right = Vector3f(0.0f, 0.0f, -1.0f);
	Vector3f camera_up_right = Vector3f(0.0f, 1.0f, 0.0f);
	Eigen::Matrix4f view_right = Eigen::Matrix4f::Identity();
	view_right.block<3, 1>(0, 0) = (camera_up_right.cross(camera_target_right)).normalized();
	view_right.block<3, 1>(0, 1) = (camera_up_right).normalized();
	view_right.block<3, 1>(0, 2) = -(camera_target_right).normalized();
	view_right(0, 3) = -camera_pos_right(0);
	view_right(1, 3) = -camera_pos_right(1);
	view_right(2, 3) = -camera_pos_right(2);

	// Set up the model matrix (assuming your result items are centered at (0, 0))
	Matrix4f model_right = Matrix4f::Identity();
	model_right(0, 3) = width / 4.0f;
	model_right(1, 3) = height / 2.0f;

	// Combine the matrices
	dual = projection_right * view_right * model_right;
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
