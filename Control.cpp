// Kody Manastyrski
// Control.cpp
//

#include "Display.hpp"
#include "DataModel.hpp"
#include "Point.hpp"
#include "Equation.hpp"
#include "Util.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

#ifndef CONTROL
#include "Control.hpp"

void Controller::onLeftDown(){
	dx = 0;
	dy = 0;	
}


void Controller::onLeftUp(){

	if(dx < 3 || dy < 3){
		double x = (prev_x - dx) / (float)display->getWidth();
		double y = (prev_y - dy) / (float)display->getHeight();
		model->addPoint(Point(x, y));
		display->updatePoints();
	}
	else{

		double x1 = prev_x / (float)display->getWidth();
		double y1 = prev_y / (float)display->getHeight();
		
		double x2 = prev_x - dx / (float)display->getWidth();
		double y2 = prev_y - dy / (float)display->getHeight();
	
		Point p(x1, y1);
		Point q(x2, y2);

		LinearEquation l = interpolate(p, q);
		model->addEquation(l,prev_x - dx, prev_x);
		display->updateEquations();
	}
}


void Controller::onRightDown(){

}


void Controller::onRightUp(){

}


void Controller::updateMousePos(double x, double y){
	dx += x - prev_x;
	dy += y - prev_y;

	prev_x = x;
	prev_y = y;

}

void Controller::handleKeys(GLFWwindow * window, int key, 
		int scancode, int action, int mods){
	switch(key){
		case GLFW_KEY_P:
			vector<LinearEquation>::iterator eqns = model->getEIterator();
			vector<Point>::iterator pts = model->getPIterator();

			for(int i = 0; i < model->getEquationCount(); i += 1){
				std::cout << eqns->toString() << std::endl;
				eqns += 1;
			}

			for(int i = 0; i < model->getPointCount(); i += 1){
				std::cout << pts->toString() << std::endl;
				pts += 1;
			}

			std::cout << std::endl;
			break;
	}

}

#endif
