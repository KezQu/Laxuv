#pragma once

#include <glm/vec4.hpp>
#include <iostream>

struct PhysicsProperties {
	glm::vec4 acceleration{};
	glm::vec4 velocity{};
	glm::vec4 position{};
	glm::vec4 mass{};
};

std::ostream& operator<<(std::ostream& out, PhysicsProperties const& particle);