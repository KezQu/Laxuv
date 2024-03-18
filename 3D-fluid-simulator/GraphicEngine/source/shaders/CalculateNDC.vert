#version 460 core

vec4 CalculateNDC(in mat4 model, in mat4 view, in mat4 projection, in vec3 position){
	vec4 outPosition = vec4(position, 1.0);
	outPosition = model * outPosition;
	outPosition = view * outPosition;
	outPosition = projection * outPosition;
	return outPosition;
}