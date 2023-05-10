// Kody Manastyrski
// Equation.cpp

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <stdlib.h>
#include <string>
#include <iostream>


using Eigen::VectorXf;
#ifndef EQN
#include "Equation.hpp"

int Equation::getDegree() { return degree; }

VectorXf Equation::getCoefficients() { return coefficients; }

Equation::Equation(){
	degree = 0;
}

Equation::Equation(int d): coefficients(d) { 
	degree = d > 0 ? d : 1; 
}

Equation::Equation(int d, float c[]): coefficients(d) {
	degree = d > 0 ? d : 1;
	for (int i = 0; i < degree; i += 1) {
		coefficients(i) = c[i];
	}
}

Equation::Equation(const Equation &src) {
	degree = src.degree;
	for (int i = 0; i < degree; i += 1) {
		coefficients(i) = src.coefficients(i);
	}
}

Equation::~Equation() {}

float Equation::getNthCoefficient(int n) {
	if (n <= degree) {
		return coefficients[n];
	} else {
		return -1;
	}
}

float Equation::calculateY(float x) {
	float res = 0;
	for(int i = degree - 1; i >= 0; i -= 1){
			res += coefficients(i) * pow(x, i);
	}
	return res;
}

int Equation::getTermCount() {
	int count = 0;
	if (degree > 1) {
		for (int i = 0; i <= degree; i += 1) {
			if (coefficients( i ) != 0) {
				count += 1;
			}
		}
	}
	return count;
}

std::string Equation::toString() {
	std::string out;
	for (int i = degree -1 ; i > 0; i -= 1) {
		float coef = coefficients(i);
		std::string term(std::to_string(coef));
		term.append("x^");
		int exp = i;
		term.append(std::to_string(exp));
		term.append(" + ");
		out.append(term);
	}
	out.append(std::to_string(coefficients(0)));
	return out;
}

Equation &Equation::operator=(Equation other) {
	degree = other.getDegree();
	coefficients = other.getCoefficients();
	return (*this);
}

Equation &Equation::operator=(const Equation &other) {
	if (this != &other) {
		degree = other.degree;
		for (int i = 0; i < degree; i += 1) {
			coefficients(i) = other.coefficients(i);
		}
	}
	return *this;
}

#endif
