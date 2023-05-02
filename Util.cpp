// Kody Manastyrski
// Util.hpp
//
#include "Point.hpp"
#include "Equation.hpp"

#ifndef UTIL
#include "Util.hpp"

LinearEquation convert(Point p){
	return LinearEquation (p.getX(), -(p.getY()));
}

Point convert(LinearEquation l){
	return Point(l.getNthCoefficient(1), - l.getNthCoefficient(0));
}


LinearEquation interpolate(Point p, Point q){
	float m = (q.getY() - p.getY())/(q.getX() - p.getX());
	float b1 = p.getY() - (m * p.getX());
	float b2 = q.getY() - (m * q.getX());
	float b = b1;
	if(b1 != b2){
		b = b1 == 0 ? b1 : b2;
	}
	return LinearEquation(m, b);

}

#endif
