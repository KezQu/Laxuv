#include "Shape.h"

Shape::Shape(VertexArray&& vertexArray, bool enableTess)
	:Element(std::move(vertexArray), enableTess)
{}

Shape::~Shape() 
{}
