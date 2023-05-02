// Kody Manastyrski
// test.cpp

#include "Equation.hpp"
#include "Point.hpp"
#include "Util.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using std::vector;


int main(){

	float c1[] = {1.0, 2.0, 3.0, 4.0};
	float c2[] = {11.0, 23.0, 4.0, 66.0, 99.0, 1234.0};

	Equation f (3, c1);
	Equation g (5, c2);

	LinearEquation h (10, 20);
	LinearEquation i (2./3., 10);

	std::cout << "f(x) = " << 
		f.toString() << " f(5) = " <<
	   	f.calculateY(5) << std::endl;

	std::cout << "g(x) = " << 
		g.toString() << "g(5) = " <<
	   	g.calculateY(5) << std::endl;
	
	std::cout << "h(x) = " << 
		h.toString() << " h(5) = " <<
	   	h.calculateY(5) << std::endl;
	
	std::cout << "i(x) = " << 
		i.toString() << " i(5) = " <<
	   	i.calculateY(5) << std::endl;

	Point p;
	Point q (1, 2);

	std::cout << "p: " << p.toString() << ", q: " << q.toString() << std::endl;

	LinearEquation j = convert(p);
	LinearEquation k = convert(q);

	std::cout << "j(x) = " << 
		j.toString() << " i(5) = " <<
	   	j.calculateY(5) << std::endl;
	
	std::cout << "k(x) = " << 
		k.toString() << " k(5) = " <<
	   	k.calculateY(5) << std::endl;

	Point u = convert(h);
	Point v = convert(i);


	std::cout << "u: " << u.toString() << ", v: " << v.toString() << std::endl;

	LinearEquation inter = interpolate(u, v);
	std::cout << "inter(x): " << inter.toString() << ", inter(5): " << 
		inter.calculateY(5) << std::endl;

	LinearEquation * lPtrTest1 = new LinearEquation(5,2.5);

	std::cout << "ptrTest1: " << lPtrTest1->toString() << ", eval'd at 5: " <<
	   	lPtrTest1->calculateY(5) << std::endl;
	
	LinearEquation * lPtrTest2 = new LinearEquation();
	*lPtrTest2 = LinearEquation(1,2);
	std::cout << "ptrTest2: " << lPtrTest2->toString() << ", eval'd at 5: " <<
	   	lPtrTest2->calculateY(5) << std::endl;

	vector<LinearEquation> laPtrTest;
	for(int i = 0; i < 10; i += 1){
		float m = rand() % 20;
		float b = rand() % 20;
		laPtrTest.push_back(LinearEquation(m, b));

		std::cout << m << "x + " << b << " equation gives -> " <<
		   	laPtrTest[i].toString() << "; eval'd at 5: " << 
			laPtrTest[i].calculateY(5) << std::endl;
	}

}
