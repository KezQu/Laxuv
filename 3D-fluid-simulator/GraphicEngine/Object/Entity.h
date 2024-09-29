#pragma once

#include <Debug.h>

#include <cstdint>
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>
#include <string>
#include <unordered_map>
#include <variant>

#include "PhysicsDispatch.h"

enum DetailsType
{
  BOOL,
  STRING,
  INT32,
  UINT8,
  UINT32,
  UINT64,
  PHYSTYPE,
  FLOAT,
  VEC2,
  VEC3,
  VEC4
};

class Entity
{
 protected:
  PhysicsDispatch _physicsDispatch;
  static uint64_t _internalID;
  uint64_t _id;
  std::string _name;
  Essentials::PhysicsType _physics;
  bool _visible{true};

 public:
  using details_map = std::vector<std::pair<
      std::string,
      std::pair<std::variant<
                    std::function<bool&()>, std::function<std::string&()>,
                    std::function<int32_t&()>, std::function<uint8_t&()>,
                    std::function<uint32_t&()>, std::function<uint64_t&()>,
                    std::function<Essentials::PhysicsType&()>,
                    std::function<float&()>, std::function<glm::vec2&()>,
                    std::function<glm::vec3&()>, std::function<glm::vec4&()>>,
                DetailsType>>>;

 public:
  Entity(Entity const& objCopy) = delete;
  Entity(Entity&& objMove) = default;
  Entity& operator=(Entity const& objCopy) = delete;
  Entity& operator=(Entity&& objMove) = default;
  virtual ~Entity() = default;

  std::string& Name()
  {
    return _name;
  }
  bool& Visible()
  {
    return _visible;
  }
  Essentials::PhysicsType& GetPhysicsType()
  {
    return _physics;
  }
  Essentials::FluidProperties& GetFluidProperties()
  {
    return _physicsDispatch.GetFluidProperties();
  }
  uint64_t ID() const
  {
    return _id;
  }
  virtual void Initialize() {}
  virtual void Calculate() {}
  virtual void Draw() const {}
  virtual details_map Details();

 protected:
  Entity(Essentials::PhysicsType physics);
};
