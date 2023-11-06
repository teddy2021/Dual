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
	origin << 0,0;
	Point p = display->getMousePosition();
	float x = p.getX();
	float y = p.getY();

	std::cout << "left down with controller in ";
	if(x <= display->getWidth() / 2.f){
		origin(0) = transformWidth(x, -0.5f) ; 
		origin(1) = transformHeight(-2.f * y, 1.f);;
	}
	else{
		ms = neutral;
	}
	switch(cs){
		case normal:
			std::cout << "normal mode" << std::endl;
			break;
		default:
			std::cout << "unknown mode" << std::endl;
			break;
		case selecting:
			std::cout << "selection mode getting ";
			bool pt = false, ln = false;
			Vector2f *selection;
			Vector2f margin(44.f/display->getWidth(), 44.f/display->getHeight());
			ln = model->getLine(origin(0), origin(1), 
					selection1, selection2, Vector2f(1,1));
			pt = model->getPoint(origin(0), origin(1), 
				   selection);
			if(pt || ln){
				ms = dragging;
			}
			else{
				ms = neutral;
			}

			if(pt){
				std::cout << " a point at " << (*selection) << std::endl;
				ds = point;
				selection1 = selection;
				selection2 = NULL;
			}
			else if (ln){
				ds = line;
				std::cout << " a line from " << (*selection1) << " to " << (*selection2) << std::endl;
				
			}
			else{
				ds = polygon;
				std::cout << "nothing" << std::endl;
			}
			break;
	}
	updateState();
}

void Controller::onLeftUp(){

	std::cout << "mouse up with controller in ";
	switch(cs){
		case normal:
			std::cout << "normal mode and mouse in ";
			switch(ms){
				case l_down:
					std::cout << "left down mode and draw state in ";
					if(ds == point){
						std::cout << "point mode" << std::endl;
						model->addPoint(origin);
						display->updatePoints();
					}
					else{
						std::cout << "line mode" << std::endl;
						Point p = display->getMousePosition();
						Vector2f q(
								( transformWidth(p.getX(), -0.5f)),
								( transformHeight( -2.f * p.getY(), 1.f)));
						model->addEquation(origin,q);
						display->updateEquations();
					}
					break;
				case dragging:
					std::cout << "dragging mode." << std::endl;
					selection1 = NULL;
					selection2 = NULL;
					break;
				case neutral:
					std::cout << "neutral mode" << std::endl;
				case r_down:
					std::cout << "right down mode" << std::endl;
				default:
					std::cout << "unknown mode" << std::endl;
					break;
				
			}
			break;
		case selecting:
			std::cout << "selection mode with draw state in ";
			switch(ds){
				case point:
					std::cout << "point mode" << std::endl;
					(* selection1 )(0) += dx;
					(* selection1 )(1) += dy;
					break;
				case line:
					std::cout << "line mode with a line going from " << (*selection1) << " to " << (*selection2) << std::endl;
					(* selection1 ) (0) += dx;
					(* selection2 ) (0) += dx;

					(* selection1 ) (1) += dy;
					(* selection2 ) (1) += dy;

					break;
				case polygon:
					std::cout << "polygon mode" << std::endl;
				default:
					std::cout << "unknown mode" << std::endl;
					break;
			}
			break;
		default:
			break;
	}
	dx = 0;
	dy = 0;
	prev_x = 0;
	prev_y = 0;
	ms = neutral;
	ds = point;
	cs = normal;
	updateState();
}


void Controller::onRightDown(){
	ms = r_down;
	updateState();

}


void Controller::onRightUp(){
	ms=neutral;
	updateState();
}


void Controller::updateMousePos(double x, double y){
		double a =  transformWidth(x, -.5f);
		double b =  transformHeight(-2.f*y, 1.f);
		double deltax = a - prev_x;
		double deltay = b - prev_y;
		dx += deltax;
		dy += deltay;

		prev_x = a;
		prev_y = b;

	switch(ms){
		case l_down:
			if (abs(dx) > 10.f/display->getWidth() || 
					abs( dy ) > 10.f/display->getHeight()){
				ds = line;
			}
			updateState();
			break;
		case dragging:
			switch(ds){
				case point:
					(* selection1 ) (0) += dx;
					(* selection1 ) (1) += dy;
					break;
				case polygon:
					break;
				case line:
					(* selection1 ) (0) += dx;
					(* selection1 ) (1) += dy;
					(* selection2 ) (0) += dx;
					(* selection2 ) (1) += dy;
					break;
				default:
					break;
			}
			break;
		case r_down:
			break;
		case neutral:
			break;
		default:
			break;
	}
}

void Controller::handleKeys(GLFWwindow * window, int key, 
		int scancode, int action, int mods){
	if(action == GLFW_PRESS){ 
		switch(key){
		  case GLFW_KEY_C:
			   if(cs == normal){
				   cs = selecting;
			   }
			   else{
				   cs = normal;
			   }
			   break;
		   case GLFW_KEY_P:
			   Eigen::IOFormat fmt(4,0,", ", "", "{","}");
			   vector<Vector2f>::iterator eqns = model->equationIterator();
			   vector<Vector2f>::iterator pts = model->pointIterator();

			   for(int i = 0; i < model->getEquationCount(); i += 1){
				   eqns += 2;
			   }

			   for(int i = 0; i < model->getPointCount(); i += 1){
				   pts += 1;
			   }

			   break;
		}
	}
	updateState();
}

void Controller::updateState(){
	string s;

	switch(ms){
		case l_down:
			s += "left mouse down\n";
			break;
		case r_down:
			s += "right mouse down\n";
			break;
		case neutral:
			s += "neutral mouse\n";
			break;
		case dragging:
			s += "mouse dragging\n";
			break;
	}

	switch(ds){
		case point:
			s += "point\n";
			break;
		case line:
			s += "line\n";
			break;
		case polygon:
			s += "polygon\n";
			break;
	}

	switch(cs){
		case normal:
			s += "normal\n";
			break;
		case selecting:
			s += "selecting\n";
			break;
	}

	display->setText(s);
}


float Controller::transformWidth(float initial,  float offset){
	return (initial / (float)display->getWidth()) + offset;
}

float Controller::transformHeight(float initial, float offset){
	return (initial / (float)display->getHeight()) + offset;
}

#endif
