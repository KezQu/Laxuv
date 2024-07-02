#pragma once

#include <functional>

#include <Shape.h>
#include <Entity.h>

class Particles : public Entity {
private:
	Shape _particleShape;
	std::size_t _count;
	enum DistributionType{
		PLANE = 0,
		LINE = 1,
		QUBE = 2,
		SPHERE = 3
	} ;
public:
	Particles(Shape&& particleShape, std::size_t count);
	void Draw() const override;
	void Bind() const;
};