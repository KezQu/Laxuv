#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

vec4 CalculateNDC(in vec3 position);

void main(){
	outColor = inColor / 255.;
	gl_Position = CalculateNDC(inPosition);
}