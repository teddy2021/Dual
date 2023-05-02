// Kody Manastyrski
//Display.hpp
//

#include "ogl.hpp"
#include "Shader.hpp"
#include "DataModel.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <eigen3/Eigen/Dense>

using Eigen::VectorXd;
using Eigen::Matrix4f;

#ifndef DISPLAY
#define DISPLAY

class Display{

	private:
		GLFWwindow * window;
		VectorXd backgroundColour;
		DataModel * model;
		GLuint * buffers;
		GLuint matrixID;
		Matrix4f primal, dual, view, projection;
		GLuint vao;

		Shader shader;

		int width, height;

		void draw();
		void recreateMatrices();

	public:
		Display(DataModel * m);
		
		void updatePoints();
		void updateEquations();
		void redraw();

		void mainLoop();

		GLFWwindow * getWindow();

		int getWidth(){ return width;};
		int getHeight(){return height;};

		void updateWidth(int w);
		void updateHeight(int h);

};

#endif
