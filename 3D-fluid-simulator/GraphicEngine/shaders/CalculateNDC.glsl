#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec4 CalculateNDC(in vec3 position){
	vec4 outPosition = vec4(position, 1.0);
	outPosition = model * outPosition;
	outPosition = view * outPosition;
	outPosition = projection * outPosition;
	return outPosition;
}