#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 normal;

struct PhysicsProperties {
	vec4 forceMass;
	vec4 velocity;
	vec4 position;
};

layout(std140, binding = 0) buffer dataBuffer{
	PhysicsProperties particle[];
};

vec4 CalculateNDC(in vec3 position, in vec4 offset);

void main(){
	outColor = inColor / 255.;
	normal = -inPosition;
	gl_Position = CalculateNDC(inPosition, particle[gl_InstanceID].position);
	gl_PointSize = 10;
}