// Kody Manastyrski
// Display.cpp
//

#include "ogl.hpp"
#include "Shader.hpp"
#include <vector>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using std::vector;
using Eigen::VectorXd;
using Eigen::Vector2d;
using Eigen::Vector3f;
using Eigen::Matrix4f;

#ifndef DISPLAY
#include "Display.hpp"

void Display::draw(){
	GLuint counts[4];
	counts[0] = model->getEquationCount();
	counts[2] = counts[0];
	counts[1] = model->getPointCount();
	counts[3] = counts[1];

	short types[4] = {1,0,1,0};

	Matrix4f p = projection * view * primal;
	Matrix4f d = projection * view * dual;


	glClear(GL_COLOR_BUFFER_BIT);
	for(int i = 0; i < 8; i += 2){
		shader.activate();	
		if(i < 4){
			glUniformMatrix4fv(matrixID, 1, GL_FALSE, (const float *)p.data());
		}
		else{
			glUniformMatrix4fv(matrixID, 1, GL_FALSE, (const float *)d.data());
		}

		ogl::draw(
				window, // screen
				buffers[i], // buffer
			   	counts[i/2], // count
				types[i/2], // type
				buffers[i + 1], // indices
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

	buffers = (GLuint *)malloc(sizeof(GLuint) * 8);
	backgroundColour = VectorXd(4);
	backgroundColour << 1, 1, 1, 1;
	shader.activate();
	for(int i = 0; i < 8; i += 1){
		glGenBuffers(1, &buffers[i]);
	}
	matrixID = shader.getUniform("mvp");

	Matrix4f scale, translate;
	scale << 2.f/(float)(width),	0, 				0, 0,
		   		0, 					1.f/(float)height, 	0, 0,
				0, 					0, 				1, 0,
				0, 					0,				0, 1;
	translate = Matrix4f::Identity();
	translate(0,3) = (float)width/2.f;

	primal = scale * Matrix4f::Identity(); 

	dual << translate * scale;

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

}


void Display::updatePoints(){

	vector<Point>::iterator  pnts = model->getPIterator();
	vector<LinearEquation>::iterator dPnts = model->getDPIterator();

	int pntCount = model->getPointCount();

	vector<Vector2d> eqnPts;
	for(int i = 0; i < pntCount; i += 1){
		eqnPts.push_back(Vector2d(0,dPnts->calculateY(0)));
		eqnPts.push_back(Vector2d(1, 
				dPnts->calculateY(1)));
		dPnts += 1;
	}

	vector<Eigen::Vector2d> points;
	for(int i = 0; i < pntCount; i += 1){
		Vector2d p(pnts->getX(), pnts->getY());
		points.push_back(p);
		pnts += 1;
	}

	GLuint pointBuffer = buffers[0];
	GLuint pointIndices = buffers[1];
	GLuint dualPointBuffer = buffers[4];
	GLuint dualIndices = buffers[5];

	genVBO(pointBuffer, pointIndices,  (void *)&points[0], index(points.size()), pntCount, 2);
	genVBO(dualPointBuffer, dualIndices, (void *)&eqnPts[0], index(eqnPts.size()), pntCount, 2);

	redraw();

}


void Display::updateEquations(){

	vector<LinearEquation>::iterator eqns = model->getEIterator();
	vector<Point>::iterator dEqns = model->getDEIterator();

	int eqnCount = model->getEquationCount();
	int pntCount = model->getPointCount();

	float ** intervals = model->getIntervals();

	vector<Vector2d> eqnPts;
	for(int i = 0; i < eqnCount; i += 1){
		eqnPts.push_back(Vector2d(intervals[i][0], 
				eqns->calculateY(intervals[i][0])));
		eqnPts.push_back(Vector2d(intervals[i][1], 
				eqns->calculateY(intervals[i][1])));
		eqns += 1;
	}

	vector<Eigen::Vector2d> points;
	for(int i = 0; i < pntCount; i += 1){
		Vector2d p(dEqns->getX(), dEqns->getY());
		points.push_back(p);
		dEqns += 1;
	}

	GLuint pointBuffer = buffers[2];
	GLuint pointIndices = buffers[3];
	GLuint dualPointBuffer = buffers[6];
	GLuint dualIndices = buffers[7];

	genVBO(pointBuffer, pointIndices,  (void *)&points[0], index(points.size()), pntCount, 2);
	genVBO(dualPointBuffer, dualIndices, (void *)&eqnPts[0], index(eqnPts.size()), eqnCount, 2);

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
#endif
