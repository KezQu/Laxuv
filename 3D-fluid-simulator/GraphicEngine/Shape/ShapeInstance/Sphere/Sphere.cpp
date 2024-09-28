#include "Sphere.h"

Sphere::Sphere(Vertex center, uint64_t radius, bool enableTess)
	:Shape(VertexArray({
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ 0, glm::golden_ratio<float>(), -1 } , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ 0, glm::golden_ratio<float>(), 1 }  , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ -glm::golden_ratio<float>(), 1, 0 } , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ glm::golden_ratio<float>(), 1, 0 }  , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ -1, 0, -glm::golden_ratio<float>() }, center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ 1, 0, -glm::golden_ratio<float>() } , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ -1, 0, glm::golden_ratio<float>() } , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ 1, 0, glm::golden_ratio<float>() }  , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ -glm::golden_ratio<float>(), -1, 0 }, center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ glm::golden_ratio<float>(), -1, 0 } , center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ 0, -glm::golden_ratio<float>(), -1 }, center.color },
			{ (center.coordinate) + (1.f / radiusScaler) * glm::vec3{ 0, -glm::golden_ratio<float>(), 1 } , center.color },
			},
			{ 0,1,2,
			  0,2,4,
			  0,4,5,
			  0,5,3,
			  0,3,1,
			  11,10,8,
			  11,8,6,
			  11,6,7,
			  11,7,9,
			  11,9,10,
			7,6,1,
			3,7,1,
			9,7,3,
			5,9,3,
			10,9,5,
			4,10,5,
			8,10,4,
			2,8,4,
			6,8,2,
			1,6,2 }), radius, enableTess)
{
	auto& coordBufferData = _vertexArray.Data().coordinateBuffer.Data();
}

Essentials::DistributionShape Sphere::GetParticleDistribution()
{
	return Essentials::DistributionShape::SPHERE;
}
