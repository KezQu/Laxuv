#pragma once

#include <glm/vec3.hpp>

struct PhysicsProperties {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	float mass;
};
