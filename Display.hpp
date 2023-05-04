// Kody Manastyrski
//Display.hpp
//

#include "ogl.hpp"
#include "Shader.hpp"
#include "DataModel.hpp"
#include "Point.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <eigen3/Eigen/Dense>

using Eigen::VectorXd;
using Eigen::Matrix4f;
using Eigen::Vector2f;

#ifndef DISPLAY
#define DISPLAY

struct VectorSet {
    std::vector<Eigen::Vector2f> vertices;
    std::vector<int> indices;
};


class Display{

	private:
		GLFWwindow * window;
		VectorXd backgroundColour;
		DataModel * model;
		
		Matrix4f primal, dual, view, projection;
		GLuint vao;
		
		GLuint counts[4];
		GLuint buffers[4];
		GLuint indices[4];

		VectorSet objectVertices[4];
		GLuint matrixID;
		GLuint colourID;

		Shader shader;

		int width, height;

		void draw();
		void recreateMatrices();

	private:
		void addVector(int set_index, Vector2f v);

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

		Point getMousePosition();

};

#endif
