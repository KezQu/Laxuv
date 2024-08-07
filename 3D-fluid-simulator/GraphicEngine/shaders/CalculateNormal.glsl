#version 460 core

vec3 CalculateNormal(in vec4 primitiveVertices[3]){
	vec3 v1 = primitiveVertices[0].xyz - primitiveVertices[1].xyz;
	vec3 v2 = primitiveVertices[1].xyz - primitiveVertices[2].xyz;
	return normalize(cross(v1, v2));
}
