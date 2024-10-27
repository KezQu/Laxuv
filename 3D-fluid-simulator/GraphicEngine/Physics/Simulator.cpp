#include "Simulator.h"

#include "Uniform.h"

Simulator::Simulator() : _terrain{"terrainBuffer"} {}

void Simulator::CleanUp()
{
  _entitiesContainer.clear();
}

Simulator& Simulator::GetInstance()
{
  static Simulator instance{};
  return instance;
}

Simulator::EntityContainer& Simulator::GetEntities()
{
  return _entitiesContainer;
}

Essentials::SimulationState Simulator::GetSimulationState()
{
  return static_cast<Essentials::SimulationState>(
      _global_simulation_state.GetValue());
}

void Simulator::UpdateDeltaTime(float dt)
{
  if (_static_timestep)
  {
    _global_delta_time = dt;
  }
  else
  {
    _global_delta_time = 1000.f / ImGui::GetIO().Framerate;
  }
}

Uniform<float>& Simulator::GetDeltaTime()
{
  return _global_delta_time;
}

bool Simulator::IsStaticDtUsed()
{
  return _static_timestep;
}

void Simulator::ToggleTimesetType()
{
  _static_timestep = !_static_timestep;
}

details::detail_controls_t Simulator::GetDetails()
{
  details::detail_controls_t details;
  details.push_back({"Space bounds", this->_space_boundries.ExposeToUI()});
  details.push_back({"Bounds viscosity", this->_bounds_viscosity.ExposeToUI()});
  details.push_back({"World shape", [this]()
                     {
                       ImGui::Combo(
                           "##World_shape",
                           (int32_t*)&this->_global_world_type.GetValue(),
                           Essentials::WorldTypeTolist());
                     }});
  return details;
}

void Simulator::BindUniforms(uint32_t program_id)
{
  _obstacles_number.MapUniform(program_id);
  _global_delta_time.MapUniform(program_id);
  _space_boundries.MapUniform(program_id);
  _bounds_viscosity.MapUniform(program_id);
  _global_simulation_state.MapUniform(program_id);
  _global_world_type.MapUniform(program_id);
}

void Simulator::BindTerrain(uint32_t program_id)
{
  _terrain.Bind(program_id);
  auto lookupBuffer = _terrain.GetBufferSubData(0U, _terrain.Size());
}

void Simulator::SetSimulationState(Essentials::SimulationState new_global_state)
{
  _global_simulation_state = static_cast<uint32_t>(new_global_state);
}

void Simulator::AddObstacle()
{
  _terrain.SetBufferMemorySize(_terrain.Size() + 1);
  _obstacles_number = _terrain.Size();
}

void Simulator::RemoveObstacle()
{
  _terrain.SetBufferMemorySize(_terrain.Size() - 1);
  _obstacles_number = _terrain.Size();
}

void Simulator::Delete(EntityContainer::key_type id)
{
  if (id != 0)
  {
    _entitiesContainer.erase(id);
  }
  else
  {
    _entitiesContainer.erase(std::prev(_entitiesContainer.end()));
  }
  RemoveObstacle();
}
