#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "Essentials.h"
#include "PhysicsDispatch.h"
#include "Uniform.h"

namespace details
{
using detail_controls_t =
    std::vector<std::pair<std::string, std::function<void()>>>;
};  // namespace details

class Entity
{
 protected:
  PhysicsDispatch _physicsDispatch;
  Uniform<uint32_t> _physics_type{
      static_cast<uint32_t>(Essentials::PhysicsType::NONE), "physicsType"};
  glm::uvec3 _mesh_size{1U};
  static uint64_t _internalID;
  uint64_t _id;
  std::string _name;
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
  uint64_t ID() const
  {
    return _id;
  }
  virtual void Initialize() {}
  virtual void Calculate() {}
  virtual void Draw() const {}
  virtual void Bind(uint32_t program_id) const;
  virtual details::detail_controls_t Details();

 protected:
  Entity(Essentials::PhysicsType physics,
         glm::uvec3 const& mesh_size = {1, 1, 1});
};
