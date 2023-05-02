// Kody Manastyrski
// LinearEquation.cpp

#include <string>
#include <stdlib.h>
#include <stdio.h>

#ifndef LIN
#include "Equation.hpp"

float LinearEquation::calculateY(float x){
	float * coef = getCoefficients();
	float m = coef[1];
	float b = coef[0];
	return (m * x) + b;
}

std::string LinearEquation::toString(){

	float * coef = getCoefficients();
	std::string out = std::to_string(coef[0]) + "x + " + 
		std::to_string(coef[1]);
	return out;
}


bool LinearEquation::operator==(LinearEquation other){
	return getNthCoefficient(0) == other.getNthCoefficient(0) &&
		getNthCoefficient(1) == other.getNthCoefficient(1);
}

bool LinearEquation::operator!=(LinearEquation other){
	return !(*this == other);
}


LinearEquation::LinearEquation(const LinearEquation &src): Equation(2){
	coefficients[0] = src.coefficients[0];
	coefficients[1] = src.coefficients[1];
}


LinearEquation::~LinearEquation(){
	free(coefficients);
	coefficients = NULL;
}

LinearEquation& LinearEquation::operator=(const LinearEquation &other){
	if(this != &other){
		degree = 2;
		coefficients = (float *)malloc(sizeof(float) * 2);
		coefficients[0] = other.coefficients[0];
		coefficients[1] = other.coefficients[1];
	}
	return *this;
}

#endif
