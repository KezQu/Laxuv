#version 460 core

uniform uint DistributionShape = 0;

const float PI = 3.14159265358979323846264338327950288;

vec4 Line(in uint idx, in uvec3 invocation, in uint maxParticles){
	return vec4(0.f, idx, 0.f, 0.f);
}
vec4 Circle(in uint idx, in uvec3 invocation, in uint maxParticles){
	const float angle = 2.f * PI / maxParticles;
	const float radius =  maxParticles / (2.f * PI);
	return vec4(
		radius * cos(angle * idx),
		radius * sin(angle * idx),
		0.f,
		0.f);
}
vec4 Square(in uint idx, in uvec3 invocation, in uint maxParticles){
	float meshSize = pow(maxParticles, 0.5);
	return vec4(
		floor(idx / meshSize),
		mod(idx, meshSize),
		0.f,
		0.f);
}
vec4 Disk(in uint idx, in uvec3 invocation, in uint maxParticles){
	vec4 candidatePosition = Square(idx, invocation, maxParticles);
	if(length(candidatePosition.xyz) > sqrt(maxParticles) / 2.){
		candidatePosition.w = 1.f;
	}
	return candidatePosition;
}
vec4 Cube(in uint idx, in uvec3 invocation, in uint maxParticles){
	return vec4(
		invocation.x, 
		invocation.y,
		invocation.z,
		0.f);
}
vec4 Sphere(in uint idx, in uvec3 invocation, in uint maxParticles){
	vec4 candidatePosition = Cube(idx, invocation, maxParticles);
	if(length(candidatePosition.xyz) > pow(maxParticles, 1. / 3.) / 2.){
		candidatePosition.w = 1.f;
	}
	return candidatePosition;
}

const uint UNDEFINED = 0;
const uint LINE = 1;
const uint CIRCLE = 2;
const uint SQUARE = 3;
const uint DISK = 4;
const uint CUBE = 5;
const uint SPHERE = 6;

vec4 InitDefaultShape(in uint idx, in uvec3 invocation, in uint maxParticles){
	vec4 positionCandidate = vec4(0);
	switch (DistributionShape){
	case LINE:
		positionCandidate = Line(idx, invocation, maxParticles);
		break;
	case CIRCLE:
		positionCandidate = Circle(idx, invocation, maxParticles);
		break;
	case SQUARE:
		positionCandidate = Square(idx, invocation, maxParticles);
		break;
	case DISK:
		positionCandidate = Disk(idx, invocation, maxParticles);
		break;
	case CUBE:
		positionCandidate = Cube(idx, invocation, maxParticles);
		break;
	case SPHERE:
		positionCandidate = Sphere(idx, invocation, maxParticles);
		break;
	default:
		break;
	}
	return positionCandidate;
}