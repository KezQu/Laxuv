#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 testcolor;

out vec4 color;

void main(){
	color = testcolor / 255.;
	gl_Position = vec4(position.x, position.y, position.z, 1.0);
}