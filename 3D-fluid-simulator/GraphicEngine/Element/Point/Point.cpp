#include <Point.h>

Point::Point(Vertex center, float radius)
	:Element(VertexArray({ center }, { 0 }))
{
	_boundingBox.x = _boundingBox.y = _boundingBox.z = radius;
}

Point::~Point()
{}

void Point::Draw() const {
	Bind();
	glDrawElements(GetDrawPrimitive(), GetVA().Size(), GetVA().IndexBufferType(), nullptr);
}
