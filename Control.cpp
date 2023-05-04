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


void Controller::onLeftDown(){
	ms = l_down;
	origin = display->getMousePosition();
	origin.setX(origin.getX() / ( ( (float)display->getWidth())/2.f ) ); 
	origin.setY(origin.getY() / (float)display->getHeight() ) ;
}


void Controller::onLeftUp(){


	std::cout << "(dx, dy) = (" << dx << ", " << dy << ")" << std::endl;
	std::cout << "(ox, oy) = (" << origin.getX() << ", " 
		<< origin.getY() << ")" << std::endl;
	if(dx < 20 || dy < 20){
		std::cout << "\tPoint (x, y) = " << origin.toString() << std::endl;
		model->addPoint(origin);
		display->updatePoints();
	}
	else{

		float x = origin.getX() / (float)display->getWidth();
		float y = origin.getY() / (float)display->getHeight();
	
		Point p(x, y);
		float x2 = x + (dx/( (float)display->getWidth())/2.f);
		float y2 = y + (dy/(float)display->getHeight());
		x2 = x2 > 1 ? 1 : x2;
		Point q(x2, y2);

		LinearEquation l = interpolate(p, q);
		std::cout << "\tEqn = " << l.toString() << std::endl;
		model->addEquation(l,prev_x - dx, prev_x);
		display->updateEquations();
	}
	dx = 0;
	dy = 0;
	prev_x = 0;
	prev_y = 0;
	ms = neutral;
}


void Controller::onRightDown(){
	ms = r_down;

}


void Controller::onRightUp(){
	ms=neutral;
}


void Controller::updateMousePos(double x, double y){
	if (ms == l_down){
		dx += x - prev_x;
		dy += y - prev_y;

		prev_x = x;
		prev_y = y;
	}
}

void Controller::handleKeys(GLFWwindow * window, int key, 
		int scancode, int action, int mods){
	switch(key){
		case GLFW_KEY_P:
			vector<LinearEquation>::iterator eqns = model->getEIterator();
			vector<Point>::iterator pts = model->getPIterator();

			std::cout << "Equations: " << std::endl;
			for(int i = 0; i < model->getEquationCount(); i += 1){
				std::cout << eqns->toString() << std::endl;
				eqns += 1;
			}

			std::cout << "Points: " << std::endl;
			for(int i = 0; i < model->getPointCount(); i += 1){
				std::cout << pts->toString() << std::endl;
				pts += 1;
			}

			std::cout << std::endl;
			break;
	}

}

#endif
