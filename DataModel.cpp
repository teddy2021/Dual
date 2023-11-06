// Kody Manastyrski
// DataModel.cpp
//

#include "Equation.hpp"
#include "Point.hpp"
#include "Util.hpp"

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include <eigen3/Eigen/Dense>


#ifndef MODEL
#include "DataModel.hpp"

using Eigen::Vector2f;
using std::vector;
using std::min;
using std::max;

DataModel::DataModel(){

	equationCount = 0;
	pointCount = 0;
}


void DataModel::addEquation(Vector2f start, Vector2f end){
	equations.push_back(start);
	eqnIndices.push_back(equations.size() - 1);
	equations.push_back(end);
	eqnIndices.push_back(equations.size() - 1);
	equationCount += 1;

	Vector2f dir = end-start;
	Vector2f dual;
	dual << -dir(0), -dir(1);

	dEquations.push_back(dual);
	dualEqnIndices.push_back(dEquations.size() - 1);
}

void DataModel::addPoint(Vector2f p){
	points.push_back(p);
	pointIndices.push_back(points.size() - 1);

	float m = -p(0);
	float b = p(1);

	float y1 = m*0 + b;
	float y2 = m*1 + b;

	Vector2f start(0.f, y1);
	Vector2f end(1.f, y2);

	dPoints.push_back(start);
	dualPntIndices.push_back(dPoints.size() - 1);
	dPoints.push_back(end);
	dualPntIndices.push_back(dPoints.size() - 1);

	pointCount += 1;
}

void DataModel::removeEquation(Vector2f start, Vector2f end){
	vector<Vector2f>::iterator eqn = equationIterator();
	int idx = 0;
	bool found = false;
	while(!equationIteratorAtEnd(eqn + 1)){
		if(*eqn == start && *(eqn + 1) == end){
			found = true;
			break;
		}
		idx += 1;
		eqn += 2;
	}

	if(found){
		equations.erase(eqn, eqn + 1);
	

		vector<Vector2f>::iterator dEqn = dualEquationIterator();
		int i = 0;
		while(i != idx && !pointIteratorAtEnd(dEqn)){
			dEqn += 1;
			i += 1;
		}

		dEquations.erase(dEqn);

		equationCount -= 1;
	}
}

void DataModel::removePoint(Vector2f p){
	vector<Vector2f>::iterator pnt = pointIterator();
	int idx = 0;
	bool found = false;
	while(!pointIteratorAtEnd(pnt)){
		if(*pnt == p){
			found = true;
			break;
		}
		idx += 1;
		pnt += 1;
	}

	if(found){
		equations.erase(pnt);
	
		vector<Vector2f>::iterator dPnt = dualPointIterator();
		int i = 0;
		while(i != idx && !pointIteratorAtEnd(dPnt)){
			dPnt += 1;
			i += 1;
		}

		dEquations.erase(dPnt, dPnt + 1);

		pointCount -= 1;
	}
}


vector<Vector2f> * DataModel::getDataPointer(int set){
	if(0 == set){
		return &points;
	}
	else if(1 == set){
		return &equations;
	}
	else if(2 == set){
		return &dPoints;
	}
	return &dEquations;
}

vector<GLuint> * DataModel::getIndexPointer(int set){
	if(set == 0){
		return &pointIndices;
	}
	else if(set == 1){
		return &eqnIndices;
	}
	else if(set == 2){
		return &dualPntIndices;
	}
	return &dualEqnIndices;
}


vector<Vector2f>::iterator  DataModel::equationIterator(){
	return equations.begin();
}

vector<Vector2f>::iterator DataModel::pointIterator(){
	return points.begin();
}

vector<Vector2f>::iterator DataModel::dualEquationIterator(){
	return dEquations.begin();
}

vector<Vector2f>::iterator DataModel::dualPointIterator(){
	return dPoints.begin();
}

bool DataModel::pointIteratorAtEnd(vector<Vector2f>::iterator it){
	return it == points.end() || it == dEquations.end();
}

bool DataModel::equationIteratorAtEnd(vector<Vector2f>::iterator it){
	return it == equations.end() || it == dPoints.end();
}

int DataModel::getEquationCount(){
	return equationCount;
}

int DataModel::getPointCount(){
	return pointCount;
}


bool DataModel::getPoint(float x, float y, Vector2f *& storage){

	float left_bound = x - 0.01;
	float right_bound = x + 0.01;
	float upper_bound = y + 0.01;
	float lower_bound = y - 0.01;


	vector<Vector2f>::iterator pts = pointIterator();

	while (!pointIteratorAtEnd(pts)){
		float posX = (*pts)(0);
		float posY = (*pts)(1);

		if( left_bound <= posX && posX <= right_bound &&
				lower_bound <= posY && posY <= upper_bound ){
			storage = &( *pts );
			return true;
		} 
		pts += 1;
	}
	return false;

}


float heronMethod(float l1, float l2, float l3){
	float semiperim = (l1 + l2 + l3)/2.f;
	float area = sqrt(semiperim * (semiperim - l1) * (semiperim - l2) * (semiperim - l3));
	return area;
}

bool DataModel::getLine(float x, float y, 
		Vector2f *& start_storage, Vector2f *& end_storage, 
		Vector2f margin){

	for(int i = 0; i < equations.size() - 1; i += 2){
		if(x < min(equations[i](0), equations[i+1](0)) || 
				x > max(equations[i](0), equations[i+1](0)) || 
				y < min(equations[i](1), equations[i+1](1)) || 
				y > max(equations[i](1), equations[i+1](1))){
			continue;
		}

		Vector2f start = equations[i];
		Vector2f end = equations[i+1];

		Vector2f direction = end - start;

		Vector2f normal = Vector2f(-direction(1), direction(0)).normalized();
		normal(0) *= margin(0);
		normal(1) *= margin(1);

		Vector2f click(x, y);


		//  Area is guaranteed to be either rectangular or trapezoidal
		//  If sum of areas for triangles between vertices of 
		//	trapezoid/rectangle are greater than the trapezoid's/rectangle's 
		//	area, then the point is not within the trapezoid
		// 
		//   p1  p4
		//    s41
		//   ┌───┐
		//   │\ /│
		//s12│ x │s34
        //   │/ \│
        //   └───┘
		//    s23
		//  p2   p3

		Vector2f p1 = start + normal;
		Vector2f p2 = start - normal;
		Vector2f p3 = end - normal;
		Vector2f p4 = end + normal;

		float l1 = (click - p1).norm();
		float l2 = (click - p2).norm();
		float l3 = (click - p3).norm();
		float l4 = (click - p4).norm();


		float s12 = (p2 - p1).norm();
		float s23 = (p3 - p2).norm();
		float s34 = (p4 - p3).norm();
		float s41 = (p1 - p4).norm();

		
		float a1 = heronMethod(l1, l2, s12);
		float a2 = heronMethod(l2, l3, s23);
		float a3 = heronMethod(l3, l4, s34);
		float a4 = heronMethod(l4, l1, s41);

		float area = s23 * s34;
		float sumareas = a1 + a2 + a3 + a4;
		if(area == sumareas){
			start_storage = &equations[i];
			end_storage = &equations[i+1];
			return true;
		}
	}
	return false;
}




#endif
