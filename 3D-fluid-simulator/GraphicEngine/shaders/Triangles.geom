#version 460 core

const int vertexPrimitiveCount = 3;
layout(triangles) in;
layout(location = 0) in vec4 inColor[];
layout(location = 3) in vec4 inShapeOffset[];

layout(triangle_strip, max_vertices = 24) out;
layout(location = 0) out vec4 outColor; 
layout(location = 1) out vec3 normal;

vec3 CalculateNormal(in vec4 primitiveVertices[3]);
vec4 CalculateNDC(in vec3 position);

void main(){
	vec4 primitiveVertices[vertexPrimitiveCount];
	for(int i = 0; i < vertexPrimitiveCount; i++){
		primitiveVertices[i] = CalculateNDC(gl_in[i].gl_Position.xyz + inShapeOffset[i].xyz);
	}
	normal = CalculateNormal(primitiveVertices);
	
	for(int i = 0; i < vertexPrimitiveCount; i++){
		gl_Position = primitiveVertices[i];
		outColor = inColor[i];
		EmitVertex();
	}
	EndPrimitive();
}