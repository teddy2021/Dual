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

void Display::draw(){

	glBindVertexArray(vao);
	draw_state types[4] = {point,line,line,point};
	Vector4f colour[2] = {{1.0f,1.0f,0,1.0f}, {1.0f,0,1.0f,1.0f}};



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(int i = 0; i < 4; i += 1){

		shader.activate();
		if(i < 2){
			shader.setUniform(matrixID, (void *)primal.data(), 1);
			shader.setUniform(colourID, (void *)colour[0].data(), 2);
		}
		else{
			shader.setUniform(matrixID, (void *)dual.data(), 1);
			shader.setUniform(colourID, (void *)colour[1].data(), 2);
		}

		ogl::draw(
				window, // screen
				buffers[i], // buffer
			   	counts[i], // count
				types[i], // type
				indices[i],  // indices
				0); // layout variable
	}

	glBindVertexArray(0);

	renderText();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

Display::Display(DataModel * m){
	state = "normal\n\n\nselecting";
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


	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
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

	if( FT_Init_FreeType(&ft)){
		perror("Failed to initialized Freetype library");
		return false;
	}
	if(FT_New_Face(ft, "../Resources/fonts/computer.ttf", 0, &face)){
		perror("Failed to get fontface");
		return false;
	}
	else{
		printf("Found the font face.\n");		
	}

	FT_Set_Pixel_Sizes(face, 0, 48);
	if(FT_Load_Char(face, 'X', FT_LOAD_RENDER)){
		perror("Failed to load test glyph");
		return false;
	}

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

		Character character = {
			texture,
			Vector2f(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Vector2f(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		alphabet.insert(std::pair<char, Character> (c, character));


	}

	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);


	return true;
}

void Display::updatePoints(){

	GLuint pointBuffer = buffers[0];
	GLuint pointIndices = indices[0];

	ogl::bind(pointBuffer, model->getDataPointer(0),
			pointIndices, model->getIndexPointer(0));

	GLuint dualPointBuffer = buffers[2];
	GLuint dualIndices = indices[2];

	ogl::bind(dualPointBuffer, model->getDataPointer(2),
			dualIndices, model->getIndexPointer(2));
	counts[0] = model->getPointCount();
	counts[2] = 2*model->getPointCount();
	redraw();
}

void Display::updateEquations(){

	GLuint eqnBuffer = buffers[1];
	GLuint eqnIndices = indices[1];
	ogl::bind(eqnBuffer, model->getDataPointer(1), 
			eqnIndices, model->getIndexPointer(1));

	GLuint dualEqnBuffer = buffers[3];
	GLuint dualEqnIndices = indices[3];

	ogl::bind(dualEqnBuffer, model->getDataPointer(3),
			dualEqnIndices, model->getIndexPointer(3));
	
	counts[1] = 2*model->getEquationCount();
	counts[3] = model->getEquationCount();
	redraw();
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

	float a = -(right+left)/(right-left);
	float b = -(bottom+top)/(bottom - top);
	float c = -(near + far)/(near-far);
	Matrix4f ret;
	ret << 	2.f/(right - left), 	0, 						0, 					a,
			0,						2.f/(top - bottom), 	0, 					b,
			0,						0, 						1.f/(near - far), 	c,
			0,						0, 						0, 					1;
	return ret;
}


void Display::recreateMatrices(){
	Matrix4f model_left, model_right;
	model_left 		<< 	width,0,0,0,
			   			0,height/2.f,0,0,
			   			0,0,1,0,
			   			0,0,0,1;

	model_right 	<< 	width/4.f,0,0,width/16.f,
						0,height/4.f,0,-height / 4.f,
						0,0,0,0,
						0,0,0,1;


	Vector3f position, up, target;
	position << 0,0,-1;
	up << 0,1,0;
	target << 0,0,1;

	Vector3f rgt = position.cross(up).normalized();


	view << 
		rgt(0), 	rgt(1), 	rgt(2), 	-rgt.dot(position),
		up(0), 		up(1), 		up(2), 		-up.dot(position),
		target(0), 	target(1), 	target(2), 	target.dot(position),
		0, 			0, 			0, 			1;


	
	float left = 	-(float)width/2.f;
	float right = 	(float)width/2.f;

	float bottom = 	-(float)height/2.f;
	float top = 	(float)height/2.f;

	float near = 	-0.5f;
	float far = 	1.f;

	projection 	= calculateProjection(left, right, bottom, top, near, far);

	textProjection = calculateProjection(0.f, width, 0, height, -1.f, 1.f);

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



void Display::setText(string txt){
	state = txt;
	write = true;
}

void Display::renderText(){
	if(write){
		std::cout << "rendering text " << state << std::endl;
	}

	string::const_iterator c = state.begin();

	double x = 1.f;
	double y = -height * .9;
	double scale = 0.25;
	float heightmax = 0;


	Vector4f verts[6];


	text.activate();
	glBindVertexArray(textArray);
	glActiveTexture(GL_TEXTURE0);
	text.setUniform(text_projeciton, (void *)textProjection.data(), 1);
	text.setUniform(text_colour, (void *)textColour.data(), 2);
	
	while(c != state.end() && *c != '\0'){
		
		if(*c == '\n'){
			x = 0.1;
			y += heightmax;
			c += 1;
			continue;
		}
		Character ch = alphabet[*c];

		float xpos = x + ch.bearing(0) * scale;
		float ypos = y - (ch.size(1) - ch.bearing(1)) * scale;

		float w = ch.size(0) * scale;
		float h = ch.size(1) * scale;

		if(h > heightmax){
			heightmax = h *1.1f;
		}

		verts[0] = {xpos, 		ypos + h, 	0.f,		0.f};
		verts[1] = {xpos, 		ypos,		0.f, 	1.f};
		verts[2] = {xpos + w, 	ypos,		1.f, 	1.f};

		verts[3] = {xpos,		ypos + h,	0.f, 	0.f};
		verts[4] = {xpos + w,	ypos,		1.f, 	1.f};
		verts[5] = {xpos + w,	ypos + h,	1.f, 	0.f};

		text.setUniform(text_sampler, &ch.texture, 0);
	
		glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		
	   x += (ch.Advance >> 6) * scale;
	   c += 1;
	}
	write = false;
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}


#endif
