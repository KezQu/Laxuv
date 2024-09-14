#include <Simulator.h>

void Simulator::CleanUp() {
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
void Simulator::SetSimulationState(Essentials::SimulationState newGlobalState)
{
	_globalSimulationState = newGlobalState;
}

void Simulator::Delete(EntityContainer::key_type id) {
	if (id != 0) {
		_entitiesContainer.erase(id);
	}
	else {
		_entitiesContainer.erase(std::prev(_entitiesContainer.end()));
	}
}