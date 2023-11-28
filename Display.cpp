// Kody Manastyrski
// Display.cpp
//

#include "ogl.hpp"
#include "Shader.hpp"
#include "Equation.hpp"
#include "Enums.hpp"

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Geometry>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <numeric>
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

using Eigen::VectorXd;
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix4f;

using std::string;

#ifndef DISPLAY
#include "Display.hpp"

bool write = false;

void Display::enterDrawState(){
	glBindVertexArray(vao);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::exitDrawState(){

	// display the frame
	glfwSwapBuffers(window);
	// get input
	glfwPollEvents();
}

void Display::draw(){

	enterDrawState();
	draw_state types[4] = {point,line,line,point};
	Vector4f colour[2] = {{1.0f,1.0f,0,1.0f}, {1.0f,0,1.0f,1.0f}};

	for(int i = 0; i < 4; i += 1){

		// activate the shader
		shader.activate();
		if(i < 2){
			shader.setUniform(matrixID, (void *)primal.data(), 1);
			shader.setUniform(colourID, (void *)colour[0].data(), 2);
		}
		else{
			shader.setUniform(matrixID, (void *)dual.data(), 1);
			shader.setUniform(colourID, (void *)colour[1].data(), 2);
		}

		// hand control over to the gpu
		ogl::draw(
				window, // screen
				buffers[i], // buffer
			   	counts[i], // count
				types[i], // type
				indices[i],  // indices
				0); // layout variable
	}

	renderText();
	exitDrawState();
}

Display::Display(DataModel * m): textVerts(36), textIndices(36*4) {
	width = 1080;
	height = 608;
	window = setup(width, height);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	shader = Shader("primal");
	text = Shader("Text");

	model = m;

	backgroundColour = VectorXd(4);
	backgroundColour << 1, 1, 1, 1;
	shader.activate();
	for(int i = 0; i < 4; i += 1){
		glGenBuffers(1, &buffers[i]);
		glGenBuffers(1, &indices[i]);
	}

	matrixID = shader.getUniform("mvp");
	colourID = shader.getUniform("colour");
	
	glViewport(0,0, width, height);
	glPointSize(4);

	text.activate();
	glGenVertexArrays(1, &textArray);
	glGenBuffers(1, &textBuffer);
	
	glBindVertexArray(textArray);
	glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,4*sizeof(float), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	text_projeciton = text.getUniform("projection");
	text_colour = text.getUniform("textColour");
	text_sampler = text.getUniform("text");
	textColour = Vector4f(1.0f,1.0f,1.0f,1.0f);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1 );


	loadFont();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	recreateMatrices();
}

bool Display::loadFont(){
	FT_Library ft;
	FT_Face face;

	// initalize the font loading library
	if( FT_Init_FreeType(&ft)){
		perror("Failed to initialized Freetype library");
		return false;
	}

	// create a fontface
	if(FT_New_Face(ft, "../Resources/fonts/computer.ttf", 0, &face)){
		perror("Failed to get fontface");
		return false;
	}
	else{
		printf("Found the font face.\n");		
	}

	// setup preferential values
	FT_Set_Pixel_Sizes(face, 0, 48);
	if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)){
		perror("Failed to load test glyph");
		return false;
	}

	// load each ASCII glyph of the font to a texture
	for(unsigned char c = 0; c < 128; c += 1 ){
	
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
			perror("Failed to load character");
			return false;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
				);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// store the attributes to a structure
		Character character = {
			texture,
			Vector2f(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Vector2f(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		// place the letter into a structure for quick reference
		alphabet.insert(std::pair<char, Character> (c, character));
	}

	// cleanup the environment
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	return true;
}

void Display::updatePoints(){

	// get the buffers relevant to the points
	GLuint pointBuffer = buffers[0];
	GLuint pointIndices = indices[0];

	// set the environment context for the points
	ogl::bind(pointBuffer, model->getDataPointer(0),
			pointIndices, model->getIndexPointer(0));

	// get the buffers relevant to the dual of the points
	GLuint dualPointBuffer = buffers[2];
	GLuint dualIndices = indices[2];

	// set tho environment context for the dual of the points
	ogl::bind(dualPointBuffer, model->getDataPointer(2),
			dualIndices, model->getIndexPointer(2));

	// update the counts for later use
	counts[0] = model->getPointCount();
	counts[2] = 2*model->getPointCount();
	
	// update the screen
	redraw();
}

void Display::updateEquations(){

	// get the equation relevant buffers and set the relevant GPU values
	GLuint eqnBuffer = buffers[1];
	GLuint eqnIndices = indices[1];
	ogl::bind(eqnBuffer, model->getDataPointer(1), 
			eqnIndices, model->getIndexPointer(1));

	// get the dual relevant buffers and set the relevant GPU values
	GLuint dualEqnBuffer = buffers[3];
	GLuint dualEqnIndices = indices[3];
	ogl::bind(dualEqnBuffer, model->getDataPointer(3),
			dualEqnIndices, model->getIndexPointer(3));
	
	// update the counts for later use
	counts[1] = 2*model->getEquationCount();
	counts[3] = model->getEquationCount();

	// update the frame
	redraw();
}

void rebind(draw_state type){

}

void Display::redraw(){
	draw();
}

void Display::mainLoop(){
	do{
		draw();
	}while( glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
			glfwWindowShouldClose(window) == 0);
}

GLFWwindow * Display::getWindow(){
	return window;
}

Matrix4f Display::calculateProjection(float left, float right, 
		float bottom, float top,
		float near, float far){

	// get the bounds of the viewing volume as ratios
	float a = -(right+left)/(right-left);
	float b = -(bottom+top)/(bottom - top);
	float c = -(near + far)/(near-far);
	Matrix4f ret;
	// set the projection matrix values
	ret << 	2.f/(right - left), 	0, 						0, 					a,
			0,						2.f/(top - bottom), 	0, 					b,
			0,						0, 						1.f/(near - far), 	c,
			0,						0, 						0, 					1;
	return ret;
}


void Display::recreateMatrices(){

	// set the model matrix values for the primal and dual displays
	Matrix4f model_left, model_right;
	model_left 		<< 	width,0,0,0,
			   			0,height/2.f,0,0,
			   			0,0,1,0,
			   			0,0,0,1;

	model_right 	<< 	width/4.f,0,0,width/16.f,
						0,height/4.f,0,-height / 4.f,
						0,0,0,0,
						0,0,0,1;


	// calculate the orientation vectors
	Vector3f position, up, target;
	position << 0,0,-1;
	up << 0,1,0;
	target << 0,0,1;
	Vector3f rgt = position.cross(up).normalized();

	// set the view matrix values
	view << 
		rgt(0), 	rgt(1), 	rgt(2), 	-rgt.dot(position),
		up(0), 		up(1), 		up(2), 		-up.dot(position),
		target(0), 	target(1), 	target(2), 	target.dot(position),
		0, 			0, 			0, 			1;


	// set the view bounds
	float left = 	-(float)width/2.f;
	float right = 	(float)width/2.f;

	float bottom = 	-(float)height/2.f;
	float top = 	(float)height/2.f;

	float near = 	-0.5f;
	float far = 	1.f;

	projection 	= calculateProjection(left, right, bottom, top, near, far);

	textProjection = calculateProjection(0.f, width, 0, height, -1.f, 1.f);

	// calculate the final MVP Matrices for each display
	primal 	= projection 	* view	* model_left;
	dual 	= projection 	* view 	* model_right;
}


void Display::updateWidth(int w){
	width = w;
	recreateMatrices();
}

void Display::updateHeight(int h){
	height = h;
	recreateMatrices();
}

Point Display::getMousePosition(){
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	return Point(x, y);
}



void Display::clearTextBuffer(){
	textVerts.clear();
	textIndices.clear();
}

void Display::updateTextBuffer(){

	string::const_iterator c = state.begin();
	const double xbound = 4.f;
	double x = xbound;
	double y = -height * 0.9f;
	double scale = 0.35;
	float heightmax = 0;
	
	// render individual characters
	while(c != state.end() && *c != '\0'){
		// move the cursor to the start of the next line if the character is a newline
		if(*c == '\n'){
			x = xbound;
			y += heightmax;
			c += 1;
			continue;
		}
		// get the attributes
		Character ch = alphabet[*c];

		// calculate the positioning, width and height
		float xpos = x + ch.bearing(0) * scale;
		float ypos = y - (ch.size(1) - ch.bearing(1)) * scale;

		float w = ch.size(0) * scale;
		float h = ch.size(1) * scale;

		if(h > heightmax){
			heightmax = h *1.1f;
		}

		// create the square that will display the charactera
		Vector4f topLeft, bottomLeft, topRight, bottomLeft, bottomRight;
		topLeft 	<< 	xpos, 		ypos + h, 	0.f,	0.f; // top left
		bottomRight 	<< 	xpos, 		ypos,		0.f, 	1.f; // botton left
		bottomLeft 	<< 	xpos + w, 	ypos,		1.f, 	1.f; // bottom right
		topRight 	<< 	xpos + w,	ypos,		1.f, 	1.f; // bottom right
		
		int baseIDX = textVerts.size() - 1;
		textVerts.push_back(topLeft);
		textVerts.push_back(bottomRight);
		textVerts.push_back(bottomLeft);
		textVerts.push_back(topRight);

		textIndices.push_back(baseIDX + 0);
		textIndices.push_back(baseIDX + 3);
		textIndices.push_back(baseIDX + 1);
		textIndices.push_back(baseIDX + 2);
		textIndices.push_back(baseIDX + 4);
		textIndices.push_back(baseIDX + 5);
	
		// tl:0,3
		// bl:1
		// br:2,4
		// tr:5

		// set the glyph

		
	   x += (ch.Advance >> 6) * scale;
	   c += 1;
	}
	// set the GPU variables and hand control over
	ogl::rebindText();
}

void Display::setText(string txt){
	state = txt;
	write = true;
	updateTextBuffer();
}


void Display::enterTextContext(){
	glBindVertexArray(textArray);
	glActiveTexture(GL_TEXTURE0);
	text.activate();
	glBindVertexArray(textArray);
	glActiveTexture(GL_TEXTURE0);
	text.setUniform(text_projeciton, (void *)textProjection.data(), 1);
	text.setUniform(text_colour, (void *)textColour.data(), 2);
}

void Display::renderText(){
	// output for debugging purposes
	if(write){
	}

	enterTextContext();

	text.setUniform(text_sampler, &ch.texture, 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	//init variables for drawing
	// cleanup
	exitTextContext();
}

void Display::exitTextContext(){
	write = false;
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

#endif
