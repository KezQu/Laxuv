#version 460 core

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const float R = 8.31446261815324;
const mat3 I = mat3(1, 0, 0,
		0, 1, 0,
		0, 0, 1);

const uint MaxValueNeighbour = uint(0xffff);
const float cellRadius = 5;
const uint MaxCells = uint(1200 / cellRadius);
const uint HashGridSize = MaxCells * MaxCells;
const uint CellCapacity = uint(3.1415 * (4. / 3.) * pow(cellRadius, 3));
const uint MaxNeighbours = 512;

const uint NONE = 0;

const uint CUBE_WORLD = 1;
const uint SPHERE_WORLD = 2;

const uint VELOCITY = 1;
const uint DENSITY_ERROR = 2;
const uint DIVERGENCE_ERROR = 3;
const uint PRESSURE = 4;

uniform uint worldType;
uniform float spaceLimiter;
uniform float boundsViscosity;

uniform float kernel_a;
uniform float kernel_c = 3;
uniform float influenceKernel;
uniform float searchKernel;
uniform float colorOpacity;
uniform uint colorType;

uniform float viscosityFactor;
uniform float dt;
uniform float mass;

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
	vec4 position;
	vec4 VolumeDensityPressureRohash;
	vec4 particleColor;
	uint neighbours[MaxNeighbours];
};

layout(std430, binding = 0) buffer dataBuffer{
	restrict ParticleProperties particle[];
};
layout(std430, binding = 1) buffer spaceGrid{
	uint space_grid[HashGridSize][CellCapacity];
};

float CalculateKernelWeight(vec3 points_dist){
	const float abs_dist = length(points_dist);
	if(abs_dist > influenceKernel){
		return 0.f;
	}
	//TEST KERNEL 1
	return 2 * pow(influenceKernel - abs_dist, kernel_c) / pow(influenceKernel, kernel_a);
	//TEST KERNEL 2
//	return (5 / 3.f) * (influenceKernel + abs_dist) * pow(influenceKernel - abs_dist, kernel_c) / pow(influenceKernel, kernel_a);
}

vec3 CalculateGradKernelWeight(vec3 points_dist){
	const float abs_dist = length(points_dist);
	const vec3 versor_ij = abs_dist > 0 ? normalize(points_dist): vec3(0);
	if(abs_dist > influenceKernel){
		return vec3(0);
	}
	//TEST KERNEL 1
	return -2 * kernel_c * pow(influenceKernel - abs_dist, kernel_c - 1) / pow(influenceKernel, kernel_a) * versor_ij;
	//TEST KERNEL 2
//	return  -(5 / 3.f) * ((kernel_c - 1) * influenceKernel + (kernel_c + 1) * abs_dist) * pow(influenceKernel - abs_dist, kernel_c - 1) / pow(influenceKernel, kernel_a) * versor_ij;
}

void ClampVelocity(uint index_x){
	float particle_speed = length(particle[index_x].velocityDFSPHfactor.xyz);
	if(particle_speed > 50){
//		particle[index_x].velocityDFSPHfactor.xyz *= 50 / particle_speed;
	}
}

uint EncodeCellNumber(vec3 cell){
	return uint(mod(73856093 * cell.x + 19349663 * cell.y + 83492791 * cell.z, HashGridSize));
}

vec3 BounceOfAWall(vec3 direction, vec3 normal){
	return direction - 2 * dot(direction, normal) * normal;
}

void ClearSpaceGrid(uint index_x, uint MaxParticles){
	uint clear_range = HashGridSize / MaxParticles;
	for(uint i = index_x * clear_range; i < (index_x != MaxParticles ? (index_x + 1) * clear_range: HashGridSize); i++){
			space_grid[i][0] = 0;
	}
}

//uint FindCell(uint index_x){
//	vec3 interim_cell = ceil((particle[index_x].position.xyz + 600) / cellRadius);
//	uint cell = EncodeCellNumber(interim_cell);
//	particle[index_x].cell = uvec4(interim_cell, cell.x);
//	return cell;
//}
//
//void AssignToCell(uint index_x){
//	uint cell = FindCell(index_x);
//	uint cell_index = atomicAdd(space_grid[cell.x][0], 1);
//	space_grid[cell.x][cell_index + 1] = index_x;
//}

//void FindNeighbours(uint index_x, uint MaxParticles){
//	for(uint i = 0; i < MaxNeighbours; i++){
//			particle[index_x].neighbours[i] = MaxValueNeighbour;
//	}
//	const vec3 pos_x = particle[index_x].position.xyz;
//	uint num_neighbours = 0;
//	for(uint j = 0; j < 27; j++){
/////////////////////////////////////////////////////////////////////////////////////	    
//		const uint x = j % 3;
//	    const uint y = (j / 3) % 3;
//	    const uint z = j / (3 * 3);
//		const uvec3 curr_cell = particle[index_x].cell.xyz + uvec3(x,y,z) - 1;
//		const uint encoded_cell = EncodeCellNumber(curr_cell);
/////////////////////////////////////////////////////////////////////////////////////	    
//		for(uint i = 0; i < space_grid[encoded_cell][0] && num_neighbours < MaxNeighbours; i++){
//			const uint neighbor_index = space_grid[encoded_cell][i + 1];
//			if(neighbor_index != index_x){
//				const vec3 pos_neighbour = particle[neighbor_index].position.xyz;
//				if(length(pos_x - pos_neighbour) < searchKernel){
//					particle[index_x].neighbours[num_neighbours] = neighbor_index;
//					num_neighbours++;
//				}
//			}
//		}
//	}
//}

void FindNeighbours(uint index_x, uint MaxParticles){
	for(uint i = 0; i < MaxNeighbours; i++){
			particle[index_x].neighbours[i] = MaxValueNeighbour;
	}
	vec3 pos_x = particle[index_x].position.xyz;
	if(!isnan(pos_x.x)){
		uint num_neighbours = 0;
		for(uint i = 0; i < MaxParticles && num_neighbours < MaxNeighbours; i++){
			if(i != index_x){
				const vec3 pos_neighbour = particle[i].position.xyz;
				if(length(pos_x - pos_neighbour) < searchKernel){
					particle[index_x].neighbours[num_neighbours] = i;
					num_neighbours++;
				}
			}
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
		const vec3 x_i = particle[index_x].position.xyz;
		const vec3 x_j = particle[curr_neighbour].position.xyz;
		const vec3 gradW_ij = CalculateGradKernelWeight(x_i - x_j);
		sum += m_j * gradW_ij;
		sum_of_squared += pow(length(m_j * gradW_ij), 2);
	}
	return pow(particle[index_x].VolumeDensityPressureRohash.y, 2) / (pow(length(sum), 2) + sum_of_squared + 1e-6);
}

float CalculateDensity(uint index_x){
	float kernel_sum = mass;
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const float m_j = mass;
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
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
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;

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
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;

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
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
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


void CheckWorldBounds(uint index_x){
	vec3 vec_form_center = particle[index_x].position.xyz + dt * particle[index_x].velocityDFSPHfactor.xyz;

	if(worldType == CUBE_WORLD){
		if(vec_form_center.x <= -spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, vec3(1, 0, 0));
		}else if(vec_form_center.x >= spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, vec3(-1, 0, 0));
		}
		if(vec_form_center.y <= -spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, vec3(0, 1, 0));
		}else if(vec_form_center.y >= spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, vec3(0, -1, 0));
		}
		if(vec_form_center.z <= -spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, vec3(0, 0, 1));
		}else if(vec_form_center.z >= spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, vec3(0, 0, -1));
		}
	}else if(worldType == SPHERE_WORLD){
		if(length(vec_form_center) >= spaceLimiter){
			particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, normalize(vec_form_center));
		}
	}
}

vec4 CalculateColor(float property_value){
	vec4 color = vec4(0, 0, 0, colorOpacity);
	const float boundCheck = 0.25f;
	
	if (property_value <= -1.f){
		color.xyz = vec3(0, 0, 1);
	}
	else if(property_value < 0.f){
		color.xyz = vec3(0, 0, 1 - property_value);
	}
	else if(property_value < boundCheck){
		color.xyz = vec3(0, property_value * 4 , 1);
	}
	else if(property_value < 2 * boundCheck){
		color.xyz = vec3(0, 1, 1 - (property_value - boundCheck) * 4);
	}
	else if(property_value < 3 * boundCheck){
		color.xyz = vec3(property_value * 4, 1, 0);
	}
	else if(property_value <= 4 * boundCheck){
		color.xyz = vec3(1, 1 - (property_value - boundCheck) * 4, 0);
	}
	else{
		color.xyz = vec3(1, 1, 1);
	}
	return color;
}

vec4 ChooseParticleColor(uint index_x){
	const float max_speed = 100.f;
	const float density0 = mass;

	switch(colorType){
		case VELOCITY:
			return CalculateColor(length(particle[index_x].velocityDFSPHfactor.xyz) / max_speed);
		case DENSITY_ERROR:
			return CalculateColor(abs(CalculateDensity(index_x) - density0) / density0);
		case DIVERGENCE_ERROR:
			return CalculateColor(abs(CalculateDerivDensity(index_x)) / (2 * mass * max_speed * 0.7f));
		case PRESSURE:
			return CalculateColor(dt * dt * particle[index_x].VolumeDensityPressureRohash.z / (density0 * particle[index_x].velocityDFSPHfactor.w));
	}
}
