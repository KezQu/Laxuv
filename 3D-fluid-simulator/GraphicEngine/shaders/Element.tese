#version 460 core

layout(triangles, equal_spacing, ccw) in;
layout(location = 0) in vec4 inColor[];
layout(location = 3) in vec4 inShapeOffset[];

layout(location = 0) out vec4 outColor[3];
layout(location = 3) out vec4 outShapeOffset[3];

uniform vec3 center;
vec4 CalculateNDC(in vec3 position);

void main(){
	vec3 tessVertexCoord = vec3(0.0);
	for(int i = 0; i < gl_PatchVerticesIn; i++){
		tessVertexCoord += gl_TessCoord[i] * gl_in[i].gl_Position.xyz;
		outColor[i] = inColor[i];
		outShapeOffset[i] = inShapeOffset[i];
	}
	float R = distance(gl_in[0].gl_Position.xyz, center);
	float tessVertexCenterLength = distance(tessVertexCoord, center);
	tessVertexCoord = tessVertexCoord + (R - tessVertexCenterLength) * normalize(tessVertexCoord - center);
	gl_Position = CalculateNDC(tessVertexCoord);
}