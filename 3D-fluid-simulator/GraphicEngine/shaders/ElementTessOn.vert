#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColorVert;

layout(location = 0) out vec4 outColorVert;
layout(location = 3) out vec4 outShapeOffset;

const uint MaxNeighbours =  16;

struct ParticleProperties {
	vec4 velocity;
	vec4 positionGroup;
	vec4 VolumeDensityPressureMass;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	ParticleProperties particle[];
};

void main(){
	outColorVert = inColorVert / 255.;
	outShapeOffset = particle[gl_InstanceID].positionGroup;
	gl_Position = vec4(inPosition, 0);
	gl_PointSize = 10;
}