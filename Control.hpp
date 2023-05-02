// Kody Manastyrski
// Control.hpp
//

#include "DataModel.hpp"
#include "Display.hpp"
#include <GLFW/glfw3.h>

#ifndef CONTROL
#define CONTROL

class Controller{

	private:
	   	DataModel * model;
		double  prev_x, prev_y;
		double  dx, dy;
		Display * display;

	public:
		Controller(DataModel * m){model = m;};
		void setDisplay(Display * d){display = d;};

		void updateMousePos(double x, double y);
		
		void onLeftDown();
		void onRightDown();
		
		void onLeftUp();
		void onRightUp();

		void handleKeys(GLFWwindow * window, int key, 
				int scancode, int action, int mods);
};

#endif
