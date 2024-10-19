#include <Simulator.h>

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
  return _globalSimulationState;
}
void Simulator::UpdateDeltaTime()
{
  _global_delta_time = 1 / ImGui::GetIO().Framerate;
}
void Simulator::UpdateDeltaTime(float dt)
{
  _global_delta_time = dt;
}

Uniform<float>& Simulator::GetDeltaTime()
{
  return _global_delta_time;
}

details::details_map Simulator::GetDetails()
{
  details::details_map details;
  details.push_back(
      {"Space bounds",
       {[this]() { return std::ref(this->_space_boundries.GetValue()); },
        details::DetailsType::FLOAT}});
  details.push_back(
      {"Bounds viscosity",
       {[this]() { return std::ref(this->_bounds_viscosity.GetValue()); },
        details::DetailsType::FLOAT}});
  return details;
}

void Simulator::BindUniforms(uint32_t program_id)
{
  _global_delta_time.MapUniform(program_id);
  _space_boundries.MapUniform(program_id);
  _bounds_viscosity.MapUniform(program_id);
}

void Simulator::SetSimulationState(Essentials::SimulationState newGlobalState)
{
  _globalSimulationState = newGlobalState;
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
}
