#version 460 core
#extension GL_NV_gpu_shader5 : enable

uniform int DistributionShape = -1;

const float PI = 3.14159265358979323846264338327950288;

vec4 Line(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
	return vec4(0.f, idx, 0.f, 0.f);
}
vec4 Circle(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
	const float angle = 2.f * PI / maxParticles;
	const float radius =  maxParticles / (2.f * PI);
	return vec4(
		radius * cos(angle * idx),
		radius * sin(angle * idx),
		0.f,
		0.f);
}
vec4 Square(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
	float meshSize = pow(maxParticles, 0.5);
	return vec4(
		floor(idx / meshSize),
		mod(idx, meshSize),
		0.f,
		0.f);
}
vec4 Disk(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
	vec4 candidatePosition = Square(idx, invocation, maxParticles);
	if(length(candidatePosition.xyz) > sqrt(maxParticles) / 2.){
		candidatePosition.w = 1.f;
	}
	return candidatePosition;
}
vec4 Qube(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
	return vec4(
		invocation.x, 
		invocation.y,
		invocation.z,
		0.f);
}
vec4 Sphere(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
	vec4 candidatePosition = Qube(idx, invocation, maxParticles);
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

vec4 InitDefaultShape(in uint32_t idx, in uvec3 invocation, in uint32_t maxParticles){
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
	case QUBE:
		positionCandidate = Qube(idx, invocation, maxParticles);
		break;
	case SPHERE:
		positionCandidate = Sphere(idx, invocation, maxParticles);
		break;
	default:
		break;
	}
	return positionCandidate;
}