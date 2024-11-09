#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "Debug.h"
#include "Entity.h"
#include "Essentials.h"
#include "GL/glew.h"
#include "Program.h"
#include "ProgramDispatch.h"
#include "Shape.h"
#include "Simulator.h"
#include "imgui.h"

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
    : Entity{Essentials::PhysicsType::STATIC},
      _shape{std::unique_ptr<Shape<Prim>>(shape)}
{
  Initialize();
}

template <GLenum Prim>
void Object<Prim>::Initialize()
{
  _physicsDispatch.Bind();
  auto program_id = _physicsDispatch.GetProgram().ID();

  _shape->BindUniforms(program_id);
  Simulator::GetInstance()->BindUniforms(program_id);
  Bind(program_id);
  _physicsDispatch.InitDefaultShape(_mesh_size);
}
template <GLenum Prim>
void Object<Prim>::Calculate()
{
  if (_visible)
  {
    _physicsDispatch.Bind();
    auto program_id = _physicsDispatch.GetProgram().ID();

    _shape->BindUniforms(program_id);
    Simulator::GetInstance()->BindUniforms(program_id);
    Simulator::GetInstance()->BindTerrain(program_id);
    Bind(program_id);
    _physicsDispatch.Calculate(_mesh_size, false);
  }
}
template <GLenum Prim>
void Object<Prim>::Draw() const
{
  if (_visible)
  {
    Program& renderer =
        _shape->GetTesselation() == true
            ? ProgramDispatch::GetInstance()->GetTesselationPipeline()
            : ProgramDispatch::GetInstance()->GetSimplePipeline();

    if (!renderer.isLinked()) renderer.Link();
    renderer.Bind();

    _shape->Bind(renderer.ID());
    Simulator::GetInstance()->BindUniforms(renderer.ID());
    _physicsDispatch.GetParticleMeshBuffer().Bind(renderer.ID());
    _(glDrawElements(_shape->GetDrawPrimitive(), _shape->GetVA().Size(),
                     _shape->GetVA().IndexBufferType(), nullptr));
  }
}

template <GLenum Prim>
details::detail_controls_t Object<Prim>::Details()
{
  auto details = Entity::Details();
  auto& shape_properties = this->_shape->GetShapeProperties();
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
  return details;
}
