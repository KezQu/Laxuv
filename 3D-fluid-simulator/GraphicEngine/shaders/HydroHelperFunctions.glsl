#version 460 core

const float E = 2.71828;
const float PI = 3.14159265358979323846264338327950288;
const float g = 9.80665;
const float R = 8.31446261815324;
const mat3 I = mat3(1, 0, 0,
					0, 1, 0,
					0, 0, 1);
const mat4 I4 = mat4(1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 1, 0,
					 0, 0, 0, 1);


const uint MaxValueNeighbour = uint(0xffff);
const uint MaxNeighbours = 512;

const uint NONE = 0;

const uint W_CUBE = 1;
const uint W_SPHERE = 2;

const uint S_POINT = 1;
const uint S_LINE = 2;
const uint S_SQUARE = 3;
const uint S_CUBE = 4;
const uint S_SPHERE = 5;

uniform mat4 model;
uniform vec3 scale;

uniform uint MaxObstacles = 0;
uniform uint terrainId = 0xfffffff;
uniform uint worldType;
uniform float spaceLimiter;
uniform float boundsViscosity;

uniform float kernel_a;

uniform float shapeRadius;
uniform float influenceKernel;

uniform float viscosityFactor;
uniform float dt;

struct ParticleProperties {
	vec4 velocityDFSPHfactor;
	vec4 position;
	dvec4 MassDensityPressureDro_Dt;
  vec4 color;
	uint neighbours[MaxNeighbours];
};

struct TerrainProperties {
	vec4 center;
	vec4 bounds;
	mat4 model;
};


layout(std430, binding = 0) buffer dataBuffer{
	restrict ParticleProperties particle[];
};

layout(std430, binding = 1) buffer terrainBuffer{
	restrict TerrainProperties terrain[];
};

double CalculateKernelWeight(vec3 points_dist){
	const float abs_dist = length(points_dist);
	if(abs_dist > influenceKernel){
		return 0.f;
	}
	return (kernel_a / 2.) * pow(influenceKernel - abs_dist, kernel_a - 1) / pow(influenceKernel, kernel_a);
}

vec3 CalculateGradKernelWeight(vec3 points_dist){
	const float abs_dist = length(points_dist);
	const vec3 versor_ij = abs_dist > 0 ? normalize(points_dist): vec3(0);
	if(abs_dist > influenceKernel){
		return vec3(0);
	}
	return - (kernel_a / 2.) * (kernel_a - 1) * pow(influenceKernel - abs_dist, kernel_a - 2) / pow(influenceKernel, kernel_a) * versor_ij;
}

double GetInternalDensity(uint index_x){
	return particle[index_x].MassDensityPressureDro_Dt.x * CalculateKernelWeight(vec3(0));
}

vec3 BounceOfAWall(vec3 direction, vec3 surface_dir){
	vec3 normal = vec3(0);
	if(length(surface_dir) > 0){
		normal = normalize(surface_dir);
	}
	return direction - boundsViscosity * 2 * dot(direction, normal) * normal;
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
				if(length(pos_x - pos_neighbour) < influenceKernel){
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
		const double m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
		const vec3 x_i = particle[index_x].position.xyz;
		const vec3 x_j = particle[curr_neighbour].position.xyz;
		const vec3 gradW_ij = CalculateGradKernelWeight(x_i - x_j);
		sum += float(m_j) * gradW_ij;
		sum_of_squared += pow(float(length(m_j * gradW_ij)), 2);
	}
	return pow(float(particle[index_x].MassDensityPressureDro_Dt.y), 2) / (pow(length(sum), 2) + sum_of_squared + 1e-6);
}

double CalculateDensity(uint index_x){
	double kernel_sum = GetInternalDensity(index_x);
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const double m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
		kernel_sum += m_j * CalculateKernelWeight(x_ij);
	}
	return kernel_sum;
}

double CalculateDerivDensity(uint index_x){
	const vec3 v_i = particle[index_x].velocityDFSPHfactor.xyz;
	
	double kernel_sum = 0.f;
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const double m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
		const vec3 v_j = particle[curr_neighbour].velocityDFSPHfactor.xyz;
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;

		kernel_sum += m_j * dot((v_i - v_j), CalculateGradKernelWeight(x_ij));
	}
	return kernel_sum;
}

vec3 CalculateGradPressure(uint index_x){
	const double ro_i = particle[index_x].MassDensityPressureDro_Dt.y;
	const double p_i = particle[index_x].MassDensityPressureDro_Dt.z;
	
	vec3 kernel_sum = vec3(0);
	uint curr_neighbour = 0;
	for(int i = 0; i < MaxNeighbours; i++){
		curr_neighbour = particle[index_x].neighbours[i];
		if(curr_neighbour == MaxValueNeighbour){
			break;
		}
		const double m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
		const double ro_j = particle[curr_neighbour].MassDensityPressureDro_Dt.y;
		const double p_j = particle[curr_neighbour].MassDensityPressureDro_Dt.z;
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;

		kernel_sum += float(m_j * p_i / pow(float(ro_i), 2) + p_j / pow(float(ro_j), 2)) * CalculateGradKernelWeight(x_ij);
	}
	return kernel_sum; // zostaje na pamiątkę zmarnowanego miesiąca szukania błędu ro_i * kernel_sum
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
		const double m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
		const double ro_j = particle[curr_neighbour].MassDensityPressureDro_Dt.y;
		const vec3 v_ij = particle[index_x].velocityDFSPHfactor.xyz - particle[curr_neighbour].velocityDFSPHfactor.xyz;
		const vec3 x_ij = particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
		kernel_sum += (float(m_j) * dot(v_ij, x_ij) * CalculateGradKernelWeight(x_ij)) / (float(ro_j) * (pow(length(x_ij), 2) + 0.01));
	}
	return kernel_sum * viscosityFactor * 2 * (d + 2);
}

void CheckWorldBounds(uint index_x){
	vec3 vec_from_center = particle[index_x].position.xyz + dt * particle[index_x].velocityDFSPHfactor.xyz;
	vec3 surface_dir = vec3(0);
	vec3 correction = vec3(0);

	if(worldType == W_CUBE){
		if(vec_from_center.x <= -spaceLimiter){
			surface_dir = vec3(1, 0, 0);
		}else if(vec_from_center.x >= spaceLimiter){
			surface_dir = vec3(-1, 0, 0);
		}
		if(vec_from_center.y <= -spaceLimiter){
			surface_dir = vec3(0, 1, 0);
		}else if(vec_from_center.y >= spaceLimiter){
			surface_dir = vec3(0, -1, 0);
		}
		if(vec_from_center.z <= -spaceLimiter){
			surface_dir = vec3(0, 0, 1);
		}else if(vec_from_center.z >= spaceLimiter){
			surface_dir = vec3(0, 0, -1);
		}
		correction = 1 - vec_from_center / spaceLimiter;
		correction.x = correction.x > 0 ? 0 :correction.x;
		correction.y = correction.y > 0 ? 0 :correction.y;
		correction.z = correction.z > 0 ? 0 :correction.z;
	}else if(worldType == W_SPHERE){
		if(length(vec_from_center) >= spaceLimiter){
			surface_dir = vec_from_center;
			correction = vec3(1 - length(vec_from_center) / spaceLimiter);
		}
	}
	particle[index_x].velocityDFSPHfactor.xyz = BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, surface_dir);
	particle[index_x].position.xyz += correction * vec_from_center;
}

void UpdateTerrainOrientation(uint index_x){
	if(terrainId != 0xffffffff){
		terrain[terrainId].center.xyz = particle[index_x].position.xyz;
		terrain[terrainId].bounds = vec4(scale, shapeRadius);
		terrain[terrainId].model = model;
	}
}

void CheckCollisions(uint index_x){
	for(uint i = 0; i < MaxObstacles; i++){
		const vec3 scale = terrain[i].bounds.xyz * terrain[i].bounds.w + shapeRadius;
		const mat4 scale_mat = mat4(scale.x,0,0,0,
									0,scale.y,0,0,
									0,0,scale.z,0,
									0,0,0,		1);
		const mat4 translate_mat = mat4(1,0,0,terrain[i].model[3].x,
										0,1,0,terrain[i].model[3].y,
										0,0,1,terrain[i].model[3].z,
										0,0,0,					  1);

		const mat4 t_model_1 = inverse(terrain[i].model * scale_mat);
		const vec3 pos_local = particle[index_x].position.xyz;
		const vec3 vel_local = particle[index_x].velocityDFSPHfactor.xyz;
		const vec4 pos = t_model_1 * vec4(pos_local, 1);
		const vec4 pos_vdt = t_model_1 * vec4(pos_local + dt * vel_local, 1);

		vec3 surface_dir = vec3(0);
		vec3 correction = vec3(0);
		vec3 abs_pos = abs(pos.xyz);

		if(uint(terrain[i].center.w) == S_CUBE){
			float max_side = max(abs_pos.x, max(abs_pos.y, abs_pos.z));
			if(abs_pos.x < 1 && abs_pos.y < 1 && abs_pos.z < 1){
				if(max_side == abs_pos.x){
					surface_dir += pos.x < 0.f ? vec3(1,0,0) : vec3(-1,0,0);
				}if(max_side == abs_pos.y){
					surface_dir += pos.y < 0.f ? vec3(0,1,0) : vec3(0,-1,0);
				}if(max_side == abs_pos.z){
					surface_dir += pos.z < 0.f ? vec3(0,0,1) : vec3(0,0,-1);
				}
				correction = vec3(-1);
			}
		}else if(uint(terrain[i].center.w) == S_SPHERE){
			if(length(pos.xyz) <= 1){
				surface_dir = pos.xyz;
				correction = vec3(1);
			}
		}
		if(length(surface_dir) > 0){
			surface_dir = (transpose(transpose(terrain[i].model) * inverse(translate_mat)) * inverse(scale_mat) * vec4(surface_dir, 1)).xyz;
		}
		if(uint(terrain[i].center.w) == S_CUBE){
			if(abs(pos_vdt).x < 1 && abs(pos_vdt).y < 1 && abs(pos_vdt).z < 1){
			}
		}else if(uint(terrain[i].center.w) == S_SPHERE){
			if(length(pos_vdt.xyz) <= 1){
			}
		}
		particle[index_x].position.xyz += correction * surface_dir;
		particle[index_x].velocityDFSPHfactor.xyz = BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, surface_dir);
	}
}
