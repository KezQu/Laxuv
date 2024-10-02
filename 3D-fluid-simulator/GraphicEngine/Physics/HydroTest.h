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
//
// struct HydroTest
//{
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
//       particle[i].position = Qube(i, Essentials::MaxParticles);
//       particle[i].velocity = glm::vec4(0);
//       // particle[i].velocity = glm::vec4((i % 10), (-i % 7), (i % 5), 0)
//       // / 10.f; particle[i].velocity = glm::vec4(1.f / ((i % 3) + 1));
//     }
//     for (int i = 0; i < Essentials::MaxParticles; i++)
//     {
//       FindNeighbours(i, Essentials::MaxParticles);
//     }
//     for (int i = 0; i < Essentials::MaxParticles; i++)
//     {
//       const float volume = 1. / CalculateOmega(i);
//       const float density = 997.f;
//       int n = 0;
//       while (particle[i].neighbours[n] != 0xffff) n++;
//       const float pressure = R * (n + 1) * 273.f / volume;
//       particle[i].VolumeDensityPressureMass.x = volume;
//       particle[i].VolumeDensityPressureMass.y = density;
//       particle[i].VolumeDensityPressureMass.z = 25000.f;
//       particle[i].VolumeDensityPressureMass.w = density * volume;
//       // particle[i].VolumeDensityPressureMass.z =
//       // particle[i].VolumeDensityPressureMass.w *
//       // glm::pow(glm::length(particle[i].velocity), 2) * 0.5 /
//       // particle[i].VolumeDensityPressureMass.x;
//     }
//     return particle;
//   }
//
//   glm::vec4 Qube(uint32_t idx, uint32_t maxParticles)
//   {
//     float meshSize = glm::pow(Essentials::MaxParticles, 1 / 3.);
//     return 3.f *
//            glm::vec4(glm::mod(glm::floor(idx / meshSize), meshSize),
//                      glm::mod(float(idx), meshSize),
//                      glm::floor(idx / (meshSize * meshSize)), 0.f) *
//            2.f;
//   }
//
//   glm::mat3 GetRotationZMatrix(float alpha_z);
//   glm::mat3 GetRotationYMatrix(float alpha_y);
//
//   float CalculateKernelWeight(glm::vec3 x);
//   glm::vec3 CalculateFrameVelocity(uint32_t index_i, uint32_t index_j);
//   void FindNeighbours(uint32_t index, uint32_t MaxParticles);
//
//   float CalculateOmega(uint32_t index_x);
//
//   float CalculatePsi(uint32_t index_i, uint32_t index_x);
//   glm::vec3 CalculateNearPsi(uint32_t index_i, uint32_t index_x);
//   glm::mat3 CalculateGradW(uint32_t index_x, Vector Wp);
//
//   glm::mat3 ConvertToUpper(glm::mat3 mtx);
//   Vector CalculateTimeDerivativeOfW(uint32_t index_x, Vector Wp);
//   Vector PrepareRiemmanProblemSide(uint32_t index_x, glm::vec3 v_frame,
//                                    glm::vec3 x_mean_x, glm::vec3*
//                                    dir_versor);
//   Flux CalculateReimannProblem(uint32_t index_i, uint32_t index_j);
//
//   Vector GenerateHydrodynamics(uint32_t index_i);
// };
