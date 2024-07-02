#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 3) out vec3 outCoordOffset;


vec3 CalculateOffset(in int instanceID);

void main(){
	outColor = inColor / 255.;
	outCoordOffset = CalculateOffset(gl_InstanceID);
	gl_Position = vec4(inPosition, 1.0);
}