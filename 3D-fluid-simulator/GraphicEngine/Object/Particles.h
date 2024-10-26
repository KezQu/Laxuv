#pragma once

#include <Entity.h>
#include <Shape.h>

#include <functional>

template <GLenum Prim>
class Particles : public Entity
{
 private:
  std::unique_ptr<Shape<Prim>> _particleShape;

 public:
  Particles(Shape<Prim>* const particleShape, uint32_t meshRadius);
  Particles(Particles<Prim> const& obj_copy) = delete;
  Particles(Particles<Prim>&& obj_move) = default;
  Particles& operator=(Particles<Prim> const& obj_copy) = delete;
  Particles& operator=(Particles<Prim>&& obj_move) = default;
  ~Particles() override = default;
  void Initialize() override;
  void Calculate() override;
  void Draw() const override;
  void Bind() const;
  details::detail_controls_t Details() override;
};

template <GLenum Prim>
Particles<Prim>::Particles(Shape<Prim>* const particleShape,
                           uint32_t meshRadius)
    : Entity{Essentials::PhysicsType::DYNAMIC,
             {meshRadius, meshRadius, meshRadius}},
      _particleShape{std::unique_ptr<Shape<Prim>>(particleShape)}
{
  Initialize();
}

template <GLenum Prim>
void Particles<Prim>::Initialize()
{
  _physicsDispatch.InitDefaultShape(GetPhysicsType(),
                                    _particleShape->GetShapeProperties());
}

template <GLenum Prim>
void Particles<Prim>::Calculate()
{
  if (_visible)
  {
    _physicsDispatch.GenerateForces(_particleShape->GetShapeProperties(),
                                    GetPhysicsType());
  }
}

template <GLenum Prim>
void Particles<Prim>::Draw() const
{
  if (_visible)
  {
    _particleShape->Bind();
    uint32_t programID =
        _particleShape->GetTesselation()
            ? ProgramDispatch::GetInstance().GetTesselationPipeline().ID()
            : ProgramDispatch::GetInstance().GetSimplePipeline().ID();

    _physicsDispatch.GetParticleMeshBuffer().Bind(programID);
    _(glDrawElementsInstanced(
        _particleShape->GetDrawPrimitive(), _particleShape->GetVA().Size(),
        _particleShape->GetVA().IndexBufferType(), nullptr,
        _physicsDispatch.GetParticleMeshBuffer().Size()));
    _physicsDispatch.GetParticleMeshBuffer().Unbind(programID);
  }
}

template <GLenum Prim>
details::detail_controls_t Particles<Prim>::Details()
{
  auto details = Entity::Details();
  auto& shape_properties = this->_particleShape->GetShapeProperties();
  auto& fluid_properties = this->_physicsDispatch.GetFluidProperties();

  details.push_back({"Location", shape_properties._location.ExposeToUI()});
  details.push_back({"Rotation", shape_properties._rotation.ExposeToUI()});
  details.push_back({"Scale", shape_properties._scale.ExposeToUI()});
  details.push_back(
      {"Subdivision", shape_properties._subdivision.ExposeToUI()});
  details.push_back({"Particle color", [&shape_properties]()
                     {
                       ImGui::Combo(
                           "##Particle_color",
                           (int32_t*)&shape_properties._color.first.GetValue(),
                           Essentials::ColorPropertyTolist());
                     }});
  details.push_back(
      {"Color opacity", shape_properties._color.second.ExposeToUI()});
  details.push_back({"Radius", shape_properties._radius.ExposeToUI()});
  details.push_back(
      {"Particle spacing", fluid_properties.particle_spacing.ExposeToUI()});
  details.push_back(
      {"Distribution", fluid_properties.distribution_shape.ExposeToUI()});
  details.push_back(
      {"Influence kernel", fluid_properties.influence_kernel.ExposeToUI()});
  details.push_back(
      {"Search kernel", fluid_properties.search_kernel.ExposeToUI()});
  details.push_back(
      {"Kernel factor A", fluid_properties.kernel_a.ExposeToUI()});
  details.push_back({"Mass", fluid_properties.mass.ExposeToUI()});
  details.push_back(
      {"Viscosity factor", fluid_properties.viscosity_factor.ExposeToUI()});
  details.push_back({"Mesh radius", [&fluid_properties]()
                     {
                       ImGui::DragInt("##Mesh_radius",
                                      (int32_t*)&fluid_properties.mesh_radius,
                                      1.f, 0, 100, "%d",
                                      ImGuiSliderFlags_AlwaysClamp);
                     }});
  return details;
}
