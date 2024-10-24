#version 460 core

const int vertexPrimitiveCount = 1;
layout(points) in;
layout(location = 0) in vec4 inColor[];
layout(location = 3) in vec3 inShapeOffset[];

layout(points, max_vertices = vertexPrimitiveCount) out;
layout(location = 0) out vec4 outColor; 
layout(location = 1) out vec3 normal;

vec4 CalculateNDC(in vec3 position, in vec3 offset);
float PointSize(in vec4 position);

void main(){
	vec3 primitiveVertices[vertexPrimitiveCount];
	for(int i = 0; i < vertexPrimitiveCount; i++){
		primitiveVertices[i] = gl_in[i].gl_Position.xyz;
	} 
	normal = -normalize(primitiveVertices[0]);
	for(int i = 0; i < vertexPrimitiveCount; i++){
		outColor = inColor[i]; 
		gl_Position = CalculateNDC(gl_in[i].gl_Position.xyz, inShapeOffset[i]);
//		gl_PointSize = PointSize(gl_Position) * shapeRadius;
		gl_PointSize = 10;
		EmitVertex();
	}
	EndPrimitive();
}