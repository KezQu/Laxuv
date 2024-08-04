#version 460 core

uniform int DistributionShape = -1;

float PI = 3.14159265358979323846264338327950288;

vec4 Line(in uint index, in uint maxParticles){
	return vec4(0.f, index - maxParticles / 2.f, 0.f, 0.f);
}
vec4 Circle(in uint index, in uint maxParticles){
	const float angle = 2.f * PI / maxParticles;
	const float radius =  maxParticles / (2.f * PI);
	return vec4(
		radius * cos(angle * index),
		radius * sin(angle * index),
		0.f,
		0.f);
}
vec4 Square(in uint index, in uint maxParticles){
	float meshSize = pow(maxParticles, 0.5);
	return vec4(
		floor(index / meshSize),
		mod(index, meshSize),
		0.f,
		0.f);
}
vec4 Disk(in uint index, in uint maxParticles){
	vec4 candidatePosition = Square(index, maxParticles);
	if(length(candidatePosition.xyz) > sqrt(maxParticles) / 2.){
		candidatePosition.w = 1.f;
	}
	return candidatePosition;
}
vec4 Qube(in uint index, in uint maxParticles){
	float meshSize = pow(maxParticles, 1 / 3.);
	return vec4(
		floor(index / meshSize), 
		mod(index, meshSize), 
		floor(index / (meshSize * meshSize)),
		0.f);
}
vec4 Sphere(in uint index, in uint maxParticles){
	vec4 candidatePosition = Qube(index, maxParticles);
	if(length(candidatePosition.xyz) > pow(maxParticles, 1. / 3.) / 2.){
		candidatePosition.w = 1.f;
	}
	return candidatePosition;
}

const int UNDEFINED = 0;
const int LINE = 1;
const int CIRCLE = 2;
const int SQUARE = 3;
const int DISK = 4;
const int QUBE = 5;
const int SPHERE = 6;

vec4 InitDefaultShape(in uint index, in uint maxParticles){
	vec4 positionCandidate = vec4(0);
	switch (DistributionShape){
	case LINE:
		positionCandidate = Line(index, maxParticles);
		break;
	case CIRCLE:
		positionCandidate = Circle(index, maxParticles);
		break;
	case SQUARE:
		positionCandidate = Square(index, maxParticles);
		break;
	case DISK:
		positionCandidate = Disk(index, maxParticles);
		break;
	case QUBE:
		positionCandidate = Qube(index, maxParticles);
		break;
	case SPHERE:
		positionCandidate = Sphere(index, maxParticles);
		break;
	default:
		break;
	}
	return positionCandidate * vec4(0.1, 0.1, 0.1, 1);
}