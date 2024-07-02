#pragma once

#include <Entity.h>
#include <Shape.h>

class Object : public Entity {
private:
	Shape _shape;
public:
	Object(Shape&& shape);
	void Draw() const override;
	Shape& GetShape();
};
