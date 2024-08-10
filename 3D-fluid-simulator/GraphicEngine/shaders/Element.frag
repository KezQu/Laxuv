#version 460 core

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec3 normal;

out vec4 BaseColor;

uniform vec3 ambientLightColor;
uniform vec3 diffuseLightDirection;
uniform vec3 diffuseLightColor;

void main(){
	if(gl_FrontFacing){
		float diffuseLightStrength = 0;
		if(length(diffuseLightDirection) != 0){
			diffuseLightStrength = max(dot(normalize(diffuseLightDirection), normal), 0.0);
		}
		BaseColor = (diffuseLightStrength * vec4(diffuseLightColor, 1.0) + vec4(ambientLightColor, 1.0)) * inColor;
	}
	else{
		BaseColor = vec4(0,0,0,1);
	}
}
 