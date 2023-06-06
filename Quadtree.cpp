// Kody Manastyrski
// Quadtree.cpp
//

#include <stdlib.h>

#ifndef QUAD
#include "Quadtree.hpp"

template <typename E>
Quadtree<E>::Quadtree(unsigned short res){
	resolution = res;
	values = (E*)malloc(sizeof(E) * resolution); 
	count = 0;
}


template <typename E>
Quadtree<E>::Quadtree(E value, unsigned short res){
	resolution = res;
	values = (E*)malloc(sizeof(E)*resolution);
	*values = value;
	count = 1;
}




template <typename E>
Quadtree<E>::Quadtree(const Quadtree<E> & other){
	this->resolution = other->resolution;
	this->values = other->values;
	this->count = other->count;
}


template <typename E>
Quadtree<E>::Quadtree(Quadtree<E> & other){
	this->resolution = other->resolution;
	this->values = other->values;
	this->count = other->count;
}


template <typename E>
Quadtree<E>::~Quadtree(){
	free(this->values);
	this->values = NULL;
	this->resolution = -1;
	this->count = -1;
}


template <typename E>
Quadtree<E> * Quadtree<E>::operator=(Quadtree<E> other){
	return new Quadtree(other);
}



template <typename E>
void Quadtree<E>::insert(E item){
	if(count < resolution - 1){
		values[count + 1] = item;
	}
	else{
		count = -1;
		
	}
}

#endif
