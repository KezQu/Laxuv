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

vec3 CalculateNormal(in vec3 position){
	const mat4 translate_mat = mat4(1,0,0,model[3].x,
									0,1,0,model[3].y,
									0,0,1,model[3].z,
									0,0,0,		   1);
	vec4 normal = transpose(transpose(model) * inverse(translate_mat)) * vec4(position, 1);
	return -normalize(normal.xyz);
}

float PointSize(in vec4 position){
	return float(viewport.y * projection[1][1]) / position.w;
}