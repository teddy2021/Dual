// Kody Manastyrski
// Axis.cpp
//

#include <math.h>

#ifndef AXIS
#include "Axis.hpp"

double Axis::getMinVal(){
	return min;
}


double Axis::getMaxVal(){
	return max;
}


double Axis::getRange(){
	return max - min;
}



double Axis::getMajorTickStep(){
	int val = (scale_step + 1) % 2;
	int tens = pow(10, scale_step);
	double r = ((double)val * 5.) * (double)tens;
	if(type == linear){
		return r;
	}else{
		return log10(r);
	}
}


double Axis::getMinorTickStep(){
	return getMajorTickStep() / 5.;
}


Axis::Type Axis::getType(){
	return type;
}


void Axis::setType(Axis::Type t){
	type = t;
}


#endif
