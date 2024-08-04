#pragma once

#include <Entity.h>
#include <Shape.h>
#include <Debug.h>

template<GLenum Prim>
class Object : public Entity {
private:
	std::unique_ptr<Shape<Prim>> _shape;
public:
	Object(Shape<Prim> * const shape, PhysicsType physics = PhysicsType::STATIC);
	Object(Object const& obj_copy) = delete;
	Object(Object&& obj_move) = default;
	Object& operator=(Object const& obj_copy) = delete;
	Object& operator=(Object&& obj_move) = default;
	~Object() override = default;
	void Initialize() override;
	void Calculate() override;
	void Draw() const override;
	void UpdateBoundingDimensions();
	details_map Details() override;
};

template<GLenum Prim>
Object<Prim>::Object(Shape<Prim> * const shape, PhysicsType physics)
	: Entity(physics),
	_shape{ std::unique_ptr<Shape<Prim>>(shape) }
{
	Initialize();
}

template<GLenum Prim>
void Object<Prim>::Initialize()
{
	UpdateBoundingDimensions();
	_physicsDispatch.InitDefaultShape(_shape->GetParticleDistribution());
}
template<GLenum Prim>
void Object<Prim>::Calculate()
{
	_physicsDispatch.GenerateForces();
}
template<GLenum Prim>
void Object<Prim>::Draw() const
{
	if (_visible) {
		_shape->Bind();
		_(glDrawElements(_shape->GetDrawPrimitive(), _shape->GetVA().Size(), _shape->GetVA().IndexBufferType(), nullptr));
	}
}

template<GLenum Prim>
inline void Object<Prim>::UpdateBoundingDimensions() {
	glm::vec4 initialDimensions = glm::vec4(glm::vec3(_shape->GetRadius()), 0);
	glm::vec4 correctedDimensions = _shape->Model() * initialDimensions;
	glm::ivec3 newMeshDimensions{ static_cast<int32_t>(correctedDimensions.x), 
		static_cast<int32_t>(correctedDimensions.y), 
		static_cast<int32_t>(correctedDimensions.z) };

	_physicsDispatch.UpdateMeshDimensions(newMeshDimensions);
}
template<GLenum Prim>
Object<Prim>::details_map Object<Prim>::Details()
{
	details_map details = Entity::Details();
	details.push_back({ "Location", { [=]() {return std::ref(this->_shape->GetLocation()); }, DetailsType::VEC3 } });
	details.push_back({ "Rotation", { [=]() {return std::ref(this->_shape->GetRotate()); }, DetailsType::VEC3 } });
	details.push_back({ "Scale", { [=]() {return std::ref(this->_shape->GetScale()); }, DetailsType::VEC3 } });
	details.push_back({ "Light", { [=]() {return std::ref(this->_shape->EnableLight()); }, DetailsType::BOOL } });
	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_shape->GetSubdivision()); }, DetailsType::UINT64 } });
	details.push_back({ "Radius", { [=]() {return std::ref(this->_shape->GetRadius()); }, DetailsType::UINT64 } });
	return details;
}
