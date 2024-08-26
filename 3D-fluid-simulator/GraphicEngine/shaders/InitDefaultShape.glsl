#version 460 core

uniform int DistributionShape = -1;

float PI = 3.14159265358979323846264338327950288;

vec4 Line(in uint idx, in uint maxParticles){
	return vec4(0.f, idx, 0.f, 0.f);
}
vec4 Circle(in uint idx, in uint maxParticles){
	const float angle = 2.f * PI / maxParticles;
	const float radius =  maxParticles / (2.f * PI);
	return vec4(
		radius * cos(angle * idx),
		radius * sin(angle * idx),
		0.f,
		0.f);
}
vec4 Square(in uint idx, in uint maxParticles){
	float meshSize = pow(maxParticles, 0.5);
	return vec4(
		floor(idx / meshSize),
		mod(idx, meshSize),
		0.f,
		0.f);
}
vec4 Disk(in uint idx, in uint maxParticles){
	vec4 candidatePosition = Square(idx, maxParticles);
	if(length(candidatePosition.xyz) > sqrt(maxParticles) / 2.){
		candidatePosition.w = 1.f;
	}
	return candidatePosition;
}
vec4 Qube(in uint idx, in uint maxParticles){
	float meshSize = pow(maxParticles, 1 / 3.);
	return vec4(
		mod(floor(idx / meshSize), meshSize), 
		mod(idx, meshSize), 
		floor(idx / (meshSize * meshSize)),
		0.f);
}
vec4 Sphere(in uint idx, in uint maxParticles){
	vec4 candidatePosition = Qube(idx, maxParticles);
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

vec4 InitDefaultShape(in uint idx, in uint maxParticles){
	vec4 positionCandidate = vec4(0);
	switch (DistributionShape){
	case LINE:
		positionCandidate = Line(idx, maxParticles);
		break;
	case CIRCLE:
		positionCandidate = Circle(idx, maxParticles);
		break;
	case SQUARE:
		positionCandidate = Square(idx, maxParticles);
		break;
	case DISK:
		positionCandidate = Disk(idx, maxParticles);
		break;
	case QUBE:
		positionCandidate = Qube(idx, maxParticles);
		break;
	case SPHERE:
		positionCandidate = Sphere(idx, maxParticles);
		break;
	default:
		break;
	}
	return positionCandidate;// * vec4(0.1, 0.1, 0.1, 1);
}