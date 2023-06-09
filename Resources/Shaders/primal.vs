#version 430

layout (location = 0) in vec2 pos;
uniform mat4 mvp;
uniform vec4 colour;

out vec4 position;
out vec4 frag_colour;


void main(){
	
	gl_Position = mvp * vec4(pos, 0, 1);
	frag_colour = colour;
}
