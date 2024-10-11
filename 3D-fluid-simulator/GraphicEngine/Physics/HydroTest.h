// #pragma once
//
// #include <Essentials.h>
// #include <imgui.h>
//
// #include <vector>
//
// #include "glm/glm.hpp"
//
// struct Vector
//{
//   float x{};
//   glm::vec3 y{0};
//   float z{};
// };
//
// struct Flux
//{
//   glm::vec3 x{0};
//   glm::mat3 y{0};
//   glm::vec3 z{0};
// };
// using namespace glm;
//
// struct HydroTest
//{
//   using uint = unsigned int;
//
//   const float E = 2.71828;
//   const float PI = 3.14159265358979323846264338327950288;
//   const float R = 8.31446261815324;
//   const float gamma = 1.4f;
//   const float dt = 1.f / ImGui::GetIO().Framerate;
//   const glm::mat3 I = glm::mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
//   const uint16_t MaxValueNeighbour = uint16_t(0xffff);
//
//   std::vector<Essentials::ParticleProperties> particle{
//       Essentials::MaxParticles};
//
//   std::vector<Essentials::ParticleProperties> Init()
//   {
//     for (int i = 0; i < Essentials::MaxParticles; i++)
//     {
//       particle[i].positionGroup = vec4(0);
//       particle[i].velocityDFSPHfactor = glm::vec4(0);
//       // particle[i].velocity = glm::vec4((i % 10), (-i % 7), (i % 5), 0)
//       // / 10.f; particle[i].velocity = glm::vec4(1.f / ((i % 3) + 1));
//     }
//   }
//   const float E = 2.71828;
//   const float PI = 3.14159265358979323846264338327950288;
//   const float R = 8.31446261815324;
//   const mat3 I = mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
//
//   const uint MaxNeighbours = 512;
//   const uint MaxValueNeighbour = uint(0xffff);
//
//   const float kernel_a = 1;
//   const float kernel_b = 1;
//
//   uint influenceKernel;
//   uint searchKernel;
//   float dt;
//   float viscosityFactor;
//   float density0;
//   float EOSFactor;
//   // uniform float inflKernelSmoother;
//
//   float CalculateKernelWeight(vec3 points_dist)
//   {
//     const float abs_dist = length(points_dist);
//
//     return pow(E, -pow(abs_dist, kernel_a) / pow(influenceKernel, kernel_b));
//   }
//
//   vec3 CalculateDerivKernelWeight(vec3 points_dist)
//   {
//     const float abs_dist = length(points_dist);
//     const vec3 versor_ij = abs_dist > 0 ? normalize(points_dist) : vec3(0);
//
//     return kernel_a * pow(abs_dist, kernel_a - 1) *
//            CalculateKernelWeight(points_dist) * versor_ij /
//            vec3(pow(influenceKernel, kernel_b));
//   }
//
//   void FindNeighbours(uint index_x, uint MaxParticles)
//   {
//     vec3 pos_x = vec3(particle[index_x].positionGroup.x,
//                       particle[index_x].positionGroup.y,
//                       particle[index_x].positionGroup.z);
//     uint num_neighbours = 0;
//     for (uint i = 0; i < MaxParticles && num_neighbours < MaxNeighbours; i++)
//     {
//       //		if(i != index_x){
//       const vec3 pos_neighbour =
//       vec3(particle[num_neighbours].positionGroup.x,
//                                       particle[num_neighbours].positionGroup.y,
//                                       particle[num_neighbours].positionGroup.z);
//       if (length(pos_x - pos_neighbour) < 2 * searchKernel)
//       {
//         particle[index_x].neighbours[num_neighbours] = i;
//         num_neighbours++;
//       }
//       //		}
//     }
//   }
//
//   float CalculateDFSPHFactor(uint index_x)
//   {
//     vec3 sum = vec3(0);
//     float sum_of_squared = 0.f;
//     uint curr_neighbour = 0;
//     for (int i = 0; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float m_j = particle[curr_neighbour].VolumeDensityPressureMass.w;
//       const vec3 x_i = vec3(particle[index_x].positionGroup.x,
//                             particle[index_x].positionGroup.y,
//                             particle[index_x].positionGroup.z);
//
//       const vec3 x_j = vec3(particle[curr_neighbour].positionGroup.x,
//                             particle[curr_neighbour].positionGroup.y,
//                             particle[curr_neighbour].positionGroup.z);
//       const vec3 gradW_ij = CalculateDerivKernelWeight(x_i - x_j);
//       sum += m_j * gradW_ij;
//       sum_of_squared += pow(length(m_j * gradW_ij), 2);
//     }
//     return pow(particle[index_x].VolumeDensityPressureMass.y, 2) /
//            (pow(length(sum), 2) + sum_of_squared + 1e-6);
//   }
//
//   float CalculateDensity(uint index_x)
//   {
//     float kernel_sum = 0;
//     uint curr_neighbour = 0;
//     for (int i = 0; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float m_j = particle[curr_neighbour].VolumeDensityPressureMass.w;
//       const vec3 x_ij = vec3(particle[index_x].positionGroup.x,
//                              particle[index_x].positionGroup.y,
//                              particle[index_x].positionGroup.z) -
//                         vec3(particle[curr_neighbour].positionGroup.x,
//                              particle[curr_neighbour].positionGroup.y,
//                              particle[curr_neighbour].positionGroup.z);
//
//       float test = m_j * CalculateKernelWeight(x_ij);
//       kernel_sum += test > 0 ? test : -100;
//     }
//     return kernel_sum;
//   }
//
//   float CalculateDerivDensity(uint index_x)
//   {
//     const vec3 v_i = vec3(particle[index_x].velocityDFSPHfactor.x,
//                           particle[index_x].velocityDFSPHfactor.y,
//                           particle[index_x].velocityDFSPHfactor.z);
//
//     float kernel_sum = 0.f;
//     uint curr_neighbour = 0;
//     for (int i = 0; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float m_j = particle[curr_neighbour].VolumeDensityPressureMass.w;
//       const vec3 v_j = vec3(particle[curr_neighbour].velocityDFSPHfactor.x,
//                             particle[curr_neighbour].velocityDFSPHfactor.y,
//                             particle[curr_neighbour].velocityDFSPHfactor.z);
//       const vec3 x_ij = particle[curr_neighbour].positionGroup.xyz -
//                         particle[index_x].positionGroup.xyz;
//
//       kernel_sum += m_j * dot((v_i - v_j), CalculateDerivKernelWeight(x_ij));
//     }
//     return kernel_sum;
//   }
//
//   vec3 CalculateGradPressure(uint index_x)
//   {
//     const float ro_i = particle[index_x].VolumeDensityPressureMass.y;
//     const float p_i = particle[index_x].VolumeDensityPressureMass.z;
//
//     vec3 kernel_sum = vec3(0);
//     uint curr_neighbour = 0;
//     for (int i = 0; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float m_j = particle[curr_neighbour].VolumeDensityPressureMass.w;
//       const float ro_j =
//       particle[curr_neighbour].VolumeDensityPressureMass.y; const float p_j =
//       particle[curr_neighbour].VolumeDensityPressureMass.z; const vec3 x_ij =
//       particle[curr_neighbour].positionGroup.xyz -
//                         particle[index_x].positionGroup.xyz;
//
//       kernel_sum += m_j * (p_i / pow(ro_i, 2) + p_j / pow(ro_j, 2)) *
//                     CalculateDerivKernelWeight(x_ij);
//     }
//     return ro_i * kernel_sum;
//   }
//
//   vec3 CalculateViscosity(uint index_x)
//   {
//     const int d = 3;
//     uint curr_neighbour = 0;
//
//     vec3 kernel_sum = vec3(0);
//     for (int i = 0; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float m_j = particle[curr_neighbour].VolumeDensityPressureMass.w;
//       const float ro_j =
//       particle[curr_neighbour].VolumeDensityPressureMass.y; const vec3 v_ij =
//       particle[index_x].velocityDFSPHfactor.xyz -
//                         particle[curr_neighbour].velocityDFSPHfactor.xyz;
//       const vec3 x_ij = particle[index_x].positionGroup.xyz -
//                         particle[curr_neighbour].positionGroup.xyz;
//       kernel_sum += (m_j * dot(v_ij, x_ij) *
//       CalculateDerivKernelWeight(x_ij)) /
//                     (ro_j * (pow(length(x_ij), 2) + 0.01));
//     }
//     return kernel_sum * viscosityFactor * 2 * (d + 2);
//   }
//
//   float CalculateAvgDensity(uint index_x)
//   {
//     float kernel_sum = particle[index_x].VolumeDensityPressureMass.y;
//     uint curr_neighbour = 0;
//     int i = 0;
//     for (; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float ro_j =
//       particle[curr_neighbour].VolumeDensityPressureMass.y; kernel_sum +=
//       ro_j;
//     }
//     return kernel_sum / (i + 1);
//   }
//
//   float CalculateAvgDerivDensity(uint index_x)
//   {
//     float kernel_sum = CalculateDerivDensity(index_x);
//     uint curr_neighbour = 0;
//     int i = 0;
//     for (; i < MaxNeighbours; i++)
//     {
//       curr_neighbour = particle[index_x].neighbours[i];
//       if (curr_neighbour == MaxValueNeighbour)
//       {
//         break;
//       }
//       const float ro_j = CalculateDerivDensity(curr_neighbour);
//       kernel_sum += ro_j;
//     }
//     return kernel_sum / (i + 1);
//   }
// };
