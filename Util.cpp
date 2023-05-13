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
	float b = p.getY() - (m * p.getX());
	return LinearEquation(m, b);

}

#endif
