#version 460 core

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const float R = 8.31446261815324;
const mat3 I = mat3(1, 0, 0,
        0, 1, 0,
        0, 0, 1);

const uint MaxNeighbours = 512;
const uint MaxValueNeighbour = uint(0xffff);

uniform float kernel_a;
uniform float influenceKernel;
uniform float searchKernel;
uniform float dt;
uniform float mass;
uniform float viscosityFactor;
uniform float density0;

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
	vec4 velocityDFSPHfactor;
	vec4 positionGroup;
	vec4 VolumeDensityPressureRohash;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	ParticleProperties particle[];
};

float CalculateKernelWeight(vec3 points_dist){
	const float abs_dist = length(points_dist);
	
	return pow(E, - pow(abs_dist, kernel_a) / influenceKernel);
}

vec3 CalculateGradKernelWeight(vec3 points_dist){
	const float abs_dist = length(points_dist);
	const vec3 versor_ij = abs_dist > 0 ? normalize(points_dist): vec3(0);
	
	return -kernel_a * pow(abs_dist, kernel_a - 1) * CalculateKernelWeight(points_dist) * versor_ij;
}

void FindNeighbours(uint index_x, uint MaxParticles){
	for(uint i = 0; i < MaxNeighbours; i++){
			particle[index_x].neighbours[i] = MaxValueNeighbour;
	}
	vec3 pos_x = particle[index_x].positionGroup.xyz;
	if(!isnan(pos_x.x)){
		uint num_neighbours = 0;
		for(uint i = 0; i < MaxParticles && num_neighbours < MaxNeighbours; i++){
	//		if(i != index_x){
				const vec3 pos_neighbour = particle[i].positionGroup.xyz;
				if(length(pos_x - pos_neighbour) < 2 * searchKernel){
					particle[index_x].neighbours[num_neighbours] = i;
					num_neighbours++;
				}
	//		}
		}
	}
}

float CalculateDFSPHFactor(uint index_x){
	vec3 sum = vec3(0);
	float sum_of_squared = 0.f;
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float m_j = mass;
		const vec3 x_i = particle[index_x].positionGroup.xyz;
		const vec3 x_j = particle[curr_neighbour].positionGroup.xyz;
		const vec3 gradW_ij = CalculateGradKernelWeight(x_i - x_j);
		sum += m_j * gradW_ij;
		sum_of_squared += pow(length(m_j * gradW_ij), 2);
	}
	return pow(particle[index_x].VolumeDensityPressureRohash.y, 2) / (pow(length(sum), 2) + sum_of_squared + 1e-6);
}

float CalculateDensity(uint index_x){
	float kernel_sum = 0;
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float m_j = mass;
		const vec3 x_ij = particle[index_x].positionGroup.xyz - particle[curr_neighbour].positionGroup.xyz;
		kernel_sum += m_j * CalculateKernelWeight(x_ij);
	}
	return kernel_sum;
}

float CalculateDerivDensity(uint index_x){
	const vec3 v_i = particle[index_x].velocityDFSPHfactor.xyz;
	
	float kernel_sum = 0.f;
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float m_j = mass;
		const vec3 v_j = particle[curr_neighbour].velocityDFSPHfactor.xyz;
		const vec3 x_ij = particle[index_x].positionGroup.xyz - particle[curr_neighbour].positionGroup.xyz;

		kernel_sum += m_j * dot((v_i - v_j), CalculateGradKernelWeight(x_ij));
	}
	return kernel_sum;
}

vec3 CalculateGradPressure(uint index_x){
	const float ro_i = particle[index_x].VolumeDensityPressureRohash.y;
	const float p_i = particle[index_x].VolumeDensityPressureRohash.z;
	
	vec3 kernel_sum = vec3(0);
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float m_j = mass;
		const float ro_j = particle[curr_neighbour].VolumeDensityPressureRohash.y;
		const float p_j = particle[curr_neighbour].VolumeDensityPressureRohash.z;
		const vec3 x_ij = particle[index_x].positionGroup.xyz - particle[curr_neighbour].positionGroup.xyz;

		kernel_sum += m_j * (p_i / pow(ro_i, 2) + p_j / (ro_j * ro_i)) * CalculateGradKernelWeight(x_ij);
	}
	return ro_i * kernel_sum;
}

vec3 CalculateViscosity(uint index_x){
	const int d = 3;
	uint curr_neighbour = 0;
	
	vec3 kernel_sum = vec3(0);
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float m_j = mass;
		const float ro_j = particle[curr_neighbour].VolumeDensityPressureRohash.y;
		const vec3 v_ij = particle[index_x].velocityDFSPHfactor.xyz - particle[curr_neighbour].velocityDFSPHfactor.xyz;
		const vec3 x_ij = particle[index_x].positionGroup.xyz - particle[curr_neighbour].positionGroup.xyz;
		kernel_sum += (m_j * dot(v_ij, x_ij) * CalculateGradKernelWeight(x_ij)) / (ro_j * (pow(length(x_ij), 2) + 0.01));
	}
	return kernel_sum * viscosityFactor * 2 * (d + 2);
}

float CalculateAvgDensityHash(uint index_x){
	
	float kernel_sum = 0.f;
	uint curr_neighbour = 0;
	int i = 0;
	for(; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float ro_j = particle[curr_neighbour].VolumeDensityPressureRohash.w;
		kernel_sum += ro_j;
	}
	return kernel_sum / i;
}

float CalculateAvgDerivDensity(uint index_x){
	
	float kernel_sum = 0.f;
	uint curr_neighbour = 0;
	int i = 0;
	for(; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float dro_j = CalculateDerivDensity(curr_neighbour);
		kernel_sum += dro_j;
	}
	return kernel_sum / i;
}