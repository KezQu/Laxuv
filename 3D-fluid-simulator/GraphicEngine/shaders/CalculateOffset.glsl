#version 460 core

uniform int distributionFunctionType;
uniform float objectSize;
uniform int maxParticles;

vec3 plane(in int instanceID){
	int meshSize = int(ceil(pow(maxParticles, 0.5)));
	return vec3(int(instanceID / meshSize), 0, instanceID % meshSize) * objectSize;
}
vec3 line(in int instanceID){
	return vec3(instanceID, 0, 0) * objectSize;
}
vec3 qube(in int instanceID){
	int meshSize = int(ceil(pow(maxParticles, 0.33333)));
	return vec3(int(instanceID / meshSize) % meshSize, int(instanceID / (meshSize * meshSize)), instanceID % meshSize) * objectSize;
}

vec3 CalculateOffset(in int instanceID){
	switch (distributionFunctionType){
		case 0:
		return plane(instanceID);
		case 1:
		return line(instanceID);
		case 2:
		return qube(instanceID);

	}
}