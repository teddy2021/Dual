// Kody Manastyrski
// GraphicalObject.cpp
//

#include <stdlib.h>

#ifndef GOBJ
#include "GraphicalObject.hpp"

template <typename E>
GraphicalObject<E>::GraphicalObject(){
	item = (E *)malloc(sizeof(E));
}


template <typename E>
GraphicalObject<E>::GraphicalObject(E val, float x, float y){
	item = (E *)malloc(sizeof(E));
	*item = val;
	posX = x;
	posY = y;
}




template <typename E>
GraphicalObject<E>::GraphicalObject(GraphicalObject<E> & other){
	item = (E*)malloc(sizeof(E));
	*item = *( other->item );
	posX = other->posX;
	posY = other->posY;
}


template <typename E>
GraphicalObject<E>::~GraphicalObject(){
	free(item);
	item = NULL;
	posX = 0;
	posY = 0;
}




template <typename E>
void GraphicalObject<E>::updateX(float newVal){
	posX = newVal;
}


template <typename E>
void GraphicalObject<E>::updateY(float newVal){
	posY = newVal;
}




template <typename E>
GraphicalObject<E> * GraphiicalObject<E>::operator=(GraphicalObject<E> other){
	item = (E *)malloc(sizeof(E));
	*item = other->item;
	posX = other->posX;
	posY = other->posY;
}




template <typename E>
bool GraphicalObject<E>::operator==(GraphicalObject<E> other){
	return *item == other->item &&
		posX == other->posX &&
		posY == other->posY;
}

#endif
