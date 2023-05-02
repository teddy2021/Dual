// Kody Manastyrski
// dual.cpp
//


#include "Display.hpp"
#include "DataModel.hpp"
#include "Control.hpp"
#include <GLFW/glfw3.h>

Controller * control;

static void mouseMove(GLFWwindow * window, double x, double y){
	control->updateMousePos(x, y);
}

static void keyPress(GLFWwindow * window, int key, int scancode, int action, int mods){
	control->handleKeys(window, key, scancode, action, mods);
}

static void mouseButton(GLFWwindow * window, int button, int action, int mods){
	switch(button){
		case GLFW_MOUSE_BUTTON_LEFT:
			switch(action){
				case GLFW_PRESS:
					control->onLeftDown();
					break;
				case GLFW_RELEASE:
					control->onLeftUp();
					break;
			}
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			switch(action){
				case GLFW_PRESS:
					control->onRightDown();
					break;
				case GLFW_RELEASE:
					control->onRightUp();
					break;
			}
			break;
	}
}


int main(){

	DataModel model;

	Display display(&model);

	control = (Controller *)malloc(sizeof(Controller));
	*control = Controller(&model);
	control->setDisplay(&display);

	glfwSetCursorPosCallback(display.getWindow(), mouseMove);
	glfwSetMouseButtonCallback(display.getWindow(), mouseButton);
	glfwSetKeyCallback(display.getWindow(), keyPress);


	display.mainLoop();

}

