#pragma once

#include <Shape.h>
#include <Entity.h>

class Line : public Shape<GL_LINES> {
public:
	Line(Vertex p1 = { {-1,0,0} }, Vertex p2 = { {1,0,0} });
	Line(Line const& obj_copy) = delete;
	Line(Line&& obj_move) = default;
	Line& operator=(Line const& obj_copy) = delete;
	Line& operator=(Line&& obj_move) = default;
	virtual ~Line();
};