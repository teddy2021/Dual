// Kody Manastyrski
// GraphicalObject.hpp
//

#ifndef GOBJ
#define GOBJ

template <typename E>
class GraphicalObject{

	private:
		E * item;
		float posX, posY;

	public:
		GraphicalObject<E>();
		GraphicalObject<E>(E val, float x = 0, float y=0);

		GraphicalObject<E>(GraphicalObject<E> & other);
		~GraphicalObject<E>();

		void updateX(float newVal);
		void updateY(float newVal);

		GraphicalObject<E> * operator=(GraphicalObject<E> other);

		bool operator==(GraphicalObject<E> other);


};


#endif
