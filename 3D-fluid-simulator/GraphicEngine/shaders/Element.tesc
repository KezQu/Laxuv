#version 460 core

layout(location = 0) in vec4 inColor[];
layout(location = 3) in vec3 inCoordOffset[];

layout(vertices = 3) out;
layout(location = 0) out vec4 outColor[];
layout(location = 3) out vec3 outCoordOffset[];

uniform int subdivision;

void main(){
	outColor[gl_InvocationID] = inColor[gl_InvocationID];
	outCoordOffset[gl_InvocationID] = inCoordOffset[gl_InvocationID];
	gl_TessLevelOuter[gl_InvocationID] = subdivision;
	gl_TessLevelInner[0] = subdivision;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	barrier();
}