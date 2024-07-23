#pragma once

#include <Entity.h>
#include <Shape.h>

template<GLenum Prim>
class Object : public Entity {
private:
	Shape<Prim> _shape;
public:
	Object(Shape<Prim>&& shape);
	Object(Object const& obj_copy) = delete;
	Object(Object&& obj_move) = default;
	Object& operator=(Object const& obj_copy) = delete;
	Object& operator=(Object&& obj_move) = default;
	~Object() override = default;
	void Draw() const override;
	Shape<Prim>& GetShape();
	details_map Details() override;
};

template<GLenum Prim>
Object<Prim>::Object(Shape<Prim>&& shape)
	: _shape{ std::exchange(shape, Shape<Prim>()) }
{}

template<GLenum Prim>
void Object<Prim>::Draw() const
{
	if (_visible) {
		_shape.Bind();
		glDrawElements(_shape.GetDrawPrimitive(), _shape.GetVA().Size(), _shape.GetVA().IndexBufferType(), nullptr);
	}
}

template<GLenum Prim>
Shape<Prim>& Object<Prim>::GetShape() {
	return _shape;
}

template<GLenum Prim>
Object<Prim>::details_map Object<Prim>::Details()
{
	details_map details = Entity::Details();
	details.push_back({ "Location", { [=]() {return std::ref(this->_shape.Move()); }, DetailsType::VEC3 } });
	details.push_back({ "Rotation", { [=]() {return std::ref(this->_shape.Rotate()); }, DetailsType::VEC3 } });
	details.push_back({ "Scale", { [=]() {return std::ref(this->_shape.Scale()); }, DetailsType::VEC3 } });
	details.push_back({ "Light", { [=]() {return std::ref(this->_shape.EnableLight()); }, DetailsType::BOOL } });
	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_shape.ChangeSubdivision()); }, DetailsType::INT } });
	details.push_back({ "Radius", { [=]() {return std::ref(this->_shape.ChangeRadius()); }, DetailsType::INT } });
	return details;
}
