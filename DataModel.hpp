// Kody Manastyrski
// DataModel.hpp
//

#include "Equation.hpp"
#include "Point.hpp"
#include <vector>

using std::vector;

#ifndef MODEL
#define MODEL

class DataModel{

	private:
		vector<LinearEquation> equations;
		vector<Point>  points;
		
		vector<LinearEquation> dPoints;
		vector<Point> dEquations;

		float ** intervals;

		int equationCount;
		int pointCount;
	
	public:
	
		DataModel();

		void addEquation(LinearEquation l, float start, float end);
		void addPoint(Point p);

		void removeEquation(LinearEquation l);
		void removePoint(Point p);

		vector<LinearEquation>::iterator getEIterator();
		vector<Point>::iterator getPIterator();

		vector<Point>::iterator getDEIterator();
		vector<LinearEquation>::iterator getDPIterator();
		bool iteratorAtEnd(vector<Point>::iterator it);
		bool iteratorAtEnd(vector<LinearEquation>::iterator it);

		float ** getIntervals();

		int getEquationCount();
		int getPointCount();
};

#endif
