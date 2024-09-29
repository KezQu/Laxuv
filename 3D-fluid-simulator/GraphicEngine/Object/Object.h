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
  details_map Details() override;
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
  _physicsDispatch.GenerateForces(GetPhysicsType());
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
Object<Prim>::details_map Object<Prim>::Details()
{
  details_map details = Entity::Details();
  details.push_back(
      {"Location",
       {[this]() { return std::ref(this->_shape->GetLocation()); },
        DetailsType::VEC3}});
  details.push_back({"Rotation",
                     {[this]() { return std::ref(this->_shape->GetRotate()); },
                      DetailsType::VEC3}});
  details.push_back({"Scale",
                     {[this]() { return std::ref(this->_shape->GetScale()); },
                      DetailsType::VEC3}});
  details.push_back(
      {"Light",
       {[this]() { return std::ref(this->_shape->EnableLight()); },
        DetailsType::BOOL}});
  details.push_back(
      {"Subdivision",
       {[this]()
        { return std::ref(this->_shape->GetSubdivision().GetValue()); },
        DetailsType::UINT32}});
  details.push_back(
      {"Radius",
       {[this]() { return std::ref(this->_shape->GetRadius().GetValue()); },
        DetailsType::UINT32}});
  details.push_back({"Physics type",
                     {[this]() { return std::ref(this->GetPhysicsType()); },
                      DetailsType::PHYSTYPE}});
  return details;
}
