#version 460

layout(triangles, equal_spacing, cw) in;
layout(location = 0) in vec4 inColor[];

uniform vec3 center;

layout(location = 0) out vec4 outColor[3];

vec4 CalculateNDC(in vec3 position);

void main(){
	vec4 tessVertexCoord = vec4(0.0);
	for(int i = 0; i < gl_PatchVerticesIn; i++){
		tessVertexCoord += gl_TessCoord[i] * gl_in[i].gl_Position;
		outColor[i] = inColor[i];
	}
	float R = distance(gl_in[0].gl_Position.xyz, center);
	float tessVertexCenterLength = distance(tessVertexCoord.xyz, center);
	tessVertexCoord.xyz = tessVertexCoord.xyz + (R - tessVertexCenterLength) * normalize(tessVertexCoord.xyz - center);
	gl_Position = CalculateNDC(tessVertexCoord.xyz);
}