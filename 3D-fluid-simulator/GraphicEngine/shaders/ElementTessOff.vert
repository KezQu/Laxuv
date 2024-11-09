#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColorVert;

layout(location = 0) out vec4 outColorVert;
layout(location = 1) out vec3 normal;

const uint MaxNeighbours =  512;
const uint NONE = 0;

uniform uint colorType;

struct ParticleProperties {
	vec4 velocityDFSPHfactor;
	vec4 position;
	vec4 VolumeDensityPressureDro_Dt;
  	vec4 color;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	ParticleProperties particle[];
};

vec4 CalculateNDC(in vec3 position, in vec3 offset);
vec3 CalculateNormal(in vec3 position);

void main(){
	if(colorType == NONE){
		outColorVert = inColorVert / 255.;
	}
	else{
		outColorVert = particle[gl_InstanceID].color;
	}
	normal = CalculateNormal(inPosition);
	gl_Position = CalculateNDC(inPosition, particle[gl_InstanceID].position.xyz);
	gl_PointSize = 10;
}