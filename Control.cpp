// Kody Manastyrski
// Control.cpp
//

#include "Display.hpp"
#include "DataModel.hpp"
#include "Equation.hpp"
#include "Enums.hpp"
#include "Util.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

#ifndef CONTROL
#include "Control.hpp"

float Controller::transformWidth(float initial,  float offset){
	return (initial / (float)display->getWidth()) + offset;
}

float Controller::transformHeight(float initial, float offset){
	return (initial / (float)display->getHeight()) + offset;
}

void Controller::onLeftDown(){
	ms = l_down;
	ds = point;
	origin << 0,0;
	Point p = display->getMousePosition();
	std::cout << "Origin in screen space: " << p.toString() << std::endl;
	float x = p.getX();
	float y = p.getY();
	if(x <= display->getWidth() / 2.f){
		origin(0) = transformWidth(x, -0.5f) ; 
		origin(1) = transformHeight(-2.f * y, 1.f);;
		std::cout << "Origin normalized: " << origin << std::endl;
	}
	else{
		ms = neutral;
	}
}


void Controller::onLeftUp(){


	if(ds == point){
		model->addPoint(origin);
		display->updatePoints();
	}
	else{
		Point p = display->getMousePosition();
		Vector2f q(
				( transformWidth(p.getX(), -0.5f)),
				( transformHeight( -2.f * p.getY(), 1.f)));

		model->addEquation(origin,q);
		display->updateEquations();
	}
	dx = 0;
	dy = 0;
	prev_x = 0;
	prev_y = 0;
	ms = neutral;
	ds = point;
}


void Controller::onRightDown(){
	ms = r_down;

}


void Controller::onRightUp(){
	ms=neutral;
}


void Controller::updateMousePos(double x, double y){
	if (ms == l_down){
		double a =  transformWidth(x, -.5f);
		double b =  transformHeight(-2.f*y, 1.f);
		double deltax = a - prev_x;
		double deltay = b - prev_y;
		dx += deltax;
		dy += deltay;

		prev_x = a;
		prev_y = b;
		
		if (abs(dx) > 5.f/display->getWidth() || abs( dy ) > 5.f/display->getHeight()){
			ds = line;
		}
	}
}

void Controller::handleKeys(GLFWwindow * window, int key, 
		int scancode, int action, int mods){
	if(action == GLFW_PRESS){ 
		switch(key){
		   case GLFW_KEY_P:
			   Eigen::IOFormat fmt(4,0,", ", "", "{","}");
			   vector<Vector2f>::iterator eqns = model->equationIterator();
			   vector<Vector2f>::iterator pts = model->pointIterator();

			   std::cout << "Equations: " << std::endl;
			   for(int i = 0; i < model->getEquationCount(); i += 1){
				   std::cout << eqns->format(fmt) << "->" << ( eqns+1 )->format(fmt) << std::endl;
				   eqns += 2;
			   }

			   std::cout << "Points: " << std::endl;
			   for(int i = 0; i < model->getPointCount(); i += 1){
				   std::cout << pts->format(fmt) << std::endl;
				   pts += 1;
			   }

			   std::cout << std::endl;
			   break;
		}
	}
}

#endif
