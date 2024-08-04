#pragma once

#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Debug.h>

#include <unordered_map>
#include <functional>
#include <string>
#include <variant>
#include <cstdint>

#include "PhysicsDispatch.h"

enum DetailsType {
	BOOL,
	STRING,
	INT32,
	UINT64,
	DISTSHAPE,
	FLOAT,
	VEC2,
	VEC3,
	VEC4
};

enum class PhysicsType : uint8_t{
	NONE,
	STATIC,
	DYNAMIC
};

class Entity {
protected:
	PhysicsDispatch _physicsDispatch;
	static uint64_t _internalID;
	uint64_t _id;
	std::string _name;
	PhysicsType _physics;
	bool _visible{ true };
public:
	using details_map = std::vector<std::pair<std::string, 
		std::pair<std::variant<
		std::function<bool& ()>,
		std::function<std::string& ()>,
		std::function<int32_t& ()>,
		std::function<uint64_t& ()>,
		std::function<DistributionShape& ()>,
		std::function<float& ()>,
		std::function<glm::vec2& ()>,
		std::function<glm::vec3& ()>,
		std::function<glm::vec4& ()>>, DetailsType>>>;
public:
	Entity(Entity const& objCopy) = delete;
	Entity(Entity && objMove) = default;
	Entity & operator=(Entity const& objCopy) = delete;
	Entity & operator=(Entity && objMove) = default;
	virtual ~Entity() = default;

	std::string& Name();
	bool& Visible();
	uint64_t ID() const;
	virtual void Initialize();
	virtual void Calculate();
	virtual void Draw() const;
	virtual void UpdateBoundingDimensions();
	virtual details_map Details();

protected:
	Entity(PhysicsType physics);
};
