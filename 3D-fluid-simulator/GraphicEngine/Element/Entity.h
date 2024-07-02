#pragma once

#include <unordered_map>
#include <functional>
#include <string>
#include <cstdint>
#include <Debug.h>

class Entity {
protected:
	static std::uint64_t _internalID;
	std::uint64_t _id;
	std::string _name;
public:
	std::string Name(std::string newName = {});
	virtual void Draw() const;
	template<typename T>
	std::unordered_map<std::string, std::function<void(T)>> Details() const {
		std::unordered_map<std::string, std::function<void(T)>> details;
		details.insert({ "Name", Entity::Name});
		return details;
	}
protected:
	Entity();
};
