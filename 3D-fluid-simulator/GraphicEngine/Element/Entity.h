#pragma once

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Debug.h>

#include <unordered_map>
#include <functional>
#include <string>
#include <variant>
#include <cstdint>

enum DetailsType {
	STRING = 0,
	INT = 1,
	FLOAT = 2,
	VEC2 = 3,
	VEC3 = 4,
	VEC4 = 5
};

class Entity {
protected:
	static std::uint64_t _internalID;
	std::uint64_t _id;
	std::string _name;
public:
	std::string& Name();
	std::string ID() const;
	virtual void Draw() const;
	using details_map = std::vector<std::pair<std::string, 
		std::pair<std::variant<
		std::function<std::string&()>,
		std::function<int&()>,
		std::function<float&()>,
		std::function<glm::vec2&()>,
		std::function<glm::vec3&()>,
		std::function<glm::vec4&()>>, DetailsType>>>;

	virtual details_map Details();
protected:
	Entity();
};
