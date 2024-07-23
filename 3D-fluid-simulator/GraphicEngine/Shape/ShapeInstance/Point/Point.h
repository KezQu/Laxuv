#pragma once

#include <Shape.h>
#include <Entity.h>

class Point : public Shape<GL_POINTS>{
public:
	Point(Vertex center = { {0,0,0} }, float radius = 150);
	Point(Point const& obj_copy) = delete;
	Point(Point&& obj_move) = default;
	Point& operator=(Point const& obj_copy) = delete;
	Point& operator=(Point&& obj_move) = default;
	virtual ~Point();
};
