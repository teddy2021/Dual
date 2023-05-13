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
	ds = point;
	Point p = display->getMousePosition();
	std::cout << "Origin in screen space: " << p.toString() << std::endl;
	origin(0) = ( 2.f * p.getX() / (float)display->getWidth() ) - 1.f ; 
	origin(1) = ( -2.f * p.getY() / (float)display->getHeight() ) + 1.f;
	std::cout << "Origin normalized: " << origin << std::endl;
}


void Controller::onLeftUp(){


	if(ds == point){
		model->addPoint(origin);
		display->updatePoints();
	}
	else{
		float x2 = origin(0) + dx;
		float y2 = origin(1) + dy;
		
		Vector2f q(x2, y2);

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
		double a = ( x/(float)display->getWidth() );
		double b = ( y/(float)display->getHeight() );
		dx += a - prev_x;
		dy += b - prev_y;

		prev_x = a;
		prev_y = b;
		
		if (dx > .2 || dy > .2){
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
