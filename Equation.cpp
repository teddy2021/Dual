// Kody Manastyrski
// Equation.cpp

#include <math.h>
#include <string>
#include <stdlib.h>

#ifndef EQN
#include "Equation.hpp"

int Equation::getDegree(){
	return degree;
}

float *Equation::getCoefficients(){
	return coefficients;
}


Equation::Equation(){
	degree = 0;
}

Equation::Equation(int d){
	degree = d > 0 ? d : 1;
	coefficients = (float *)malloc((d) * sizeof(float));
}

Equation::Equation(int d, float c[]){
	degree = d > 0 ? d : 1;
	coefficients = (float *)malloc(sizeof(float) * (d));
	for(int i = 0; i <= degree; i += 1){
		coefficients[i] = c[i];
	}
}

Equation::Equation(const Equation &src){
	degree = src.degree;
	coefficients = (float *)malloc(degree * sizeof(float));
	for(int i = 0; i < degree; i += 1){
		coefficients[i] = src.coefficients[i];
	}
}


Equation::~Equation(){
	free(coefficients);
	coefficients = NULL;
}


float Equation::getNthCoefficient(int n){
	if(n <= degree){
		return coefficients[n];
	}
	else{
		return -1;
	}
}

float Equation::calculateY(float x){
	float val = 0;
	if(degree > 1){
		for(int i = 1; i <= degree; i += 1){
			val += coefficients[i] * pow(x, i);
		}
		val += coefficients[0];
	}
	else{
		val = coefficients[0];
	}
	return val;
}

int Equation::getTermCount(){
	int count = 0;
	if(degree > 1){
		for(int i = 0; i <= degree; i += 1){
			if(coefficients[i] != 0){
				count += 1;
			}
		}
	}
	return count;
}

std::string Equation::toString(){
	std::string out;
	for(int i = 0; i < degree - 1; i += 1){
		float coef = coefficients[i];
		std::string term(std::to_string(coef));
		term.append("x^");
		int exp = degree - i;
		term.append(std::to_string(exp));
		term.append(" + ");
		out.append(term);
	}
	out.append(std::to_string(coefficients[degree - 1]));
	return out;
}


Equation& Equation::operator=(Equation other){
	degree = other.getDegree();
	free(coefficients);
	coefficients = other.getCoefficients();
	return (*this);
}

Equation& Equation::operator=(const Equation &other){
	if(this != &other){
		degree = other.degree;
		coefficients = (float *)malloc(degree * sizeof(float));
		for(int i = 0; i < degree; i += 1){
			coefficients[i] = other.coefficients[i];
		}
	}
	return *this;
}

#endif
