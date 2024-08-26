#pragma once

#include <functional>

#include <Shape.h>
#include <Entity.h>

template<GLenum Prim>
class Particles : public Entity {
private:
	std::unique_ptr<Shape<Prim>> _particleShape;
	uint32_t _meshRadius;
	DistributionShape _distributionShape{ DistributionShape::QUBE };
public:
	Particles(Shape<Prim> * const particleShape, uint32_t meshRadius);
	Particles(Particles<Prim> const& obj_copy) = delete;
	Particles(Particles<Prim>&& obj_move) = default;
	Particles& operator=(Particles<Prim> const& obj_copy) = delete;
	Particles& operator=(Particles<Prim>&& obj_move) = default;
	~Particles() override = default;
	void Initialize() override;
	void Calculate() override;
	void Draw() const override;
	void Bind() const;
	void UpdateBoundingDimensions();
	details_map Details() override;
	DistributionShape& GetDistributionShape();
	uint32_t& GetMeshRadius();
};

template<GLenum Prim>
Particles<Prim>::Particles(Shape<Prim>* const particleShape, uint32_t _meshRadius)
	:Entity(PhysicsType::DYNAMIC),
	_particleShape{ std::unique_ptr<Shape<Prim>>(particleShape) },
	_meshRadius{ _meshRadius }
{
	Initialize();
}
template<GLenum Prim>
void Particles<Prim>::Initialize()
{
	UpdateBoundingDimensions();
	_physicsDispatch.InitDefaultShape(GetDistributionShape(), GetPhysicsType(), _particleShape->GetRadius() * 2);
}
template<GLenum Prim>
void Particles<Prim>::Calculate()
{
	_physicsDispatch.GenerateForces(GetPhysicsType());
}
template<GLenum Prim>
void Particles<Prim>::Draw() const
{
	if (_visible) {
		glm::ivec3 const& meshDimensions = _physicsDispatch.GetMeshDimensions();
		_particleShape->Bind();
		uint32_t programID = _particleShape->EnableTesselation() ?
			ProgramDispatch::GetInstance().GetTesselationPipeline().ID() :
			ProgramDispatch::GetInstance().GetSimplePipeline().ID();

		_physicsDispatch.GetParticleMeshBuffer().Bind(programID);
		_(glDrawElementsInstanced(_particleShape->GetDrawPrimitive(), _particleShape->GetVA().Size(), _particleShape->GetVA().IndexBufferType(), nullptr, meshDimensions.x * meshDimensions.y * meshDimensions.z));
		_physicsDispatch.GetParticleMeshBuffer().Unbind(programID);
	}
}
template<GLenum Prim>
inline void Particles<Prim>::UpdateBoundingDimensions() {
	_physicsDispatch.UpdateMeshDimensions(glm::uvec3(_meshRadius));
}

template<GLenum Prim>
Particles<Prim>::details_map Particles<Prim>::Details()
{
	details_map details = Entity::Details();
	details.push_back({ "Location", { [=]() {return std::ref(this->_particleShape->GetLocation()); }, DetailsType::VEC3 } });
	details.push_back({ "Rotation", { [=]() {return std::ref(this->_particleShape->GetRotate()); }, DetailsType::VEC3 } });
	details.push_back({ "Scale", { [=]() {return std::ref(this->_particleShape->GetScale()); }, DetailsType::VEC3 } });
	details.push_back({ "Light", { [=]() {return std::ref(this->_particleShape->EnableLight()); }, DetailsType::BOOL } });
	details.push_back({ "Subdivision", { [=]() {return std::ref(this->_particleShape->GetSubdivision()); }, DetailsType::UINT32 } });
	details.push_back({ "Radius", { [=]() {return std::ref(this->_particleShape->GetRadius()); }, DetailsType::UINT32 } });
	details.push_back({ "Distribution shape", { [=]() {return std::ref(this->GetDistributionShape()); }, DetailsType::DISTSHAPE } });
	details.push_back({ "Mesh radius", { [=]() {return std::ref(this->GetMeshRadius()); }, DetailsType::UINT32 } });
	details.push_back({ "Physics type", { [=]() {return std::ref(this->GetPhysicsType()); }, DetailsType::PHYSTYPE } });
	return details;
}

template<GLenum Prim>
DistributionShape& Particles<Prim>::GetDistributionShape()
{
	return _distributionShape;
}

template<GLenum Prim>
uint32_t& Particles<Prim>::GetMeshRadius()
{
	return _meshRadius;
}

