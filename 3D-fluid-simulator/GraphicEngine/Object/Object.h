#pragma once

#include <Debug.h>
#include <Entity.h>
#include <Shape.h>

template <GLenum Prim>
class Object : public Entity
{
 private:
  std::unique_ptr<Shape<Prim>> _shape;

 public:
  Object(Shape<Prim>* const shape,
         Essentials::PhysicsType physics = Essentials::PhysicsType::STATIC);
  Object(Object const& obj_copy) = delete;
  Object(Object&& obj_move) = default;
  Object& operator=(Object const& obj_copy) = delete;
  Object& operator=(Object&& obj_move) = default;
  ~Object() override = default;
  void Initialize() override;
  void Calculate() override;
  void Draw() const override;
  details::detail_controls_t Details() override;
};

template <GLenum Prim>
Object<Prim>::Object(Shape<Prim>* const shape, Essentials::PhysicsType physics)
    : Entity(physics), _shape{std::unique_ptr<Shape<Prim>>(shape)}
{
  Initialize();
}

template <GLenum Prim>
void Object<Prim>::Initialize()
{
  // _physicsDispatch.InitDefaultShape(_shape->GetParticleDistribution(),
  //                                   GetPhysicsType(), 1);
}
template <GLenum Prim>
void Object<Prim>::Calculate()
{
  if (_visible)
  {
    _physicsDispatch.GenerateForces(_shape->Model(), GetPhysicsType());
  }
}
template <GLenum Prim>
void Object<Prim>::Draw() const
{
  if (_visible)
  {
    _shape->Bind();
    uint32_t programID =
        _shape->EnableTesselation()
            ? ProgramDispatch::GetInstance().GetTesselationPipeline().ID()
            : ProgramDispatch::GetInstance().GetSimplePipeline().ID();

    _physicsDispatch.GetParticleMeshBuffer().Bind(programID);
    _(glDrawElements(_shape->GetDrawPrimitive(), _shape->GetVA().Size(),
                     _shape->GetVA().IndexBufferType(), nullptr));
    _physicsDispatch.GetParticleMeshBuffer().Unbind(programID);
  }
}

template <GLenum Prim>
details::detail_controls_t Object<Prim>::Details()
{
  auto details = Entity::Details();
  details.push_back({"Location", this->_shape->GetLocation().ExposeToUI()});
  details.push_back({"Rotation", this->_shape->GetRotate().ExposeToUI()});
  details.push_back({"Scale", this->_shape->GetScale().ExposeToUI()});
  details.push_back(
      {"Subdivision", this->_shape->GetSubdivision().ExposeToUI()});
  details.push_back({"Radius", this->_shape->GetRadius().ExposeToUI()});
  return details;
}
