#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 3) out vec4 outShapeOffset;

const uint KernelRadius = 1;
const uint MaxNeighbours = KernelRadius * KernelRadius * 4 * 4 * 2;

struct PhysicsProperties {
	vec4 forceMass;
	vec4 velocity;
	vec4 position;
	vec4 VolumeDensityPressureMass;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	PhysicsProperties particle[];
};

void main(){
	outColor = inColor / 255.;
	outShapeOffset = particle[gl_InstanceID].position;
	gl_Position = vec4(inPosition, 0);
	gl_PointSize = 10;
}