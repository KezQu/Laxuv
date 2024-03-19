#version 460 core

layout(location = 0) in vec4 inColor;

out vec4 FragColor;

void main(){
	FragColor = inColor;
}
