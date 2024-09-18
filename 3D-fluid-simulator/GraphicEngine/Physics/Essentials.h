#pragma once

#include <glm/vec4.hpp>
#include <ostream>
#include <cstdint>


namespace Essentials {

	constexpr uint32_t KernelRadius = 4;
	constexpr uint32_t MaxParticles = 27;
	constexpr uint32_t MaxNeighbours = 4 * KernelRadius * KernelRadius * KernelRadius;

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
		SIMULATION,
		GEN_FRAME
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
		uint16_t neighbours[MaxNeighbours];
	};
}

std::ostream& operator<<(std::ostream& out, Essentials::PhysicsProperties const& particle);