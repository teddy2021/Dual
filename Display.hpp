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

#include <string>
#include <map>

using Eigen::VectorXd;
using Eigen::Matrix4f;
using Eigen::Vector4f;
using Eigen::Vector2f;
using std::string;
using std::map;

#ifndef DISPLAY
#define DISPLAY


struct Character {
	unsigned int texture;
	Vector2f size;
	Vector2f bearing;
 	long Advance;
};

class Display{

	private:

		map<char, Character> alphabet;
		bool loadFont();

		GLFWwindow * window;
		VectorXd backgroundColour;
		DataModel * model;
		
		Matrix4f primal, dual, view, projection, textProjection;
		GLuint vao;
		
		GLuint counts[4];
		GLuint buffers[4];
		GLuint indices[4];

		GLuint matrixID;
		GLuint colourID;


		Shader shader;

		GLuint text_projeciton;
		GLuint text_colour;
		GLuint text_sampler;

		Shader text;

		GLuint textArray;
		GLuint textBuffer;

		string state;
		Vector4f textColour;

		int width, height;

		void draw();
		void recreateMatrices();
		
		Matrix4f calculateProjection(float left, float right, 
				float bottom, float top,
				float near, float far);

		void renderText();

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

		void setText(string txt);

};

#endif
