#version 460 core

uniform mat4 model;
uniform vec3 scale;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 viewport;
uniform float shapeRadius;

vec4 CalculateNDC(in vec3 position, in vec3 offset){
	vec4 outPosition = vec4((position + offset) * scale * shapeRadius, 1.0);

	outPosition = projection * view * model * outPosition;
	
	return outPosition;
}

float PointSize(in vec4 position){
	return float(viewport.y * projection[1][1]) / position.w;
}