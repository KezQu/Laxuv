#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) out vec4 outColor;

vec4 CalculateNDC(in mat4 model, in mat4 view, in mat4 projection, in vec3 position);

void main(){
	outColor = inColor / 255.;
	gl_Position = CalculateNDC(model, view, projection, inPosition);
}