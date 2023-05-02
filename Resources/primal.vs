#version 430

layout (location = 0) in vec2 pos;
uniform mat4 mvp;

out vec4 position;
out vec4 frag_colour;


void main(){
	vec4 temp = mvp * vec4(pos, 1, 1);
	position = mvp * temp;
	frag_colour = vec4(temp.xyz, 1);
}
