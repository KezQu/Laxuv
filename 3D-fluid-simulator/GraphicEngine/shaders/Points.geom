#version 460 core

const int vertexPrimitiveCount = 1;
layout(points) in;
layout(location = 0) in vec4 inColor[];
layout(location = 3) in vec3 outCoordOffset[];

layout(points, max_vertices = vertexPrimitiveCount) out;
layout(location = 0) out vec4 outColor; 
layout(location = 1) out vec3 normal;

uniform int shapeRadius;

vec4 CalculateNDC(in vec3 position);
float PointSize(in vec4 position);

void main(){
	vec3 primitiveVertices[vertexPrimitiveCount];
	for(int i = 0; i < vertexPrimitiveCount; i++){
		primitiveVertices[i] = gl_in[i].gl_Position.xyz + outCoordOffset[0];
	} 
	normal = -normalize(primitiveVertices[0]);
	for(int i = 0; i < vertexPrimitiveCount; i++){
		outColor = inColor[i]; 
		gl_Position = CalculateNDC((gl_in[i].gl_Position.xyz + outCoordOffset[0] * 2) * shapeRadius);
		gl_PointSize = PointSize(gl_Position) * shapeRadius;
		EmitVertex();
	}
	EndPrimitive();
}