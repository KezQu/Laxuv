#version 460 core

vec3 CalculateNormal(in vec3 primitiveVertices[3]){
	vec3 v1 = primitiveVertices[0] - primitiveVertices[1];
	vec3 v2 = primitiveVertices[1] - primitiveVertices[2];
	return normalize(cross(v1, v2));
}
