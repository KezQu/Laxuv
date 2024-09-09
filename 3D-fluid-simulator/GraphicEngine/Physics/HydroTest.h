#pragma once

#include "glm/glm.hpp"
#include <PhysicsDispatch.h>


struct Vector {
	float x;
	glm::vec3 y;
	float z;
};
struct GradVector {
	glm::vec3 x;
	glm::mat3 y;
	glm::vec3 z;
};

PhysicsProperties* Init();

glm::vec4 Qube(uint32_t idx, uint32_t maxParticles);

glm::mat3 GetRotationZMatrix(float alpha_z);
glm::mat3 GetRotationYMatrix(float alpha_y);

float CalculateKernelWeight(glm::vec3 x);
glm::vec3 CalculateFrameVelocity(uint32_t index_i, uint32_t index_j);
void FindNeighbours(uint32_t index, uint32_t MaxParticles);

float CalculateOmega(uint32_t index_x);

float CalculatePsi(uint32_t index_i, uint32_t index_x);
glm::vec3 CalculateNearPsi(uint32_t index_i, uint32_t index_x);
GradVector CalculateGradW(uint32_t index_x, Vector Wp);

Vector CalculateTimeDerivativeOfW(uint32_t index_x, Vector Wp);
Vector CalculateReimannProblem(uint32_t index_i, uint32_t index_j);

Vector GenerateHydrodynamics(uint32_t index_i);