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
	/**
	 * DataModel()
	 * empty constructor
	 * no preconditions or postconditions
	 **/

	equationCount = 0;
	pointCount = 0;
}


void DataModel::addEquation(Vector2f start, Vector2f end){
	/**
	 * addEquation(start, end)
	 * A method to track an equation in the model
	 * Preconditions: none
	 * Params:
	 * 		start, Vector2f: a vector representing one endpoint (as cartesian 
	 * 		coordinates) of the equation.
	 * 		end, Vector2f: a vector representing the other endpoint (as cartesian
	 * 		coordinates) of the equation.
	 * Ideally the controller will prevent start = end conditions, but the model
	 * will allow them and attempt to draw them as lines.
	 * Postconditins: the model now stores the equation, and will attempt to draw
	 * it and its dual.
	 **/
	// store the equation and its indices
	equations.push_back(start);
	eqnIndices.push_back(equations.size() - 1);
	equations.push_back(end);
	eqnIndices.push_back(equations.size() - 1);
	equationCount += 1;

	// calculate the dual of the equation
	Vector2f dir = end-start;
	Vector2f dual;
	dual << -dir(0), -dir(1);

	// store the dual
	dEquations.push_back(dual);
	dualEqnIndices.push_back(dEquations.size() - 1);
}

void DataModel::addPoint(Vector2f p){
	/**
	 * addPoint(p)
	 * A method to track a point in the model.
	 * Preconditions: none
	 * Params:
	 * 		p, Vector2f: a vector representing the point in cartesian coordinates
	 * Postconditions: the point is tracked by the model, and the model will 
	 * attempt to draw the point and its dual.
	 **/
	// store the point and its index
	points.push_back(p);
	pointIndices.push_back(points.size() - 1);

	// calculate the dual of the point
	float m = -p(0);
	float b = p(1);

	float y1 = m*0 + b;
	float y2 = m*1 + b;

	Vector2f start(0.f, y1);
	Vector2f end(1.f, y2);

	// store the dual of the point
	dPoints.push_back(start);
	dualPntIndices.push_back(dPoints.size() - 1);
	dPoints.push_back(end);
	dualPntIndices.push_back(dPoints.size() - 1);

	pointCount += 1;
}

void DataModel::removeEquation(Vector2f start, Vector2f end){
	/**
	 * removeEquation(start, end)
	 * A method to remove an equation from the model.
	 * Preconditions: none
	 * Params: 
	 * 		start, Vector2f: one of the equation's endpoints as cartesian coordinates
	 * 		end, Vector2f: one of the equation's endpoints as cartesian coordinates
	 * Ideally the controller will prevent start = end conditions.
	 * Postconditions: the equation is no longer tracked by the model, and will 
	 * no longer be drawn.
	 **/
	vector<Vector2f>::iterator eqn = getEquationIterator();
	int idx = 0;
	bool found = false;
	// get the equation index
	while(!equationIteratorAtEnd(eqn + 1)){
		if(*eqn == start && *(eqn + 1) == end){
			found = true;
			break;
		}
		idx += 1;
		eqn += 2;
	}

	// remove the equation and its dual
	if(found){
		equations.erase(eqn, eqn + 1);
	

		vector<Vector2f>::iterator dEqn = getDualEquationIterator();
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
	/**
	 * removePoint(p)
	 * A method to remove an equation from the model.
	 * Preconditions: none
	 * Params: 
	 * 		p, Vector2f: one of the point endpoints as cartesian coordinates
	 * Postconditions: the point is no longer tracked by the model, and will 
	 * no longer be drawn.
	 **/
	vector<Vector2f>::iterator pnt = getPointIterator();
	int idx = 0;
	bool found = false;
	// locate the point to get the index.
	while(!pointIteratorAtEnd(pnt)){
		if(*pnt == p){
			found = true;
			break;
		}
		idx += 1;
		pnt += 1;
	}

	// delete the point and its dual
	if(found){
		equations.erase(pnt);
	
		vector<Vector2f>::iterator dPnt = getDualPointIterator();
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
	/**
	 * getDataPointer(set): vector<Vector2f>*
	 * A method to get the pointer to a given dataset (in order its points, 
	 * equations, dual points, and dual equations). To be used only to expose 
	 * data to openGL ONLY
	 * Preconditions: none
	 * Params:
	 * 		set, int: the index of the set (in respective ordering as abowe).
	 * Returns: a pointer to the requested dataset, NOTE: this may be a NULL 
	 * pointer if the dataset was never init'd, check your returns!
	 * Postconditions: none.
	 **/
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
	/**
	 * getIndexPointer(set): vector<GLuint>*
	 * A method to get the pointer to the indices used for drawing the respective
	 * items in openGL.
	 * Preconditions: none
	 * Params:
	 * 		set, int: The index of the set, ordered as points, equations, dual
	 * 		points, dual equations.
	 * Returns: a pointer to the set of indices, NOTE: this may be NULL if the 
	 * dataset was never init'd, check your returns!
	 * Postconditions: none.
	 **/
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


vector<Vector2f>::iterator  DataModel::getEquationIterator(){
	/**
	 * equationIterator(): vector<Vector2f>::iterator
	 * A way to access the iterator for the model's tracked equations.
	 * No Pre/Postconditions.
	 * Returns an iterator for the equations.
	 **/
	return equations.begin();
}

vector<Vector2f>::iterator DataModel::getPointIterator(){
	/**
	 * pointIterator(): vector<Vector2f>::iterator
	 * A way to access the iterator for the model's tracked points.
	 * No Pre/Postconditions.
	 * Returns an iterator for the points.
	 **/
	return points.begin();
}

vector<Vector2f>::iterator DataModel::getDualEquationIterator(){
	/**
	 * dualEquationIterator(): vector<Vector2f>::iterator
	 * A way to access the iterator for the model's tracked dual equations.
	 * No Pre/Postconditions.
	 * Returns an iterator for the dual equations.
	 **/
	return dEquations.begin();
}

vector<Vector2f>::iterator DataModel::getDualPointIterator(){
	/**
	 * equationIterator(): vector<Vector2f>::iterator
	 * A way to access the iterator for the model's tracked dual points.
	 * No Pre/Postconditions.
	 * Returns an iterator for the dual points.
	 **/
	return dPoints.begin();
}

bool DataModel::pointIteratorAtEnd(vector<Vector2f>::iterator it){
	/**
	 * pointIteratorAtEnd(it): bool
	 * A method to check if the point or dual equation iterator is at its end.
	 * No Pre/Postconditions.
	 * Params:
	 * 		it, vector<Vector2f>: the iterator being checked.
	 * Returns true if the iterator is at the end of its set. NOTE: if the 
	 * iterator is not for either of the expected sets, this will always return
	 * false.
	 **/
	return it == points.end() || it == dEquations.end();
}

bool DataModel::equationIteratorAtEnd(vector<Vector2f>::iterator it){
	/**
	 * equationIteratorAtEnd(it): bool
	 * A method to check if the dual point or equation iterator is at its end.
	 * No Pre/Postconditions.
	 * Params:
	 * 		it, vector<Vector2f>: the iterator being checked.
	 * Returns true if the iterator is at the end of its set. NOTE: if the 
	 * iterator is not for either of the expected sets, this will always return
	 * false.
	 **/
	return it == equations.end() || it == dPoints.end();
}

int DataModel::getEquationCount(){
	/**
	 * getEquationCount(): int
	 * A method to get the count of equations (and by extension, their duals)
	 * No Pre/Postconditions
	 * Returns: the count of equations
	 **/
	return equationCount;
}

int DataModel::getPointCount(){
	/**
	 * getPointCount(): int
	 * A method to get the count of points (and by extension, their duals)
	 * No Pre/Postconditions
	 * Returns: the count of points
	 **/
	return pointCount;
}


bool DataModel::getPoint(float x, float y, Vector2f *& storage){
	/**
	 * getPoint(x, y, storage): bool
	 * A method to get the point in a margin around the given cartesian 
	 * coordinates and store it in the provided storage variable.
	 * No Preconditions.
	 * Params:
	 * 		x, float: the x coordinate of the querry point. Should be normalized.
	 * 		y, float: the y coordinate of the querry point. Should be normalized.
	 * 		storage, Vector2f *&: a provided variable for point output.
	 * Postconditions: storage will have the found point if found.
	 * Returns: true if a point is found, false otherwise.
	 **/

	float left_bound = x - 0.01;
	float right_bound = x + 0.01;
	float upper_bound = y + 0.01;
	float lower_bound = y - 0.01;


	vector<Vector2f>::iterator pts = getPointIterator();

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
	/**
	 * heronMethod(l1, l2, l3): float
	 * a method to get the area of a triangle.
	 * No Pre/Postconditions
	 * Params:
	 * 		l{1,2,3}, float: the length of one of the edges of the triangle
	 * Returns: the area of the triangle. NOTE, this uses a square root.
	 **/
	float semiperim = (l1 + l2 + l3)/2.f;
	float area = sqrt(semiperim * (semiperim - l1) * (semiperim - l2) * (semiperim - l3));
	return area;
}

bool DataModel::getLine(float x, float y, 
		Vector2f *& start_storage, Vector2f *& end_storage, 
		Vector2f margin){
	/**
	 * getLine(x, y, start_storage, end_storage, margin): bool
	 * A method to get a line within a given margin around the given cartesian 
	 * coordinates and store it in the provided storage variables. 
	 * No Preconditions.
	 * Params:
	 * 		x, float: the x coordinate of the querry point. Should be normalized.
	 * 		y, float: the y coordinate of the querry point. Should be normalized.
	 * 		{start, end}_storage, Vector2f *&: a provided variable for point output.
	 * 		margin, Vector2f: the scale which creates the margin around the given
	 * 		coorinates.
	 * Postconditions: storage will have the found equation if found.
	 * Returns: true if a equation is found, false otherwise.
	 **/

	// find the first most relevant equation
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

		// create the points of the bounding trapezoid
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

		
		// calculate the triangular area for each triangle created by each 
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
