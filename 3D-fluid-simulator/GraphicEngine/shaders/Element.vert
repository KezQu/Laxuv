#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;

struct PhysicsProperties {
	vec4 forceMass;
	vec4 velocity;
	vec4 position;
};

layout(std140, binding = 0) buffer dataBuffer{
	PhysicsProperties particle[];
};

void main(){
	outColor = inColor / 255.;
	gl_Position = vec4(inPosition + particle[gl_InstanceID].position.xyz, 1.0);
}