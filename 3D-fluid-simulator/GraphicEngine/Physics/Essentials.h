#pragma once
#include <glm/vec4.hpp>
#include <iostream>

enum class DistributionShape :uint8_t {
	UNDEFINED,
	LINE,
	CIRCLE,
	SQUARE,
	DISK,
	QUBE,
	SPHERE
};

enum class SimulationState : uint8_t {
	IDLE,
	INIT,
	SIMULATION
};

/* Physics properties memory layout in bytes
|1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|
|      Force               |   Mass    |
|      Velocity            |XXXXXXXXXXX|
|      Position            |XXXXXXXXXXX|
*/
struct PhysicsProperties {
	glm::vec4 forceMass{};
	glm::vec4 velocity{};
	glm::vec4 position{};
};

std::ostream& operator<<(std::ostream& out, PhysicsProperties const& particle);