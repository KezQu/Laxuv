#version 460 core

// Specify mathematical constants used in the simulation
const float g = 9.80665;

// Define Implementation specific constants
const uint MaxValueNeighbour = uint(0xffff);
const uint MaxNeighbors = 512;
const uint MaxGranularity = 16;

const uint NONE = 0;
// Create enum-like variables to match equivalent enums in the C++
// interfaces
const uint W_CUBE = 1;
const uint W_SPHERE = 2;

const uint S_CUBE = 1;
const uint S_SPHERE = 2;
const uint S_LINE = 3;

uniform mat4 model;
uniform vec3 scale;

uniform uint MaxObstacles = 0;
uniform uint terrainId = 0xfffffff;
uniform uint worldType;
uniform float spaceLimiter;
uniform float boundsViscosity;

uniform float kernel_a;
uniform float influenceKernel;

uniform float shapeRadius;
uniform float viscosityFactor;
uniform float dt;

// Define structs containing required properties of particles and terrain to
// perform simulation
struct ParticleProperties
{
  vec4 velocityDFSPHfactor;
  vec4 position;
  vec4 MassDensityPressureDro_Dt;
  vec4 color;
  uint neighbors[MaxNeighbors];
};

struct TerrainProperties
{
  vec4 center;
  vec4 bounds;
  mat4 model;
};

// Define Shader Storage Buffers used during the simulation
layout(std430, binding = 0) buffer dataBuffer
{
  restrict ParticleProperties particle[];
};

layout(std430, binding = 1) buffer granularityBuffer
{
  restrict uint granularity[][MaxGranularity];
};

layout(std430, binding = 2) buffer terrainBuffer
{
  restrict TerrainProperties terrain[];
};

// Defines function responsible for calculating the smoothing kernel value based
// on the relative position to the center of the currently processed particle
float CalculateKernelWeight(vec3 points_dist)
{
  const float abs_dist = length(points_dist);
  if (abs_dist > influenceKernel)
  {
    return 0.f;
  }
  return (kernel_a / 2.) * pow(influenceKernel - abs_dist, kernel_a - 1) /
         pow(influenceKernel, kernel_a);
}

// Defines function responsible for calculating the gradient of a smoothing
// kernel value based on the relative position to the center of the currently
// processed particle
vec3 CalculateGradKernelWeight(vec3 points_dist)
{
  const float abs_dist = length(points_dist);
  const vec3 versor_ij = abs_dist > 0 ? normalize(points_dist) : vec3(0);
  if (abs_dist > influenceKernel)
  {
    return vec3(0);
  }
  return -(kernel_a / 2.) * (kernel_a - 1) *
         pow(influenceKernel - abs_dist, kernel_a - 2) /
         pow(influenceKernel, kernel_a) * versor_ij;
}

// Defines function responsible for calculating the laplace of a smoothing
// kernel value based on the relative position to the center of the currently
// processed particle
float CalculateLaplasjanKernelWeight(vec3 points_dist)
{
  const float abs_dist = length(points_dist);
  if (abs_dist > influenceKernel)
  {
    return 0;
  }
  return (kernel_a / 2.) * (kernel_a - 1) * (kernel_a - 2) *
         pow(influenceKernel - abs_dist, kernel_a - 3) /
         pow(influenceKernel, kernel_a);
}

// Function responsible for retrieving density of a single particle
float GetInternalDensity(uint index_x)
{
  return particle[index_x].MassDensityPressureDro_Dt.x *
         CalculateKernelWeight(vec3(0));
}

// Function defining a net vector as a result of bouncing of a surface with a
// given surface normal and factor defining the amount of a energy lost during
// collision
vec3 BounceOfAWall(vec3 direction, vec3 surface_dir)
{
  vec3 normal = vec3(0);
  // Create a surface normal
  if (length(surface_dir) > 0)
  {
    normal = normalize(surface_dir);
  }
  // Apply bouncing equation for a given vector
  return direction - boundsViscosity * 2 * dot(direction, normal) * normal;
}

// Helper function implementing brute force approach for fixed-radius near
// neighbor search
void FindNeighbors(uint index_x, uint MaxParticles)
{
  // Set neighbor list to a default value
  for (uint i = 0; i < MaxNeighbors; i++)
  {
    particle[index_x].neighbors[i] = MaxValueNeighbour;
  }
  // Consider only particles that have valid position
  vec3 pos_x = particle[index_x].position.xyz;
  if (!isnan(pos_x.x))
  {
    uint num_neighbors = 0;
    // Go though all particles and stop if neighbor list goes out of range
    for (uint i = 0; i < MaxParticles && num_neighbors < MaxNeighbors; i++)
    {
      // Ensure not counting the particle itself as a neighbor
      if (i != index_x)
      {
        const vec3 pos_neighbour = particle[i].position.xyz;
        // Check if the distance between the particle and proposed neighbor
        // don't fall out of the smoothing kernel range
        if (length(pos_x - pos_neighbour) < influenceKernel)
        {
          particle[index_x].neighbors[num_neighbors] = i;
          num_neighbors++;
        }
      }
    }
  }
}

// Function responsible for calculating the alpha factor used in divergence-free
// smoothing particle hydrodynamics algorithm
float CalculateDFSPHFactor(uint index_x)
{
  vec3 sum = vec3(0);
  float sum_of_squared = 0.f;
  uint curr_neighbour = 0;
  // Consider only particles within the neighborhood
  for (int i = 0; i < MaxNeighbors; i++)
  {
    curr_neighbour = particle[index_x].neighbors[i];
    // Stop if the current neighbor is not a valid particle
    if (curr_neighbour == MaxValueNeighbour)
    {
      break;
    }
    const float m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
    const vec3 x_i = particle[index_x].position.xyz;
    const vec3 x_j = particle[curr_neighbour].position.xyz;
    const vec3 gradW_ij = CalculateGradKernelWeight(x_i - x_j);
    sum += float(m_j) * gradW_ij;
    sum_of_squared += pow(float(length(m_j * gradW_ij)), 2);
  }
  // Apply the equation used for calculating alpha factor
  return pow(float(particle[index_x].MassDensityPressureDro_Dt.y), 2) /
         (pow(length(sum), 2) + sum_of_squared + 1e-6);
}

// Helper function allowing to retrieve current density for a specified particle
float CalculateDensity(uint index_x)
{
  // Retrieve value of a density for the particle itself
  float kernel_sum = GetInternalDensity(index_x);
  uint curr_neighbour = 0;
  for (int i = 0; i < MaxNeighbors; i++)
  {
    curr_neighbour = particle[index_x].neighbors[i];
    // Stop if the current neighbor is not a valid particle
    if (curr_neighbour == MaxValueNeighbour)
    {
      break;
    }
    const float m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
    const vec3 x_ij =
        particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
    // Include densities of the neighbors based on the smoothing kernel function
    // value
    kernel_sum += m_j * CalculateKernelWeight(x_ij);
  }
  return kernel_sum;
}

// Helper function allowing to retrieve current density derivative for a
// specified particle
float CalculateDerivDensity(uint index_x)
{
  const vec3 v_i = particle[index_x].velocityDFSPHfactor.xyz;

  float kernel_sum = 0.f;
  uint curr_neighbour = 0;
  for (int i = 0; i < MaxNeighbors; i++)
  {
    curr_neighbour = particle[index_x].neighbors[i];
    // Stop if the current neighbor is not a valid particle
    if (curr_neighbour == MaxValueNeighbour)
    {
      break;
    }
    const float m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
    const vec3 v_j = particle[curr_neighbour].velocityDFSPHfactor.xyz;
    const vec3 x_ij =
        particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
    // Retrieve density change based on the equation from the DFSPH algorithm
    kernel_sum += m_j * dot((v_i - v_j), CalculateGradKernelWeight(x_ij));
  }
  return kernel_sum;
}

// Function responsible for providing the gradient of the pressure for a given
// simulation frame
vec3 CalculateGradPressure(uint index_x)
{
  const float ro_i = particle[index_x].MassDensityPressureDro_Dt.y;
  const float p_i = particle[index_x].MassDensityPressureDro_Dt.z;

  vec3 kernel_sum = vec3(0);
  uint curr_neighbour = 0;
  for (int i = 0; i < MaxNeighbors; i++)
  {
    curr_neighbour = particle[index_x].neighbors[i];
    // Stop if the current neighbor is not a valid particle
    if (curr_neighbour == MaxValueNeighbour)
    {
      break;
    }
    const float m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
    const float ro_j = particle[curr_neighbour].MassDensityPressureDro_Dt.y;
    const float p_j = particle[curr_neighbour].MassDensityPressureDro_Dt.z;
    const vec3 x_ij =
        particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;

    // Retrieve pressure gradient based on the equation from the DFSPH algorithm
    kernel_sum +=
        float(m_j * p_i / pow(float(ro_i), 2) + p_j / pow(float(ro_j), 2)) *
        CalculateGradKernelWeight(x_ij);
  }
  return kernel_sum;
}

vec3 CalculateViscosity(uint index_x)
{
  uint curr_neighbour = 0;
  vec3 kernel_sum = vec3(0);
  for (int i = 0; i < MaxNeighbors; i++)
  {
    curr_neighbour = particle[index_x].neighbors[i];
    // Stop if the current neighbor is not a valid particle
    if (curr_neighbour == MaxValueNeighbour)
    {
      break;
    }
    const float m_j = particle[curr_neighbour].MassDensityPressureDro_Dt.x;
    const float ro_j = particle[curr_neighbour].MassDensityPressureDro_Dt.y;
    const vec3 v_ij = particle[index_x].velocityDFSPHfactor.xyz -
                      particle[curr_neighbour].velocityDFSPHfactor.xyz;
    const vec3 x_ij =
        particle[index_x].position.xyz - particle[curr_neighbour].position.xyz;
    kernel_sum += (v_ij * float(m_j) * CalculateLaplasjanKernelWeight(x_ij)) /
                  float(ro_j);
  }
  return kernel_sum * viscosityFactor;
}

// Function responsible for checking if a given particle falls out of world
// bounds, and bounce it back if so
void CheckWorldBounds(uint index_x)
{
  vec3 vec_from_center = particle[index_x].position.xyz +
                         dt * particle[index_x].velocityDFSPHfactor.xyz;
  vec3 surface_dir = vec3(0);
  vec3 correction = vec3(0);

  // Choose the bouncing method based on the world bounds shape
  if (worldType == W_CUBE)
  {
    // Recreate a surface direction based on which side of the cube was bypassed
    if (vec_from_center.x <= -spaceLimiter)
    {
      surface_dir += vec3(1, 0, 0);
    }
    else if (vec_from_center.x >= spaceLimiter)
    {
      surface_dir += vec3(-1, 0, 0);
    }
    if (vec_from_center.y <= -spaceLimiter)
    {
      surface_dir += vec3(0, 1, 0);
    }
    else if (vec_from_center.y >= spaceLimiter)
    {
      surface_dir += vec3(0, -1, 0);
    }
    if (vec_from_center.z <= -spaceLimiter)
    {
      surface_dir += vec3(0, 0, 1);
    }
    else if (vec_from_center.z >= spaceLimiter)
    {
      surface_dir += vec3(0, 0, -1);
    }
    // Explicitly define position correction vector to minimize artifacts
    correction = 1 - abs(vec_from_center) / spaceLimiter;
    correction.x = correction.x > 0 ? 0 : correction.x;
    correction.y = correction.y > 0 ? 0 : correction.y;
    correction.z = correction.z > 0 ? 0 : correction.z;
  }
  else if (worldType == W_SPHERE)
  {
    // Create a surface normal if the particle position falls out of world shape
    if (length(vec_from_center) >= spaceLimiter)
    {
      surface_dir = vec_from_center;
      // Explicitly define position correction vector to minimize artifacts
      correction = vec3(1 - length(vec_from_center) / spaceLimiter);
    }
  }
  // Correct the position velocity vector if the particle crossed a given plane
  // and should bounce out of it
  particle[index_x].velocityDFSPHfactor.xyz =
      BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, surface_dir);
  // Apply explicit position correction to minimize artifacts caused by the fact
  // that particle already crossed a given plane
  particle[index_x].position.xyz += correction * vec_from_center;
}

// Update the parameters of the terrain based on the currently processed object
// registered as a terrain
void UpdateTerrainOrientation(uint index_x)
{
  // Update values if the terrain id is a valid value
  if (terrainId != 0xffffffff)
  {
    terrain[terrainId].center.xyz = particle[index_x].position.xyz;
    terrain[terrainId].bounds = vec4(scale, shapeRadius);
    terrain[terrainId].model = model;
  }
}

// Helper function used to check if current particle didn't hit the terrain and
// if so process it
void CheckCollisions(uint index_x)
{
  // Check collision with every registered terrain
  for (uint i = 0; i < MaxObstacles; i++)
  {
    // Create matrices to transform particle to the terrain coordinate system
    const vec3 scale =
        terrain[i].bounds.xyz * terrain[i].bounds.w + shapeRadius;
    const mat4 scale_mat =
        mat4(scale.x, 0, 0, 0, 0, scale.y, 0, 0, 0, 0, scale.z, 0, 0, 0, 0, 1);
    const mat4 translate_mat =
        mat4(1, 0, 0, terrain[i].model[3].x, 0, 1, 0, terrain[i].model[3].y, 0,
             0, 1, terrain[i].model[3].z, 0, 0, 0, 1);

    // Convert the particle position to the terrain coordinates
    const mat4 t_model_1 = inverse(terrain[i].model * scale_mat);
    const vec3 pos_local = particle[index_x].position.xyz;
    const vec3 vel_local = particle[index_x].velocityDFSPHfactor.xyz;
    const vec4 pos = t_model_1 * vec4(pos_local, 1);
    const vec4 pos_vdt = t_model_1 * vec4(pos_local + dt * vel_local, 1);

    vec3 surface_dir = vec3(0);
    vec3 correction = vec3(0);
    vec3 abs_pos = abs(pos.xyz);

    if (uint(terrain[i].center.w) == S_CUBE)
    {
      float max_side = max(abs_pos.x, max(abs_pos.y, abs_pos.z));
      // Recreate the surface normal based on the side the particle crossed when
      // the terrain shape is cube
      if (abs_pos.x < 1 && abs_pos.y < 1 && abs_pos.z < 1)
      {
        if (max_side == abs_pos.x)
        {
          surface_dir += pos.x < 0.f ? vec3(1, 0, 0) : vec3(-1, 0, 0);
        }
        if (max_side == abs_pos.y)
        {
          surface_dir += pos.y < 0.f ? vec3(0, 1, 0) : vec3(0, -1, 0);
        }
        if (max_side == abs_pos.z)
        {
          surface_dir += pos.z < 0.f ? vec3(0, 0, 1) : vec3(0, 0, -1);
        }
        correction = vec3(-1);
      }
    }
    else if (uint(terrain[i].center.w) == S_SPHERE)
    {
      // Create a surface normal if the particle is inside the sphere
      if (length(pos.xyz) <= 1)
      {
        surface_dir = pos.xyz;
        correction = vec3(1);
      }
    }
    if (length(surface_dir) > 0)
    {
      // Transform the created surface normal to the particle coordinate system
      surface_dir =
          (transpose(transpose(terrain[i].model) * inverse(translate_mat)) *
           inverse(scale_mat) * vec4(surface_dir, 1))
              .xyz;
    }
    // Transform particle velocity based on the created surface normal
    particle[index_x].position.xyz += correction * surface_dir;
    particle[index_x].velocityDFSPHfactor.xyz =
        BounceOfAWall(particle[index_x].velocityDFSPHfactor.xyz, surface_dir);
  }
}
