#include "Sphere.h"

Sphere::Sphere()
	:Sphere({ { 0,0,0 } }, 150)
{}

Sphere::Sphere(Vertex center, float radius)
	:Object(Program({
		{ GL_VERTEX_SHADER, "/shaders/Object.vert" },
		{ GL_VERTEX_SHADER, "/shaders/CalculateNDC.vert" },
		{ GL_TESS_CONTROL_SHADER, "/shaders/Object.tesc" },
		{ GL_TESS_EVALUATION_SHADER, "/shaders/Object.tese" },
		{ GL_FRAGMENT_SHADER, "/shaders/Object.frag" } }), 
		VertexArray({
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ 0, glm::golden_ratio<float>(), -1 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ 0, glm::golden_ratio<float>(), 1 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ -glm::golden_ratio<float>(), 1, 0 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ glm::golden_ratio<float>(), 1, 0 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ -1, 0, -glm::golden_ratio<float>() } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ 1, 0, -glm::golden_ratio<float>() } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ -1, 0, glm::golden_ratio<float>() } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ 1, 0, glm::golden_ratio<float>() } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ -glm::golden_ratio<float>(), -1, 0 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ glm::golden_ratio<float>(), -1, 0 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ 0, -glm::golden_ratio<float>(), -1 } },
			{ (center.coordinate) + (radius / 2.f) * glm::vec3{ 0, -glm::golden_ratio<float>(), 1 } },
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
			1,7,6,1,3,7,
			3,9,7,3,5,9,
			5,10,9,5,4,10,
			4,8,10,4,2,8,
			2,6,8,2,1,6}))
{}
