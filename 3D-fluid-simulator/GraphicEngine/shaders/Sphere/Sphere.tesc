#version 460

layout(location = 0) in vec4 inColor[];

layout(vertices = 3) out;
layout(location = 0) out vec4 outColor[];

uniform int subdivision;

void main(){
	outColor[gl_InvocationID] = inColor[gl_InvocationID];
	gl_TessLevelOuter[gl_InvocationID] = subdivision;
	gl_TessLevelInner[0] = subdivision;
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	barrier();
}