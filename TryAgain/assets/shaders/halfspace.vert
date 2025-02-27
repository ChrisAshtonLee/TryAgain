#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 fragPos;

uniform mat4 projView;

void main(){
	fragPos = color;
	gl_Position = projView*vec4(pos,1.0);

}