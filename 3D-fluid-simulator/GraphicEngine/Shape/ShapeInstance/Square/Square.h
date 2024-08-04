#pragma once

#include <Shape.h>

class Square : public Shape<GL_TRIANGLES> {
public:
	Square(Vertex center = {{0,0,0}}, uint64_t radius = 150);
	Square(Square const& obj_copy) = delete;
	Square(Square&& obj_move) = default;
	Square& operator=(Square const& obj_copy) = delete;
	Square& operator=(Square&& obj_move) = default;
	~Square() override = default;
	DistributionShape GetParticleDistribution() override;
protected:
	Square(Vertex v0, Vertex v1, Vertex v2, Vertex v3, uint64_t radius);
};