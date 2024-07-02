#include <Object.h>

Object::Object(Shape&& shape)
	:_shape{ std::exchange(shape, Shape()) }
{}

void Object::Draw() const
{
	_shape.Bind();
	glDrawElements(_shape.GetDrawPrimitive(), _shape.GetVA().Size(), _shape.GetVA().IndexBufferType(), nullptr);
}

Shape& Object::GetShape() {
	return _shape;
}
