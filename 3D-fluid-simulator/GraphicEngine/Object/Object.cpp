//#include <Object.h>
//
//Object::Object(Shape&& shape)
//	:_shape{ std::exchange(shape, Shape()) }
//{}
//
//void Object::Draw() const
//{
//	if (_visible) {
//		_shape.Bind();
//		glDrawElements(_shape.GetDrawPrimitive(), _shape.GetVA().Size(), _shape.GetVA().IndexBufferType(), nullptr);
//	}
//}
//
//Shape& Object::GetShape() {
//	return _shape;
//}
//
//Object::details_map Object::Details()
//{
//	details_map details = Entity::Details();
//	details.push_back({ "Location", { [=]() {return std::ref(this->_shape.Move()); }, DetailsType::VEC3 } });
//	details.push_back({ "Rotation", { [=]() {return std::ref(this->_shape.Rotate()); }, DetailsType::VEC3 } });
//	details.push_back({ "Scale", { [=]() {return std::ref(this->_shape.Scale()); }, DetailsType::VEC3 } });
//	details.push_back({ "Light", { [=]() {return std::ref(this->_shape.EnableLight()); }, DetailsType::BOOL } });
//	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_shape.ChangeSubdivision()); }, DetailsType::INT } });
//	return details;
//}
