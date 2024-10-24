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
                           uint32_t _meshRadius)
    : Entity{Essentials::PhysicsType::DYNAMIC},
      _particleShape{std::unique_ptr<Shape<Prim>>(particleShape)}
{
  Initialize();
}

template <GLenum Prim>
void Particles<Prim>::Initialize()
{
  _physicsDispatch.InitDefaultShape(GetPhysicsType(),
                                    _particleShape->GetRadius() * 2);
}

template <GLenum Prim>
void Particles<Prim>::Calculate()
{
  if (_visible)
  {
    _physicsDispatch.GenerateForces(_particleShape->Model(), GetPhysicsType());
  }
}

template <GLenum Prim>
void Particles<Prim>::Draw() const
{
  if (_visible)
  {
    _particleShape->Bind();
    uint32_t programID =
        _particleShape->EnableTesselation()
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
  details.push_back(
      {"Location", this->_particleShape->GetLocation().ExposeToUI()});
  details.push_back(
      {"Rotation", this->_particleShape->GetRotate().ExposeToUI()});
  details.push_back({"Scale", this->_particleShape->GetScale().ExposeToUI()});
  details.push_back(
      {"Subdivision", this->_particleShape->GetSubdivision().ExposeToUI()});
  details.push_back(
      {"Particle color", [this]()
       {
         ImGui::Combo("##Particle_color",
                      (int32_t*)&this->_physicsDispatch.GetFluidProperties()
                          .particle_color.first.GetValue(),
                      Essentials::ColorPropertyTolist());
       }});
  details.push_back(
      {"Color opacity", this->_physicsDispatch.GetFluidProperties()
                            .particle_color.first.ExposeToUI()});
  details.push_back({"Radius", this->_particleShape->GetRadius().ExposeToUI()});
  details.push_back(
      {"Particle spacing", this->_physicsDispatch.GetFluidProperties()
                               .particle_spacing.ExposeToUI()});
  details.push_back({"Distribution", this->_physicsDispatch.GetFluidProperties()
                                         .distribution_shape.ExposeToUI()});
  details.push_back(
      {"Influence kernel", this->_physicsDispatch.GetFluidProperties()
                               .influence_kernel.ExposeToUI()});
  details.push_back(
      {"Search kernel",
       this->_physicsDispatch.GetFluidProperties().search_kernel.ExposeToUI()});
  details.push_back(
      {"Kernel factor A",
       this->_physicsDispatch.GetFluidProperties().kernel_a.ExposeToUI()});
  details.push_back(
      {"Mass", this->_physicsDispatch.GetFluidProperties().mass.ExposeToUI()});
  details.push_back(
      {"Viscosity factor", this->_physicsDispatch.GetFluidProperties()
                               .viscosity_factor.ExposeToUI()});
  details.push_back(
      {"Mesh radius", [this]()
       {
         ImGui::DragInt(
             "##Mesh_radius",
             (int32_t*)&this->_physicsDispatch.GetFluidProperties().mesh_radius,
             1.f, 0, 100, "%d", ImGuiSliderFlags_AlwaysClamp);
       }});
  return details;
}
