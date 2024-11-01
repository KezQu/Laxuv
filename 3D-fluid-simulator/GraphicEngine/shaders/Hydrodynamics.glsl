#version 460 core

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const float g = 9.80665;
const float R = 8.31446261815324;
const float kernel_c = 3;
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

const uint W_CUBE = 1;
const uint W_SPHERE = 2;

const uint S_POINT = 1;
const uint S_LINE = 2;
const uint S_SQUARE = 3;
const uint S_CUBE = 4;
const uint S_SPHERE = 5;

const uint VELOCITY = 1;
const uint DENSITY_ERROR = 2;
const uint DIVERGENCE_ERROR = 3;
const uint PRESSURE = 4;

uniform mat4 model;

uniform uint MaxObstacles = 0;
uniform uint terrainId = 0xfffffff;
uniform uint worldType;
uniform float spaceLimiter;
uniform float boundsViscosity;

uniform float kernel_a;

uniform float shapeRadius;
uniform float influenceKernel;
uniform float searchKernel;
uniform float colorOpacity;
uniform uint colorType;

uniform float viscosityFactor;
uniform float dt;
uniform float mass;

struct ParticleProperties {
	vec4 velocityDFSPHfactor;
	vec4 position;
	vec4 VolumeDensityPressureRohash;
	vec4 color;
	uint neighbours[MaxNeighbours];
};

struct TerrainProperties {
	vec4 center;
	mat4 model;
};
//	vec4 bounds;

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

layout(std430, binding = 0) buffer dataBuffer{
	restrict ParticleProperties particle[];
};
layout(std430, binding = 1) buffer terrainBuffer{
	restrict TerrainProperties terrain[];
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
	vec3 vec_from_center = particle[index_x].position.xyz + dt * particle[index_x].velocityDFSPHfactor.xyz;
	vec3 bounce_normal = vec3(0);
	vec3 correction = vec3(0);

	if(worldType == W_CUBE){
		if(vec_from_center.x <= -spaceLimiter){
			bounce_normal = vec3(1, 0, 0);
		}else if(vec_from_center.x >= spaceLimiter){
			bounce_normal = vec3(-1, 0, 0);
		}
		if(vec_from_center.y <= -spaceLimiter){
			bounce_normal = vec3(0, 1, 0);
		}else if(vec_from_center.y >= spaceLimiter){
			bounce_normal = vec3(0, -1, 0);
		}
		if(vec_from_center.z <= -spaceLimiter){
			bounce_normal = vec3(0, 0, 1);
		}else if(vec_from_center.z >= spaceLimiter){
			bounce_normal = vec3(0, 0, -1);
		}
		correction = 1 - vec_from_center / spaceLimiter;
		correction.x = correction.x > 0 ? 0 :correction.x;
		correction.y = correction.y > 0 ? 0 :correction.y;
		correction.z = correction.z > 0 ? 0 :correction.z;
	}else if(worldType == W_SPHERE){
		if(length(vec_from_center) >= spaceLimiter){
			bounce_normal = normalize(vec_from_center);
			correction = vec3(1 - length(vec_from_center) / spaceLimiter);
		}
	}
	particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, bounce_normal);
	particle[index_x].position.xyz += correction * vec_from_center;
}

void UpdateTerrainOrientation(){
	if(terrainId != 0xffffffff){
//		terrain[terrainId].bounds.xyz = vec3(shapeRadius);
		terrain[terrainId].model = model;
	}
}

void AddToTerrain(uint index_x){
	if(terrainId != 0xffffffff){
		terrain[terrainId].center.xyz = particle[index_x].position.xyz;
		UpdateTerrainOrientation();
	}
}

void CheckCollisions(uint index_x){
	for(uint i = 0; i < MaxObstacles; i++){
		const vec4 translation = vec4(terrain[i].model[3].xyz, 0);
		const vec3 scale = vec3(terrain[i].model[0][0], terrain[i].model[1][1], terrain[i].model[2][2]);

		const vec4 obstacle_center = translation + terrain[i].center;
		const vec3 obstacle_bounds = scale * vec3(1);
		const vec3 vec_from_center = (inverse(terrain[i].model) * vec4(particle[index_x].position.xyz + dt * particle[index_x].velocityDFSPHfactor.xyz, 1)).xyz;
		vec3 bounce_normal = vec3(0);
		vec3 correction = vec3(0);

		if(uint(obstacle_center.w) == S_CUBE){
//			float side = max(vec_from_center.x, max(vec_from_center.y, vec_from_center.x));
//			if((vec_from_center.x < obstacle_bounds.x) && (vec_from_center.y < obstacle_bounds.y) && (vec_from_center.z < obstacle_bounds.z)){
//				if(side == vec_from_center.x){
//					bounce_normal = vec3(1,0,0);
//				}
//				else if(side == vec_from_center.y){
//					bounce_normal = vec3(0,1,0);
//				}
//				else{
//					bounce_normal = vec3(0,0,1);
//				}
//			}
//			correction = 1 - vec_from_center / spaceLimiter;
//			correction.x = correction.x > 0 ? 0 :correction.x;
//			correction.y = correction.y > 0 ? 0 :correction.y;
//			correction.z = correction.z > 0 ? 0 :correction.z;
		}else if(uint(obstacle_center.w) == S_SPHERE){
			if(length(vec_from_center) <= 1){
				bounce_normal = -normalize(vec_from_center);
				correction = vec3(1 - length(vec_from_center));
			}
		}
		particle[index_x].velocityDFSPHfactor.xyz = boundsViscosity * BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, bounce_normal);
		particle[index_x].position.xyz += correction * vec_from_center;
	}
}

vec4 CalculateColor(float property_value){
	vec4 color = vec4(0, 0, 0, colorOpacity);
	const float boundCheck = 0.25f;
	
	if (property_value <= -1.f){
		color.xyz = vec3(1);
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
		color.xyz = vec3((property_value - 2 * boundCheck) * 4, 1, 0);
	}
	else if(property_value <= 4 * boundCheck){
		color.xyz = vec3(1, 1 - (property_value - 3 * boundCheck) * 4, 0);
	}
	else{
		color.xyz = vec3(0.05);
	}
	return color;
}

void ChooseParticleColor(uint index_x){
	const float max_speed = 50.f;
	const float density0 = mass;
	const float d_density = CalculateDerivDensity(index_x);
	vec4 temp_color = vec4(0);
	switch(colorType){
		case VELOCITY:
			temp_color = CalculateColor(length(particle[index_x].velocityDFSPHfactor.xyz) / max_speed);
			break;
		case DENSITY_ERROR:
			temp_color = CalculateColor(abs(CalculateDensity(index_x) - density0) / density0);
			break;
		case DIVERGENCE_ERROR:
			temp_color = CalculateColor(abs(d_density));
			break;
		case PRESSURE:
			temp_color = CalculateColor(sqrt(particle[index_x].VolumeDensityPressureRohash.z) * mass * 1e+3);
			break;
	}
	particle[index_x].color = temp_color;
}


void CalculateExternalForces(uint index_x){
	const float ro_i = particle[index_x].VolumeDensityPressureRohash.y;

	vec3 viscosity_acceleration = CalculateViscosity(index_x) / ro_i;
	particle[index_x].velocityDFSPHfactor.xyz += dt * (viscosity_acceleration - vec3(0, g, 0));
}
void SolveDensityError(uint index_x){
	const float density0 = mass;
	const float factor_x = particle[index_x].velocityDFSPHfactor.w;
	
	float ro_hash = particle[index_x].VolumeDensityPressureRohash.y + dt * CalculateDerivDensity(index_x);
	for(int i = 0; i < 500; i++){
		particle[index_x].VolumeDensityPressureRohash.z = (ro_hash  - density0) * factor_x / pow(dt, 2);

		particle[index_x].velocityDFSPHfactor.xyz -= dt * CalculateGradPressure(index_x);
		ro_hash = particle[index_x].VolumeDensityPressureRohash.y + dt * CalculateDerivDensity(index_x);

		if((abs((ro_hash - density0)) / density0 < 2e-2) && i > 5){
			break;
		}
	}
	particle[index_x].VolumeDensityPressureRohash.w = abs(particle[index_x].VolumeDensityPressureRohash.y - density0) / density0;

}
void UpdatePosition(uint index_x){
	CheckWorldBounds(index_x);
	CheckCollisions(index_x);
	particle[index_x].position.xyz += dt * particle[index_x].velocityDFSPHfactor.xyz;
}
void SolveDivergenceError(uint index_x){
	particle[index_x].VolumeDensityPressureRohash.y = CalculateDensity(index_x);
	particle[index_x].velocityDFSPHfactor.w = CalculateDFSPHFactor(index_x);
	const float factor_x = particle[index_x].velocityDFSPHfactor.w;

	float d_ro = CalculateDerivDensity(index_x);
	for(int i = 0; i < 500 && !((abs(d_ro) < 1e-2) && i > 2); i++){
		particle[index_x].VolumeDensityPressureRohash.z = d_ro * factor_x / dt;
			
		particle[index_x].velocityDFSPHfactor.xyz -= dt * CalculateGradPressure(index_x);
		d_ro = CalculateDerivDensity(index_x);
	}
	particle[index_x].position.xyz = (inverse(model) * vec4(particle[index_x].position.xyz, 1.0)).xyz;

}
