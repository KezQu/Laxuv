#pragma once

#include <Element.h>

class Shape : public Element<GL_TRIANGLES> {
public:
	Shape(VertexArray&& vertexArray = {}, bool enableTess = false);
	Shape(Shape const& obj_copy) = delete;
	Shape(Shape&& obj_move) = default;
	Shape& operator=(Shape const& obj_copy) = delete;
	Shape& operator=(Shape&& obj_move) = default;
	virtual ~Shape();
};