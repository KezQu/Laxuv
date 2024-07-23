#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <Interface.h>
#include <ShapeInstance.h>
#include <Object.h>
#include <Particles.h>


class Explorer : public Interface {
public:
	using entityContainer = std::unordered_map<std::uint64_t, std::unique_ptr<Entity>>;
	static entityContainer EntitiesContainer;
public:
	Explorer(ImVec2 const& size, ImVec2 const& position);
	template<typename T>
	static void Append(T && entity);
	static void Delete(entityContainer::key_type id = 0);
	void Generate() override;
};
template<typename T>
void Explorer::Append(T&& entity) {
	entityContainer::key_type entity_id = entity.ID();
	EntitiesContainer[entity_id] = std::make_unique<T>(std::move(entity));
}