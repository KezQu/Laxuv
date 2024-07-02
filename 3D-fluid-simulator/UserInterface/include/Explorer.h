#pragma once

#include <vector>
#include <memory>
#include <Interface.h>
#include <Entity.h>

class Explorer : public Interface {
public:
	static std::vector<std::unique_ptr<Entity>> EntitiesVector;
public:
	Explorer(ImVec2 const& size, ImVec2 const& position);
	template<typename T>
	static void Append(T && entity);
	void Generate() override;
};
template<typename T>
void Explorer::Append(T&& entity) {
	EntitiesVector.push_back(std::make_unique<T>(std::move(entity)));
}