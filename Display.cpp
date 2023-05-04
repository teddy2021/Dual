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
using Eigen::Vector2d;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix4f;

#ifndef DISPLAY
#include "Display.hpp"

void Display::draw(){

	short types[4] = {1,0,1,0};
	Vector4f colour[2] = {{255,255,0,255}, {255,0,255,1}};

	Matrix4f p = view;
	Matrix4f d = view;


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for(int i = 0; i < 4; i += 1){
		shader.activate();	
		if(i < 2){
			shader.setUniform(matrixID, (void *)p.data(), 1);
			shader.setUniform(colourID, (void *)colour[0].data(), 2);
		}
		else{
			shader.setUniform(matrixID, (void *)d.data(), 1);
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

	Matrix4f scale, translate;
	scale << (float)(width),	0, 				0, 0,
		   		0, 					(float)height, 	0, 0,
				0, 					0, 				1, 0,
				0, 					0,				0, 1;
	translate = Matrix4f::Identity();
	translate(0,3) = (float)width/2.f;

	primal = Matrix4f::Identity(); 

	dual << Matrix4f::Identity();

	Eigen::Matrix3f v;
	Vector3f position =  { 1.5f, 0, 0};
	Vector3f target = { -1.f,  0, 0 };
	Vector3f up 	= { 0, 1.f, 0 };

	Vector3f zaxis = target - position;
	zaxis = zaxis.normalized();

	Vector3f xaxis = zaxis.cross(up).normalized();
	Vector3f yaxis = xaxis.cross(zaxis).normalized();
	view = Matrix4f::Identity();
	for(int i = 0; i < 3; i += 1){
		view(0, i) = xaxis(i);
		view(1, i) = yaxis(i);
		view(2, i) = zaxis(i);

	}

	view(0,3) = -xaxis.dot(position);
	view(1,3) = -yaxis.dot(position);
	view(2,3) = -zaxis.dot(position);


	//
	//v.col(2) = (position - target).normalized();
	//v.col(0) = up.cross(v.col(2)).normalized();
	//v.col(1) = v.col(2).cross(v.col(0));

	//view.topLeftCorner(3,3) = v.transpose();
	//view.topRightCorner(3,1) = -v.transpose() * position;
	//view(3,3) = 1.f;
	
	projection = Matrix4f::Zero();

	float theta = 5.0f;
	float range = 3 - (-1);
	float invtan = 1./tan(theta);

	projection(0,0) = invtan / (float)((float)width/(float)height);
	projection(1,1) = invtan;
	projection(2,2) = -(-1 + 3)/range;
	projection(3,2) = -1.f;
	projection(2,3) = -2.f * (-1 + 3)/range;
	projection(3,3) = 0;

	glPointSize(4);
}


void Display::updatePoints(){

	Eigen::IOFormat fmt(4, 0, ", ", "", "[", "]");
	
	GLuint pointBuffer = buffers[0];
	GLuint pointIndices = indices[0];
	GLuint dualPointBuffer = buffers[2];
	GLuint dualIndices = indices[2];

	vector<Point>::iterator  pnts = model->getPIterator();

	glBindBuffer(GL_ARRAY_BUFFER, pointBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pointIndices);
	vector<Eigen::Vector2d> points;
	while(!model->iteratorAtEnd(pnts)){
		Vector2d p(pnts->getX(), pnts->getY());
		std::cout << "Points updated with " << 
			p.format(fmt) << std::endl;
		points.push_back(p);
		pnts += 1;
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) * points.size(), points.data(), GL_STATIC_DRAW);

	vector<GLuint> p_indicies(points.size());
	std::iota(p_indicies.begin(), p_indicies.end(), 0);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(p_indicies) * p_indicies.size(), p_indicies.data(), GL_STATIC_DRAW);
	


	glBindBuffer(GL_ARRAY_BUFFER, dualPointBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dualIndices);

	vector<LinearEquation>::iterator dPnts = model->getDPIterator();
	vector<Vector2d> eqnPts;
	while(!model->iteratorAtEnd(dPnts)){
		eqnPts.push_back(Vector2d(0,dPnts->calculateY(0)));
		eqnPts.push_back(Vector2d(1, 
				dPnts->calculateY(1)));

		std::cout << "Dual equations  updated with " << 
			eqnPts[eqnPts.size() -1 ].format(fmt) << " and " <<
		   eqnPts[eqnPts.size() - 2].format(fmt) << std::endl;
		dPnts += 1;
	}
	vector<GLuint> d_indicies(eqnPts.size());
	std::iota(d_indicies.begin(), d_indicies.end(), 0);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(d_indicies) * d_indicies.size(), d_indicies.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eqnPts) * eqnPts.size(), eqnPts.data(), GL_STATIC_DRAW);
	
	counts[0] = p_indicies.size();
	counts[2] = d_indicies.size();




	redraw();

}


void Display::updateEquations(){

	// Get buffers for updating. Equations and dual points are oddly numbered
	GLuint eqnBuffer = buffers[1];
	GLuint eqnIndices = indices[1];
	GLuint dualPointBuffer = buffers[3];
	GLuint dualIndices = indices[3];
	
	glBindBuffer(GL_ARRAY_BUFFER, eqnBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eqnIndices);

	// get necessary variables for iterating through equation list
	vector<LinearEquation>::iterator eqns = model->getEIterator();
	int eqnCount = model->getEquationCount();
	float ** intervals = model->getIntervals();

	// storage for vertices and indices
	vector<Vector2d> eqnPts;
	vector<GLuint> eqnIdx(eqnCount);
	int i = 0;
	Eigen::IOFormat fmt(4, 0, ", ", "", "[", "]");
	while(!model->iteratorAtEnd(eqns)){
		eqnPts.push_back(Vector2d(intervals[i][0], 
				eqns->calculateY(intervals[i][0])));
		eqnIdx.push_back(eqnPts.size() - 1);

		eqnPts.push_back(Vector2d(intervals[i][1], 
				eqns->calculateY(intervals[i][1])));

		std::cout << "Equations updated with " << 
			eqnPts[ eqnPts.size() - 1 ].format(fmt) << 
			", and " << eqnPts[ eqnPts.size() - 2 ].format(fmt) << std::endl;
		eqnIdx.push_back(eqnPts.size() - 1);
		eqns += 1;
		i += 1;
	}
	glBufferData(GL_ARRAY_BUFFER, eqnPts.size() * sizeof(eqnPts), eqnPts.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, eqnIdx.size() * sizeof(eqnIdx), eqnIdx.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,dualPointBuffer);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dualIndices);

	// Storage for dual points
	vector<Eigen::Vector2d> points;
	vector<Point>::iterator dEqns = model->getDEIterator();
	vector<GLuint> ptsIdx;
	while(!model->iteratorAtEnd(dEqns)){
		std::cout << "Dual Points updated with " << 
			points[ points.size() - 1 ].format(fmt) << 
			", and " << points[ points.size() - 2 ].format(fmt) << std::endl;
		Vector2d p(dEqns->getX(), dEqns->getY());
		points.push_back(p);
		ptsIdx.push_back(points.size() - 1);
		dEqns += 1;
	}

	counts[ 1 ] = eqnIdx.size();
	counts[3] = ptsIdx.size();

	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(points), points.data(), GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, ptsIdx.size() * sizeof(ptsIdx), ptsIdx.data(), GL_STATIC_DRAW);
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
	primal << 	(float)(width)/2.f, 0, 1,
		   		0, height, 1,
				0, 0, 1;

	dual << 	(float)(width)/2.f, 0, (float)(width)/2.f,
		 		0, height, 1,
				0 ,0, 1;
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
