#version 460 core

layout(location = 0) in vec4 inColorFrag;
layout(location = 1) in vec3 normal;

out vec4 BaseColor;

uniform vec3 ambientLightColor = vec3(1.0);
uniform vec3 diffuseLightDirection = vec3(1.0);
uniform vec3 diffuseLightColor = vec3(0.0);

void main(){
	if(gl_FrontFacing){
		float diffuseLightStrength = 0;
		if(length(diffuseLightDirection) != 0){
			diffuseLightStrength = max(dot(normalize(diffuseLightDirection), normal), 0.0);
		}
		BaseColor = (diffuseLightStrength * vec4(diffuseLightColor, 1.0) + vec4(ambientLightColor, 1.0)) * inColorFrag;
	}
	else{
		BaseColor = vec4(0,0,0,1);
	}
}
 