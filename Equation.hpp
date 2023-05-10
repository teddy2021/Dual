// Kody Manastyrski
// Equation.h
#include <string>
#include <algorithm>
#include <stdio.h>
#include <eigen3/Eigen/Dense>

using Eigen::VectorXf;

#ifndef EQN 
#define EQN 

class Equation {

	
	protected:
		int degree;
		VectorXf coefficients;
		int getDegree();
		VectorXf  getCoefficients();
	
	public:
		Equation();
		Equation(int d);
		Equation(int d, float c[]); 

		Equation(const Equation &src);

		~Equation();

		float getNthCoefficient(int n);
		virtual float calculateY(float x);
		int getTermCount();
		virtual std::string toString();

		Equation & operator=(Equation other);	
		Equation & operator=(const Equation &other);

};

#endif

#ifndef LIN
#define LIN

class LinearEquation : public Equation {

	public:
		LinearEquation():Equation(2){};

		LinearEquation(float m, float b) : Equation(2){
			coefficients[0] = b;
			coefficients[1] = m;
		};

		LinearEquation(const LinearEquation &src);
		~LinearEquation();

		float calculateY(float x);
		virtual std::string toString();
		bool operator==(LinearEquation other);
		bool operator!=(LinearEquation other);

		LinearEquation& operator=(const LinearEquation &other);

};

#endif
