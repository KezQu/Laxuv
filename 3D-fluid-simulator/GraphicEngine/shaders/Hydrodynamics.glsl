#version 460 core

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const float R = 8.31446261815324;
const mat3 I = mat3(1, 0, 0,
        0, 1, 0,
        0, 0, 1);

const uint MaxNeighbours = 16;
const uint MaxValueNeighbour = uint(0xffff);

uniform uint influenceKernel = 4;
uniform float dt;
uniform float gamma = 1.4f;
uniform float inflKernelSmoother;

struct stateVector{
	float x;
	vec3 y;
	float z;
};

struct Flux {
	vec3 x;
	mat3 y;
	vec3 z;
};

struct ParticleProperties {
	vec4 velocity;
	vec4 positionGroup;
	vec4 VolumeDensityPressureMass;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	ParticleProperties particle[];
};

float CalculateKernelWeight(vec3 points_dist){
	float abs_dist = length(points_dist);
	return pow(E, - pow(abs_dist, 2) / inflKernelSmoother);
}

float CalculateDerivKernelWeight(vec3 points_dist){
	float abs_dist = length(points_dist);
	return - 2 * abs_dist / inflKernelSmoother * calculateKernelWeight(points_dist);
}

void FindNeighbours(uint index_x, uint MaxParticles){
	vec3 position_x = particle[index_x].positionGroup.xyz;
	uint num_neighbours = 0;
	for(uint i = 0; i < MaxParticles && num_neighbours < MaxNeighbours; i++){
		if(i !=index_x){
			vec3 pos_x = particle[index_x].positionGroup.xyz;
			vec3 pos_neighbour= particle[i].positionGroup.xyz;
			if(CalculateKernelWeight(pos_x - pos_neighbour) > 0){
				particle[index_x].neighbours[num_neighbours] = i;
				num_neighbours++;
			}
		}
	}
}

stateVector CalculateEulerEquations(uint index_x){
	float mass_x = particle[index_x].VolumeDensityPressureMass.w;
	float density_x = particle[index_x].VolumeDensityPressureMass.y;
	vec3 velocity_x = particle[index_x].velocity.xyz;
	float energy_x = mass_x * length(velocity_x) / 2.;
	stateVector U_n = stateVector(density_x, velocity_x, energy_x);

}