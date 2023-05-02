// Kody Manastyrski
// Point.cpp
//

#include <string>

#ifndef PT
#include "Point.hpp"

Point::Point(){
	x = 0;
	y = 0;
}

Point::Point(float a, float b){
	x = a;
	y = b;
}

Point::Point(const Point &other){
	x = other.x;
	y = other.y;
}

Point::~Point(){
	x = 0;
	y = 0;
}

void Point::setX(float newX){
	x = newX;
}

void Point::setY(float newY){
	y = newY;
}

float Point::getY(){
	return y;
}

float Point::getX(){
	return x;
}

std::string Point::toString(){
	return "(" + std::to_string(getX()) + ", " + std::to_string(getY()) + ")";
}


bool Point::operator==(Point other){
	return x == other.getX() && y == other.getY();
}

bool Point::operator!=(Point other){
	return !(*this == other);
}

Point& Point::operator=(const Point &other){
	if(this != &other){
		x = other.x;
		y = other.y;
	}
	return *this;
}

#endif
