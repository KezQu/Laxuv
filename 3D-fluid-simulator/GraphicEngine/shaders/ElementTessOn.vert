#version 460 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColorVert;

layout(location = 0) out vec4 outColorVert;
layout(location = 3) out vec3 outShapeOffset;

const uint MaxNeighbours =  512;
const uint NONE = 0;

uniform uint colorType;

struct ParticleProperties {
	vec4 velocityDFSPHfactor;
	vec4 position;
	vec4 VolumeDensityPressureDro_Dt;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	ParticleProperties particle[];
};

vec4 ChooseColor(ParticleProperties properties);

void main(){
	if(colorType == NONE){
		outColorVert = inColorVert / 255.;
	}
	else{
		outColorVert = ChooseColor(particle[gl_InstanceID]);
	}
	outShapeOffset = particle[gl_InstanceID].position.xyz;
	gl_Position = vec4(inPosition, 0);
	gl_PointSize = 10;
}