#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColorVert;

layout(location = 0) out vec4 outColorVert;
layout(location = 1) out vec3 normal;

const uint MaxNeighbours =  512;

struct ParticleProperties {
	vec4 forceMass;
	vec4 velocity;
	vec4 position;	
	vec4 VolumeDensityPressureMass;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	ParticleProperties particle[];
};

vec4 CalculateNDC(in vec3 position, in vec4 offset);

void main(){
	outColorVert = inColorVert / 255.;
	normal = -inPosition;
	gl_Position = CalculateNDC(inPosition, particle[gl_InstanceID].position);
	gl_PointSize = 10;
}