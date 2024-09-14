#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <Essentials.h>


struct Vector {
	float x{};
	glm::vec3 y{ 0 };
	float z{};
};
struct GradVector {
	glm::vec3 x{ 0 };
	glm::mat3 y{ 0 };
	glm::vec3 z{ 0 };
};

struct HydroTest {
	const uint32_t maxParticles = 64;

	const float E = 2.71828;
	const float PI = 3.14159265358979323846264338327950288;
	const float gamma = 0.6f;

	std::vector<Essentials::PhysicsProperties> particle{ maxParticles };


	std::vector<Essentials::PhysicsProperties> Init() {
		for (int i = 0; i < maxParticles; i++)
		{
			particle[i].position = Qube(i, maxParticles);
			particle[i].velocity = glm::vec4(i % 10, -i % 10, i % 5, 0);
		}
		for (int i = 0; i < maxParticles; i++)
		{
			FindNeighbours(i, maxParticles);
		}
		for (int i = 0; i < maxParticles; i++)
		{
			particle[i].VolumeDensityPressureMass.w = 1;
			particle[i].VolumeDensityPressureMass.x = 1. / CalculateOmega(i);
			particle[i].VolumeDensityPressureMass.y = particle[i].VolumeDensityPressureMass.w / particle[i].VolumeDensityPressureMass.x;
			particle[i].VolumeDensityPressureMass.z = particle[i].VolumeDensityPressureMass.w * glm::pow(glm::length(particle[i].velocity), 2) * 0.5 / particle[i].VolumeDensityPressureMass.x;
		}
		return particle;
	}

	glm::vec4 Qube(uint32_t idx, uint32_t maxParticles) {
		float meshSize = glm::pow(maxParticles, 1 / 3.);
		return glm::vec4(
			4 * glm::mod(glm::floor(idx / meshSize), meshSize),
			4 * glm::mod(float(idx), meshSize),
			4 * glm::floor(idx / (meshSize * meshSize)),
			0.f);
	}

	glm::mat3 GetRotationZMatrix(float alpha_z);
	glm::mat3 GetRotationYMatrix(float alpha_y);

	float CalculateKernelWeight(glm::vec3 x);
	glm::vec3 CalculateFrameVelocity(uint32_t index_i, uint32_t index_j);
	void FindNeighbours(uint32_t index, uint32_t MaxParticles);

	float CalculateOmega(uint32_t index_x);

	float CalculatePsi(uint32_t index_i, uint32_t index_x);
	glm::vec3 CalculateNearPsi(uint32_t index_i, uint32_t index_x);
	GradVector CalculateGradW(uint32_t index_x, Vector Wp);

	glm::mat3 ConvertToUpper(glm::mat3 mtx);
	Vector CalculateTimeDerivativeOfW(uint32_t index_x, Vector Wp);
	Vector PrepareRiemmanProblemSide(uint32_t index_x, glm::vec3 v_frame, glm::vec3 x_mean_x, glm::vec3* dir_versor);
	Vector CalculateReimannProblem(uint32_t index_i, uint32_t index_j);
	
	Vector GenerateHydrodynamics(uint32_t index_i);
};
