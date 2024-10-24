#version 460 core

//uniform mat4 translate;
//uniform mat4 rotate;
//uniform mat4 scale;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 viewport;

vec4 CalculateNDC(in vec3 position, in vec3 offset){
	vec4 outPosition = vec4(position + offset, 1.0);
//	mat4 apended_translate = translate * mat4(vec4(0), vec4(0), vec4(0), vec4(offset, 0));

	outPosition = projection * view * model * outPosition;
	
	return outPosition;
}

vec4 InverseNDC(in vec3 position, in vec3 offset){
	vec4 outPosition = vec4(position + offset, 1.0);
//	mat4 apended_translate = translate * mat4(vec4(0), vec4(0), vec4(0), vec4(-offset, 0));
	
	outPosition = inverse(projection * view * model) * outPosition;
	
	return outPosition;
}


float PointSize(in vec4 position){
	return float(viewport.y * projection[1][1]) / position.w;
}