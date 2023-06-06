// Kody Manastyrski
// Quadtree.hpp
//
#include "GraphicalObject.hpp"

#ifndef QUAD
#define QUAD

class Quadtree{

	private:
		Quadtree children[4];
		unsigned short resolution;
		GraphicalObject * values;
		unsigned int count;
		
	public:
		Quadtree (unsigned short rso=1);
		Quadtree (GraphicalObject value, unsigned short res=1);
		
		Quadtree (const Quadtree & other);
		Quadtree (Quadtree & other);

		~Quadtree ();

		Quadtree * operator=(Quadtree other);

		void insert(GraphicalObject item);
		void remove(GraphicalObject item);

};

#endif

