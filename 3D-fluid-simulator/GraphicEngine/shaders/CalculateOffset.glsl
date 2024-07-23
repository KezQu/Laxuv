#version 460 core

uniform int distributionFunctionType = -1;
uniform int maxParticles = 1;

float PI = 3.14159265359;
float GOLDEN_RATIO = (1 + sqrt(5.)) / 2.;

vec3 plane(in int instanceID){
	int meshSize = int(ceil(pow(maxParticles, 0.5)));
	return vec3(instanceID / meshSize, 0, mod(instanceID, meshSize));
}
vec3 line(in int instanceID){
	return vec3(instanceID, 0, 0);
}
vec3 qube(in int instanceID){
	int meshSize = int(ceil(pow(maxParticles, 1 / 3.)));
	return vec3(mod(instanceID / meshSize, meshSize), (instanceID / (meshSize * meshSize)), mod(instanceID, meshSize));
}
vec3 sphere(in int instanceID){
	float meshSize = 1.33333 * 3.1415 * pow(maxParticles, 0.33333);
	float theta = 2 * PI * instanceID / GOLDEN_RATIO;
	float phi = acos(1 - float(2 * instanceID) / maxParticles);

	float scale = ceil(pow(instanceID, 0.33333) / cos(theta));
	return vec3(scale * cos(theta) * sin(phi), scale * sin(theta) * sin(phi), scale * cos(phi));
}
vec3 disk(in int instanceID){
	float meshSize = 2 * pow(maxParticles, 0.5) / PI;
	float radius = floor(meshSize * sqrt(instanceID / float(maxParticles - 1)));
	float phi = 2 * PI * mod(instanceID / 1.415, 1.);
	return vec3(radius * cos(phi), 0, radius * sin(phi));
}
vec3 CalculateOffset(in int instanceID){
	vec3 result = vec3(0);
	switch (distributionFunctionType){
	case 0:
		result = plane(instanceID);
		break;
	case 1:
		result = line(instanceID);
		break;
	case 2:
		result = qube(instanceID);
		break;
	case 3:
		result = sphere(instanceID);
		break;
	case 4:
		result = disk(instanceID);
		break;
	}
	return result;
}