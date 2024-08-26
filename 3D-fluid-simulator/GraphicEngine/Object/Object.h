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
	_physicsDispatch.InitDefaultShape(_shape->GetParticleDistribution(), GetPhysicsType(), 1);
}
template<GLenum Prim>
void Object<Prim>::Calculate()
{
	_physicsDispatch.GenerateForces(GetPhysicsType());
}
template<GLenum Prim>
void Object<Prim>::Draw() const
{
	if (_visible) {
		_shape->Bind();
		uint32_t programID = _shape->EnableTesselation() ?
			ProgramDispatch::GetInstance().GetTesselationPipeline().ID() :
			ProgramDispatch::GetInstance().GetSimplePipeline().ID();

		_physicsDispatch.GetParticleMeshBuffer().Bind(programID);
		_(glDrawElements(_shape->GetDrawPrimitive(), _shape->GetVA().Size(), _shape->GetVA().IndexBufferType(), nullptr));
		_physicsDispatch.GetParticleMeshBuffer().Unbind(programID);
	}
}

template<GLenum Prim>
inline void Object<Prim>::UpdateBoundingDimensions() {
	glm::uvec3 newMeshDimensions{ _shape->GetRadius() / 10 };

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
	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_shape->GetSubdivision()); }, DetailsType::UINT32 } });
	details.push_back({ "Radius", { [=]() {return std::ref(this->_shape->GetRadius()); }, DetailsType::UINT32 } });
	details.push_back({ "Physics type", { [=]() {return std::ref(this->GetPhysicsType()); }, DetailsType::PHYSTYPE } });
	return details;
}
