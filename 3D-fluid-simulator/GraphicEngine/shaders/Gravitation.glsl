#version 460 core

const vec3 g = vec3(0, -9.80665, 0);

vec4 ApplyGravitation(in vec4 particleForceMass, in float dt){
	return vec4(g * particleForceMass.w, particleForceMass.w);
}