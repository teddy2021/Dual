// Kody Manastyrski
// Control.hpp
//

#include "DataModel.hpp"
#include "Display.hpp"
#include "Enums.hpp"
#include "Equation.hpp"
#include <GLFW/glfw3.h>
#include <eigen3/Eigen/Dense>

using Eigen::Vector2f;
#ifndef CONTROL
#define CONTROL

class Controller{

	private:
	   	mouse_state ms;
		draw_state ds;

		Vector2f origin;
		DataModel * model;
		double  prev_x, prev_y;
		double  dx, dy;
		Display * display;

		float transformWidth(float initial, float offset);
		float transformHeight(float initial, float offset);

	public:
		Controller(DataModel * m){model = m; ms = neutral;};
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
