#version 460 core

layout(triangles, equal_spacing, ccw) in;
layout(location = 0) in vec4 inColorTese[];
layout(location = 3) in vec4 inShapeOffset[];


layout(location = 0) out vec4 outColorTese;
layout(location = 1) out vec3 normal;

uniform vec3 center;
vec4 CalculateNDC(in vec3 position, in vec4 offset);

void main(){
	vec3 tessVertexCoord = vec3(0.0);
	outColorTese = inColorTese[0];
	for(int i = 0; i < gl_PatchVerticesIn; i++){
		tessVertexCoord += gl_TessCoord[i] * gl_in[i].gl_Position.xyz;
	}
	float R = distance(gl_in[0].gl_Position.xyz, center);
	float tessVertexCenterLength = distance(tessVertexCoord, center);
	tessVertexCoord = tessVertexCoord + (R - tessVertexCenterLength) * normalize(tessVertexCoord - center);
	normal = -tessVertexCoord;
	gl_Position = CalculateNDC(tessVertexCoord, inShapeOffset[0]);
}