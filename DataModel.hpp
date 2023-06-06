// Kody Manastyrski
// DataModel.hpp
//

#include "Equation.hpp"
#include "Point.hpp"
#include "Enums.hpp"
#include <vector>
#include <eigen3/Eigen/Dense>
#include <GL/glew.h>

using Eigen::Vector2f;
using std::vector;

#ifndef MODEL
#define MODEL

class DataModel{

	private:
		vector<Vector2f> points;
		vector<GLuint> pointIndices;

		vector<Vector2f> equations;
		vector<GLuint> eqnIndices;
		
		vector<Vector2f> dPoints;
		vector<GLuint> dualPntIndices;

		vector<Vector2f> dEquations;
		vector<GLuint> dualEqnIndices;

		int equationCount;
		int pointCount;
	
	public:
	
		DataModel();

		void addEquation(Vector2f start, Vector2f end);
		void addPoint(Vector2f p);

		void removeEquation(Vector2f start, Vector2f end );
		void removePoint(Vector2f p);

		vector<Vector2f> * getDataPointer(int set);
		vector<GLuint> * getIndexPointer(int set);

		vector<Vector2f>::iterator equationIterator();
		vector<Vector2f>::iterator pointIterator();

		vector<Vector2f>::iterator dualEquationIterator();
		vector<Vector2f>::iterator dualPointIterator();
		bool equationIteratorAtEnd(vector<Vector2f>::iterator it);
		bool pointIteratorAtEnd(vector<Vector2f>::iterator it);

		int getEquationCount();
		int getPointCount();

		bool getPoint(float x, float y, Vector2f & storage);
		bool getLine(float x, float y, Vector2f & start_storage, Vector2f & end_storage);
		
};

#endif
