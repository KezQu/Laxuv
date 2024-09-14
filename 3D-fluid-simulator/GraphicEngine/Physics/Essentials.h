#pragma once

#include <glm/vec4.hpp>
#include <ostream>
#include <cstdint>


namespace Essentials {

const uint32_t KernelRadius = 1;
const uint32_t MaxNeighbours = KernelRadius * KernelRadius * 4 * 4 * 2;

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

enum class PhysicsType : uint8_t {
	NONE,
	STATIC,
	DYNAMIC
};

/* Physics properties memory layout in bytes
| 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14| 15 | 16 |
|      Force									   |      Energy      |
|      Velocity									   |    XXXXXXXXXXX   |
|      Position									   |    XXXXXXXXXXX   |
|      Volume   |     Density   |     Pressure     |       Mass       |
*/
struct PhysicsProperties {
	glm::vec4 force{ 0 };
	glm::vec4 velocity{ 0 };
	glm::vec4 position{ 0 };
	glm::vec4 VolumeDensityPressureMass{ 0 };
	uint32_t neighbours[MaxNeighbours];
};
}

std::ostream& operator<<(std::ostream& out, Essentials::PhysicsProperties const& particle);