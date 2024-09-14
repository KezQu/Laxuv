#pragma once

#include <Essentials.h>
#include <Entity.h>

class Simulator {
public:
	using EntityContainer = std::unordered_map<uint64_t, std::unique_ptr<Entity>>;
private:
	EntityContainer _entitiesContainer;
	Essentials::SimulationState _globalSimulationState;
private:
	Simulator() = default;
public:
	static Simulator& GetInstance();
	EntityContainer& GetEntities();
	Essentials::SimulationState GetSimulationState();
	void SetSimulationState(Essentials::SimulationState newGlobalState);
	template<typename T>
	void Append(T&& entity);
	void Delete(EntityContainer::key_type id = 0);
	void CleanUp();
};
template<typename T>
void Simulator::Append(T&& entity) {
	EntityContainer::key_type entity_id = entity.ID();
	_entitiesContainer[entity_id] = std::make_unique<T>(std::move(entity));
}