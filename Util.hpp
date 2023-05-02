// Kody Manastyrski
// Util.hpp
//
#include "Equation.hpp"
#include "Point.hpp"

#ifndef UTIL 
#define UTIL 

Point convert(LinearEquation l);
LinearEquation convert(Point p);
LinearEquation interpolate(Point p, Point q);

#endif
