// Kody Manastyrski
// DataModel.cpp
//

#include "Equation.hpp"
#include "Point.hpp"
#include "Util.hpp"
#include <stdlib.h>
#include <vector>

#ifndef MODEL
#include "DataModel.hpp"

using std::vector;

DataModel::DataModel(){

	intervals = (float **)malloc(sizeof(float*));
	*intervals = (float *)malloc(sizeof(float) * 2);

	equationCount = 0;
	pointCount = 0;
}


void DataModel::addEquation(LinearEquation l,float start, float end){
	equations.push_back(l);
	dEquations.push_back(convert(l));

	intervals = (float **)realloc((void*)intervals,
		   	sizeof(float*) * equationCount + 1);
	intervals[equationCount] = (float*)malloc(sizeof(float) * 2);
	intervals[equationCount][0] = start;
	intervals[equationCount][1] = end;
	equationCount += 1;
}

void DataModel::addPoint(Point p){
	points.push_back(p);

	dPoints.push_back(convert(p));

	pointCount += 1;
}

void DataModel::removeEquation(LinearEquation l){
	Point dual = convert(l);
	
	vector<LinearEquation>::iterator it1 = equations.begin();
	vector<Point>::iterator it2 = dEquations.begin();


	while(it1 != equations.end() && *it1 != l){
		it1 += 1;
	}
	while(it2 != dEquations.end() && *it2 != dual){
		it1 += 1;
	}

	equations.erase(it1);
	dEquations.erase(it2);

	equationCount -= 1;
}

void DataModel::removePoint(Point p){
	LinearEquation dual = convert(p);

	vector<Point>::iterator it1 = points.begin();
	vector<LinearEquation>::iterator it2 = dPoints.begin();


	while(it1 != points.end() && *it1 != p){
		it1 += 1;
	}
	while(it2 != dPoints.end() && *it2 != dual){
		it1 += 1;
	}

	points.erase(it1);
	dPoints.erase(it2);

	pointCount -= 1;
}


vector<LinearEquation>::iterator  DataModel::getEIterator(){
	return equations.begin();
}

vector<Point>::iterator DataModel::getPIterator(){
	return points.begin();
}

vector<Point>::iterator DataModel::getDEIterator(){
	return dEquations.begin();
}

vector<LinearEquation>::iterator DataModel::getDPIterator(){
	return dPoints.begin();
}


int DataModel::getEquationCount(){
	return equationCount;
}

int DataModel::getPointCount(){
	return pointCount;
}

float ** DataModel::getIntervals(){
	return intervals;
}

#endif
