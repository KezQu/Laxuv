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
  Object(Shape<Prim>* const shape);
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
Object<Prim>::Object(Shape<Prim>* const shape)
    : Entity{Essentials::PhysicsType::STATIC, {1, 1, 1}},
      _shape{std::unique_ptr<Shape<Prim>>(shape)}
{
  Initialize();
}

template <GLenum Prim>
void Object<Prim>::Initialize()
{
  _physicsDispatch.InitDefaultShape(GetPhysicsType(),
                                    _shape->GetShapeProperties());
}
template <GLenum Prim>
void Object<Prim>::Calculate()
{
  if (_visible)
  {
    _physicsDispatch.GenerateForces(this->_shape->GetShapeProperties(),
                                    GetPhysicsType());
  }
}
template <GLenum Prim>
void Object<Prim>::Draw() const
{
  if (_visible)
  {
    _shape->Bind();
    uint32_t programID =
        _shape->GetTesselation()
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
  auto& shape_properties = this->_shape->GetShapeProperties();

  details.push_back({"Location", shape_properties._location.ExposeToUI()});
  details.push_back({"Rotation", shape_properties._rotation.ExposeToUI()});
  details.push_back({"Scale", shape_properties._scale.ExposeToUI()});
  details.push_back(
      {"Subdivision", shape_properties._subdivision.ExposeToUI()});
  details.push_back({"Radius", shape_properties._radius.ExposeToUI()});
  return details;
}
