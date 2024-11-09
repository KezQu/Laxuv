#pragma once

#include <Entity.h>
#include <Essentials.h>
#include <Program.h>
#include <Shape.h>

template <GLenum Prim>
class Particles : public Entity
{
 private:
  std::unique_ptr<Shape<Prim>> _particleShape;
  Essentials::ParticleProperties _particle_properties;

 public:
  Particles(Shape<Prim>* const particleShape, glm::uvec3 mesh_size);
  Particles(Particles<Prim> const& obj_copy) = delete;
  Particles(Particles<Prim>&& obj_move) = default;
  Particles& operator=(Particles<Prim> const& obj_copy) = delete;
  Particles& operator=(Particles<Prim>&& obj_move) = default;
  ~Particles() override = default;
  void Initialize() override;
  void Calculate() override;
  void Draw() const override;
  void Bind(uint32_t program_id) const override;
  details::detail_controls_t Details() override;
};

template <GLenum Prim>
Particles<Prim>::Particles(Shape<Prim>* const particleShape,
                           glm::uvec3 mesh_size)
    : Entity{Essentials::PhysicsType::DYNAMIC, mesh_size},
      _particleShape{std::unique_ptr<Shape<Prim>>(particleShape)}
{
  Initialize();
}

template <GLenum Prim>
void Particles<Prim>::Initialize()
{
  _physicsDispatch.Bind();
  auto program_id = _physicsDispatch.GetProgram().ID();

  _particleShape->BindUniforms(program_id);
  Simulator::GetInstance()->BindUniforms(program_id);
  Bind(program_id);
  _physicsDispatch.InitDefaultShape(_mesh_size);
}

template <GLenum Prim>
void Particles<Prim>::Calculate()
{
  if (_visible)
  {
    _physicsDispatch.Bind();
    auto program_id = _physicsDispatch.GetProgram().ID();

    _particleShape->BindUniforms(program_id);
    Simulator::GetInstance()->BindUniforms(program_id);
    Simulator::GetInstance()->BindTerrain(program_id);
    Bind(program_id);
    _physicsDispatch.Calculate(_mesh_size, true);
  }
}

template <GLenum Prim>
void Particles<Prim>::Draw() const
{
  if (_visible)
  {
    Program& renderer =
        _particleShape->GetTesselation() == true
            ? ProgramDispatch::GetInstance()->GetTesselationPipeline()
            : ProgramDispatch::GetInstance()->GetSimplePipeline();

    if (!renderer.isLinked()) renderer.Link();
    renderer.Bind();

    Bind(renderer.ID());
    _particleShape->Bind(renderer.ID());
    Simulator::GetInstance()->BindUniforms(renderer.ID());
    _physicsDispatch.GetParticleMeshBuffer().Bind(renderer.ID());
    _(glDrawElementsInstanced(
        _particleShape->GetDrawPrimitive(), _particleShape->GetVA().Size(),
        _particleShape->GetVA().IndexBufferType(), nullptr,
        _physicsDispatch.GetParticleMeshBuffer().Size()));
  }
}

template <GLenum Prim>
void Particles<Prim>::Bind(uint32_t program_id) const
{
  Entity::Bind(program_id);

  _particle_properties.viscosity_factor.MapUniform(program_id);
  _particle_properties.mass.MapUniform(program_id);

  _particle_properties.kernel_a.MapUniform(program_id);
  _particle_properties.influence_kernel.MapUniform(program_id);
  _particle_properties.search_kernel.MapUniform(program_id);

  _particle_properties.init_velocity.MapUniform(program_id);
  _particle_properties.particle_spacing.MapUniform(program_id);
  _particle_properties.distribution_shape.MapUniform(program_id);
}

template <GLenum Prim>
details::detail_controls_t Particles<Prim>::Details()
{
  auto details = Entity::Details();
  auto& shape_properties = this->_particleShape->GetShapeProperties();
  auto const& ui_callback = [this]()
  {
    auto old_sim_state_saved = Simulator::GetInstance()->GetSimulationState();
    Simulator::GetInstance()->SetSimulationState(
        Essentials::SimulationState::INIT);
    Initialize();
    Simulator::GetInstance()->SetSimulationState(old_sim_state_saved);
  };

  details.push_back({"Location", shape_properties._location.ExposeToUI()});
  details.push_back({"Rotation", shape_properties._rotation.ExposeToUI()});
  details.push_back({"Scale", shape_properties._scale.ExposeToUI()});
  details.push_back(
      {"Subdivision", shape_properties._subdivision.ExposeToUI()});
  details.push_back(
      {"Color type", [ui_callback, &shape_properties]()
       {
         if (ImGui::Combo("##Color_type",
                          (int32_t*)&shape_properties._color.first.GetValue(),
                          Essentials::ColorPropertyTolist()))
         {
           ui_callback();
         }
       }});
  details.push_back(
      {"Color", shape_properties._color.second.ExposeToUI(ui_callback)});
  details.push_back({"Radius", shape_properties._radius.ExposeToUI()});
  details.push_back(
      {"Initial velocity",
       _particle_properties.init_velocity.ExposeToUI(ui_callback)});
  details.push_back(
      {"Particle spacing",
       _particle_properties.particle_spacing.ExposeToUI(ui_callback)});
  details.push_back(
      {"Distribution",
       _particle_properties.distribution_shape.ExposeToUI(ui_callback)});
  details.push_back(
      {"Influence kernel", _particle_properties.influence_kernel.ExposeToUI()});
  details.push_back(
      {"Search kernel", _particle_properties.search_kernel.ExposeToUI()});
  details.push_back(
      {"Kernel factor A", _particle_properties.kernel_a.ExposeToUI()});
  details.push_back({"Mass", _particle_properties.mass.ExposeToUI()});
  details.push_back(
      {"Viscosity factor", _particle_properties.viscosity_factor.ExposeToUI()});
  details.push_back(
      {"Mesh radius", [this, ui_callback]()
       {
         if (ImGui::DragInt3("##Mesh_radius", (int32_t*)&this->_mesh_size, 1.f,
                             0, 100, "%d", ImGuiSliderFlags_AlwaysClamp))
         {
           ui_callback();
         }
       }});
  return details;
}
