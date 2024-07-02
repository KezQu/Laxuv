#pragma once

#include <Element.h>
#include <Entity.h>

class Point : public Element<GL_POINTS>, Entity {
public:
	Point(Vertex center, float radius);
	//Point(Point const& obj_copy) = delete;
	//Point(Point&& obj_move) = default;
	virtual ~Point();
	void Draw() const override;
};
