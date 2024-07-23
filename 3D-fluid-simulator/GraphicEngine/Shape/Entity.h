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
	BOOL = 0,
	STRING = 1,
	INT = 2,
	SIZET = 3,
	FLOAT = 4,
	VEC2 = 5,
	VEC3 = 6,
	VEC4 = 7
};

class Entity {
protected:
	static std::uint64_t _internalID;
	std::uint64_t _id;
	std::string _name;
	bool _visible{ true };
public:
	std::string& Name();
	bool& Visible();
	std::uint64_t ID() const;
	virtual void Draw() const;
	using details_map = std::vector<std::pair<std::string, 
		std::pair<std::variant<
		std::function<bool& ()>,
		std::function<std::string& ()>,
		std::function<int& ()>,
		std::function<std::size_t& ()>,
		std::function<float& ()>,
		std::function<glm::vec2& ()>,
		std::function<glm::vec3& ()>,
		std::function<glm::vec4& ()>>, DetailsType>>>;

	virtual details_map Details();
	virtual ~Entity() = default;
protected:
	Entity();
};
