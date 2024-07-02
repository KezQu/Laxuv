#pragma once

#include <Element.h>
#include <Entity.h>

class Line : public Element<GL_LINES> {
public:
	Line(Vertex p1, Vertex p2);
	//Line(Line const& obj_copy) = delete;
	//Line(Line&& obj_move) = default;
	virtual ~Line();
};