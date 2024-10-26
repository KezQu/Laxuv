#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "Essentials.h"
#include "PhysicsDispatch.h"

namespace details
{
using detail_controls_t =
    std::vector<std::pair<std::string, std::function<void()>>>;
};  // namespace details

class Entity
{
 protected:
  PhysicsDispatch _physicsDispatch;
  static uint64_t _internalID;
  uint64_t _id;
  std::string _name;
  // TODO: change to Uniform
  Essentials::PhysicsType _physics;
  bool _visible{true};

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
  virtual details::detail_controls_t Details();

 protected:
  Entity(Essentials::PhysicsType physics, glm::ivec3 const& particle = {0,0,0});
};
