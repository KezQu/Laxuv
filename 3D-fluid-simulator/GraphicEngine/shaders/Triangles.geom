#version 460 core

const int vertexPrimitiveCount = 3;
layout(triangles) in;
layout(location = 0) in vec4 inColor[];
layout(location = 3) in vec3 outCoordOffset[];

layout(triangle_strip, max_vertices = 24) out;
layout(location = 0) out vec4 outColor; 
layout(location = 1) out vec3 normal;

vec3 CalculateNormal(in vec3 primitiveVertices[3]);
vec4 CalculateNDC(in vec3 position);

void main(){
	vec3 primitiveVertices[vertexPrimitiveCount];
	for(int i = 0; i < vertexPrimitiveCount; i++){
		primitiveVertices[i] = gl_in[i].gl_Position.xyz + outCoordOffset[0];
	}
	normal = CalculateNormal(primitiveVertices);
	
//	for(int a = 0; a < 2; a++){
//		for(int b = 0; b < 2; b++){
//			for(int c = 0; c < 2; c++){
				for(int i = 0; i < vertexPrimitiveCount; i++){
					gl_Position = CalculateNDC(gl_in[i].gl_Position.xyz + outCoordOffset[0]); // + vec3(a,b,c) * 10
					outColor = inColor[i];
					EmitVertex();
				}
				EndPrimitive();
// 	}}}
}