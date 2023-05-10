// Kody Manastyrski
// LinearEquation.cpp

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <eigen3/Eigen/Dense>

using Eigen::VectorXf;

#ifndef LIN
#include "Equation.hpp"

float LinearEquation::calculateY(float x){
	return Equation::calculateY(x);
}


std::string LinearEquation::toString(){
	return Equation::toString();
}


bool LinearEquation::operator==(LinearEquation other){
	return getNthCoefficient(0) == other.getNthCoefficient(0) &&
		getNthCoefficient(1) == other.getNthCoefficient(1);
}

bool LinearEquation::operator!=(LinearEquation other){
	return !(*this == other);
}


LinearEquation::LinearEquation(const LinearEquation &src): Equation(2){
	coefficients << src.coefficients(0), src.coefficients(1);
}


LinearEquation::~LinearEquation(){
}

LinearEquation& LinearEquation::operator=(const LinearEquation &other){
	if(this != &other){
		degree = 2;
		coefficients << other.coefficients( 0 ),  other.coefficients( 1 );
	}
	return *this;
}

#endif
