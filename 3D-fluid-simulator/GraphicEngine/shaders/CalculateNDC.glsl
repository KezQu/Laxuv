#version 460 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 viewport;
uniform float shapeRadius;

vec4 CalculateNDC(in vec3 position, in vec4 offset){
	vec4 outPosition = vec4(position * shapeRadius + offset.xyz, 1.0);
	outPosition = model * outPosition;
	outPosition = view * outPosition;
	outPosition = projection * outPosition;
	return outPosition;
}

float PointSize(in vec4 position){
	return float(viewport.y * projection[1][1]) / position.w;
}