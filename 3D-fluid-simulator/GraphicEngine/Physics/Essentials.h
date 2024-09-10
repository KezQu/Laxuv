#pragma once
#include <glm/vec4.hpp>
#include <iostream>
#include <cstdint>

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
|      Force									   |    XXXXXXXXXXX   |
|      Velocity									   |    XXXXXXXXXXX   |
|      Position									   |    XXXXXXXXXXX   |
|      Volume   |     Density   |     Pressure     |       Mass       |
*/
struct PhysicsProperties {
	glm::vec4 force = glm::vec4(0, 0, 0, 0);
	glm::vec4 velocity = glm::vec4(0, 0, 0, 0);
	glm::vec4 position = glm::vec4(0, 0, 0, 0);
	glm::vec4 VolumeDensityPressureMass = glm::vec4(1, 1, 1, 1);
	uint32_t neighbours[MaxNeighbours] = { 0xffffffff };
};

std::ostream& operator<<(std::ostream& out, PhysicsProperties const& particle);