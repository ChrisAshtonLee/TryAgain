#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 projView;

void main(){
	gl_Position = projView*vec4(pos, 1.0);

}