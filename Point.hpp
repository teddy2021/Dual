// Kody Manastyrski
// Point.h
//
#include <string>
#ifndef PT
#define PT

class Point{

	private:
		float x;
		float y;

	public:
		Point();
		Point(float a, float b);

		Point(const Point &other);
		~Point();

		void setX(float newX);
		void setY(float newY);

		float getX();
		float getY();

		std::string toString();

		bool operator==(Point other);
		bool operator!=(Point other);

		Point& operator=(const Point &other);

};

#endif
