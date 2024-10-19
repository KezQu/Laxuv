#pragma once

#include <Essentials.h>
#include <imgui.h>

#include <cmath>
#include <vector>

#include "glm/glm.hpp"

struct Vector
{
  float x{};
  glm::vec3 y{0};
  float z{};
};

struct Flux
{
  glm::vec3 x{0};
  glm::mat3 y{0};
  glm::vec3 z{0};
};
using namespace glm;

struct HydroTest
{
  using uint = unsigned int;
  const float E = 2.71828;
  const float PI = 3.14159265358979323846264338327950288;
  const float R = 8.31446261815324;
  const mat3 I = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);

  const uint MaxNeighbours = 512;
  const uint MaxValueNeighbour = uint(0xffff);

  float kernel_a;
  float kernel_b;

  float influenceKernel;
  float searchKernel;
  float mass;
  float viscosityFactor = 0.2;

  std::vector<Essentials::ParticleProperties> particle;

  vec4 Qube(uint index, uint MaxParticles)
  {
    uint sideLength = static_cast<uint>(std::cbrt(MaxParticles));
    uint x = index % sideLength;
    uint y = (index / sideLength) % sideLength;
    uint z = index / (sideLength * sideLength);
    return vec4(x, y, z, 0);
  }

  void Init(float mass_new, float particleRadius, float particleSpacing,
            uint MaxParticles, uint influenceKernel_new, uint searchKernel_new,
            float kernel_a_new, float kernel_b_new)
  {
    influenceKernel = influenceKernel_new;
    searchKernel = searchKernel_new;
    kernel_a = kernel_a_new;
    kernel_b = kernel_b_new;
    particle.resize(MaxParticles);
    for (uint index = 0; index < MaxParticles; index++)
    {
      particle[index].velocityDFSPHfactor = vec4(0);
      particle[index].position = vec4(0);
      particle[index].VolumeDensityPressureRohash = vec4(0);
      particle[index].position =
          Qube(index, MaxParticles) * particleRadius * particleSpacing;
      FindNeighbours(index, MaxParticles);
      mass = mass_new;
    }
    for (uint index = 0; index < MaxParticles; index++)
    {
      particle[index].VolumeDensityPressureRohash.y = CalculateDensity(index);
      particle[index].VolumeDensityPressureRohash.x =
          (1. / CalculateDensity(index)) * mass;
      particle[index].velocityDFSPHfactor.w = CalculateDFSPHFactor(index);
    }
  }
  float CalculateKernelWeight(vec3 points_dist)
  {
    const float abs_dist = length(points_dist);

    return pow(E, -pow(abs_dist, kernel_a) / pow(influenceKernel, kernel_b));
  }

  vec3 CalculateGradKernelWeight(vec3 points_dist)
  {
    const float abs_dist = length(points_dist);
    const vec3 versor_ij = abs_dist > 0 ? normalize(points_dist) : vec3(0);

    return -kernel_a * pow(abs_dist, kernel_a - 1) *
           CalculateKernelWeight(points_dist) * versor_ij /
           float(pow(influenceKernel, kernel_b));
  }

  void FindNeighbours(uint index_x, uint MaxParticles)
  {
    for (uint i = 0; i < MaxNeighbours; i++)
    {
      particle[index_x].neighbours[i] = MaxValueNeighbour;
    }
    vec3 pos_x = vec3(particle[index_x].position.x,
                      particle[index_x].position.y,
                      particle[index_x].position.z);
    if (!isnan(pos_x.x))
    {
      uint num_neighbours = 0;
      for (uint i = 0; i < MaxParticles && num_neighbours < MaxNeighbours; i++)
      {
        //		if(i != index_x){
        const vec3 pos_neighbour =
            vec3(particle[i].position.x, particle[i].position.y,
                 particle[i].position.z);
        if (length(pos_x - pos_neighbour) < 2 * searchKernel)
        {
          particle[index_x].neighbours[num_neighbours] = i;
          num_neighbours++;
        }
        //		}
      }
    }
  }

  float CalculateDFSPHFactor(uint index_x)
  {
    vec3 sum = vec3(0);
    float sum_of_squared = 0.f;
    uint curr_neighbour = 0;
    for (int i = 0; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float m_j = mass;
      const vec3 x_i = vec3(particle[index_x].position.x,
                            particle[index_x].position.y,
                            particle[index_x].position.z);
      const vec3 x_j = vec3(particle[curr_neighbour].position.x,
                            particle[curr_neighbour].position.y,
                            particle[curr_neighbour].position.z);
      const vec3 gradW_ij = CalculateGradKernelWeight(x_i - x_j);
      sum += m_j * gradW_ij;
      sum_of_squared += pow(length(m_j * gradW_ij), 2);
    }
    return pow(particle[index_x].VolumeDensityPressureRohash.y, 2) /
           (pow(length(sum), 2) + sum_of_squared + 1e-6);
  }

  float CalculateDensity(uint index_x)
  {
    float kernel_sum = 0;
    uint curr_neighbour = 0;
    for (int i = 0; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float m_j = mass;
      const vec3 x_ij = vec3(particle[index_x].position.x,
                             particle[index_x].position.y,
                             particle[index_x].position.z) -
                        vec3(particle[curr_neighbour].position.x,
                             particle[curr_neighbour].position.y,
                             particle[curr_neighbour].position.z);
      float test = m_j * CalculateKernelWeight(x_ij);
      if (test < 0)
      {
        std::terminate();
      }
      kernel_sum += test;
    }
    return kernel_sum;
  }

  float CalculateDerivDensity(uint index_x)
  {
    const vec3 v_i = vec3(particle[index_x].velocityDFSPHfactor.x,
                          particle[index_x].velocityDFSPHfactor.y,
                          particle[index_x].velocityDFSPHfactor.z);

    float kernel_sum = 0.f;
    uint curr_neighbour = 0;
    for (int i = 0; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float m_j = mass;
      const vec3 v_j = vec3(particle[curr_neighbour].velocityDFSPHfactor.x,
                            particle[curr_neighbour].velocityDFSPHfactor.y,
                            particle[curr_neighbour].velocityDFSPHfactor.z);
      const vec3 x_ij = vec3(particle[index_x].position.x,
                             particle[index_x].position.y,
                             particle[index_x].position.z) -
                        vec3(particle[curr_neighbour].position.x,
                             particle[curr_neighbour].position.y,
                             particle[curr_neighbour].position.z);

      kernel_sum += m_j * dot((v_i - v_j), CalculateGradKernelWeight(x_ij));
    }
    return kernel_sum;
  }

  vec3 CalculateGradPressure(uint index_x)
  {
    const float ro_i = particle[index_x].VolumeDensityPressureRohash.y;
    const float p_i = particle[index_x].VolumeDensityPressureRohash.z;

    vec3 kernel_sum = vec3(0);
    uint curr_neighbour = 0;
    for (int i = 0; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float m_j = mass;
      const float ro_j = particle[curr_neighbour].VolumeDensityPressureRohash.y;
      const float p_j = particle[curr_neighbour].VolumeDensityPressureRohash.z;
      const vec3 x_ij = vec3(particle[index_x].position.x,
                             particle[index_x].position.y,
                             particle[index_x].position.z) -
                        vec3(particle[curr_neighbour].position.x,
                             particle[curr_neighbour].position.y,
                             particle[curr_neighbour].position.z);

      kernel_sum += float(m_j * (p_i / pow(ro_i, 2) + p_j / (ro_j * ro_i))) *
                    CalculateGradKernelWeight(x_ij);
    }
    return ro_i * kernel_sum;
  }

  vec3 CalculateViscosity(uint index_x)
  {
    const int d = 3;
    uint curr_neighbour = 0;

    vec3 kernel_sum = vec3(0);
    for (int i = 0; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float m_j = mass;
      const float ro_j = particle[curr_neighbour].VolumeDensityPressureRohash.y;
      const vec3 v_ij = vec3(particle[index_x].velocityDFSPHfactor.x,
                             particle[index_x].velocityDFSPHfactor.y,
                             particle[index_x].velocityDFSPHfactor.z) -
                        vec3(particle[curr_neighbour].velocityDFSPHfactor.x,
                             particle[curr_neighbour].velocityDFSPHfactor.y,
                             particle[curr_neighbour].velocityDFSPHfactor.z);
      const vec3 x_ij = vec3(particle[index_x].position.x,
                             particle[index_x].position.y,
                             particle[index_x].position.z) -
                        vec3(particle[curr_neighbour].position.x,
                             particle[curr_neighbour].position.y,
                             particle[curr_neighbour].position.z);
      kernel_sum += (m_j * dot(v_ij, x_ij) * CalculateGradKernelWeight(x_ij)) /
                    float(ro_j * (pow(length(x_ij), 2) + 0.01));
    }
    return kernel_sum * viscosityFactor * float(2 * (d + 2));
  }

  float CalculateAvgDensity(uint index_x)
  {
    float kernel_sum = 0.f;
    uint curr_neighbour = 0;
    int i = 0;
    for (; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float ro_j = particle[curr_neighbour].VolumeDensityPressureRohash.y;
      kernel_sum += ro_j;
    }
    return kernel_sum / i;
  }

  float CalculateAvgDerivDensity(uint index_x)
  {
    float kernel_sum = 0.f;
    uint curr_neighbour = 0;
    int i = 0;
    for (; i < MaxNeighbours; i++)
    {
      curr_neighbour = particle[index_x].neighbours[i];
      if (curr_neighbour == MaxValueNeighbour)
      {
        break;
      }
      const float dro_j = CalculateDerivDensity(curr_neighbour);
      kernel_sum += dro_j;
    }
    return kernel_sum / i;
  }

  void CalculateDFSPHSimulation(uint MaxParticles, float g, float density0)
  {
    const uint spaceLimiter = 100;
    const float boundsViscosity = 0.95;
    const float dt = 1.f / ImGui::GetIO().Framerate;

    for (uint index = 0; index < MaxParticles; index++)
    {
      const float m_i = mass;
      const float ro_i = particle[index].VolumeDensityPressureRohash.y;

      vec3 viscosity_acceleration = CalculateViscosity(index) / ro_i;
      particle[index].velocityDFSPHfactor.x +=
          dt * (viscosity_acceleration - vec3(0, g, 0)).x;
      particle[index].velocityDFSPHfactor.y +=
          dt * (viscosity_acceleration - vec3(0, g, 0)).y;
      particle[index].velocityDFSPHfactor.z +=
          dt * (viscosity_acceleration - vec3(0, g, 0)).z;
    }
    //---------------------------------Calc Density free
    // solver--------------------------------------
    //
    for (int i = 0; i < 100 && abs(CalculateAvgDensity(0) - density0) > 1e-1;
         i++)
    {
      for (uint index = 0; index < MaxParticles; index++)
      {
        float ro_hash = particle[index].VolumeDensityPressureRohash.y +
                        dt * CalculateDerivDensity(index);
        particle[index].VolumeDensityPressureRohash.z =
            (ro_hash - density0) * particle[index].velocityDFSPHfactor.w /
            pow(dt, 2);
      }
      for (uint index = 0; index < MaxParticles; index++)
      {
        vec3 gradP = CalculateGradPressure(index);
        particle[index].velocityDFSPHfactor.x -= dt * gradP.x * 1e-4;
        particle[index].velocityDFSPHfactor.y -= dt * gradP.y * 1e-4;
        particle[index].velocityDFSPHfactor.z -= dt * gradP.z * 1e-4;
      }
    }

    //---------------------------------Check worlds
    // bounds--------------------------------------
    for (uint index = 0; index < MaxParticles; index++)
    {
      vec3 vec_form_center =
          vec3(particle[index].position.x, particle[index].position.y,
               particle[index].position.z) +
          dt * vec3(particle[index].velocityDFSPHfactor.x,
                    particle[index].velocityDFSPHfactor.y,
                    particle[index].velocityDFSPHfactor.z);
      if (length(vec_form_center) >= spaceLimiter)
      {
        vec3 normal = -vec_form_center / length(vec_form_center);
        float Vn = dot(vec3(particle[index].velocityDFSPHfactor.x,
                            particle[index].velocityDFSPHfactor.y,
                            particle[index].velocityDFSPHfactor.z),
                       normal);
        particle[index].velocityDFSPHfactor.x +=
            -(boundsViscosity * 2 * Vn * normal).x;
        particle[index].velocityDFSPHfactor.y +=
            -(boundsViscosity * 2 * Vn * normal).y;
        particle[index].velocityDFSPHfactor.z +=
            -(boundsViscosity * 2 * Vn * normal).z;
        //			vec3 normal = - vec3(1);
        //			particle[index].velocityDFSPHfactor.xyz *=
        // boundsViscosity * normal;

        particle[index].position.x *=
            spaceLimiter / length(vec_form_center);
        particle[index].position.y *=
            spaceLimiter / length(vec_form_center);
        particle[index].position.z *=
            spaceLimiter / length(vec_form_center);
      }
      particle[index].position.x +=
          dt * particle[index].velocityDFSPHfactor.x;
      particle[index].position.y +=
          dt * particle[index].velocityDFSPHfactor.y;
      particle[index].position.z +=
          dt * particle[index].velocityDFSPHfactor.z;
    }
    //---------------------------------Calc divergence free
    // solver--------------------------------------
    for (uint index = 0; index < MaxParticles; index++)
    {
      FindNeighbours(index, MaxParticles);
      particle[index].VolumeDensityPressureRohash.y = CalculateDensity(index);
      particle[index].velocityDFSPHfactor.w = CalculateDFSPHFactor(index);
      particle[index].position.w =
          abs(CalculateAvgDensity(index) - density0);
    }

    for (int i = 0; i < 100 && (abs(CalculateAvgDerivDensity(0)) > 1e-1); i++)
    {
      for (uint index = 0; index < MaxParticles; index++)
      {
        particle[index].VolumeDensityPressureRohash.z =
            CalculateDerivDensity(index) *
            particle[index].velocityDFSPHfactor.w / dt;
      }
      for (uint index = 0; index < MaxParticles; index++)
      {
        vec3 gradP = CalculateGradPressure(index);
        particle[index].velocityDFSPHfactor.x -= dt * gradP.x * 1e-4;
        particle[index].velocityDFSPHfactor.y -= dt * gradP.y * 1e-4;
        particle[index].velocityDFSPHfactor.z -= dt * gradP.z * 1e-4;
      }
    }
  }
};
