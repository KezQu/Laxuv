#version 460 core

const int vertexPrimitiveCount = 2;
layout(lines) in;
layout(location = 0) in vec4 inColor[];

layout(line_strip, max_vertices = vertexPrimitiveCount) out;
layout(location = 0) out vec4 outColor; 
layout(location = 1) out vec3 normal;

uniform int shapeRadius;

vec4 CalculateNDC(in vec3 position);

void main(){
	vec3 primitiveVertices[vertexPrimitiveCount];
	for(int i = 0; i < vertexPrimitiveCount; i++){
		primitiveVertices[i] = gl_in[i].gl_Position.xyz;
	}
	normal = normalize(primitiveVertices[1] - primitiveVertices[0]);
	for(int i = 0; i < vertexPrimitiveCount; i++){
		outColor = inColor[i];
		gl_Position = CalculateNDC((gl_in[i].gl_Position.xyz) * shapeRadius);
		EmitVertex();
	}
	EndPrimitive();
}