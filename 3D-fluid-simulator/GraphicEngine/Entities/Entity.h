#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "Essentials.h"
#include "PhysicsDispatch.h"
#include "Uniform.h"
#include "glm/fwd.hpp"

class Entity;
namespace Essentials
{
using DetailControls =
    std::vector<std::pair<std::string, std::function<void()>>>;
using EntityContainer = std::unordered_map<uint64_t, std::unique_ptr<Entity>>;

};  // namespace Essentials

/**
 * @brief Base class representing an abstract called entity
 *
 */
class Entity
{
 protected:
  PhysicsDispatch _physicsDispatch;
  Uniform<uint32_t> _physics_type{
      static_cast<uint32_t>(Essentials::PhysicsType::NONE), "physicsType"};
  Uniform<glm::ivec3, int32_t> _mesh_size;
  static uint64_t _internalID;
  uint64_t _id;
  Uniform<uint64_t> _terrain_id{std::numeric_limits<uint64_t>::max(),
                                "terrainId"};
  std::string _name;
  bool _visible{true};

 public:
  /**
   * @brief Explicit deletion of copy constructor to ensure that
   * every entity is unique
   *
   * @param objCopy
   */
  Entity(Entity const& objCopy) = delete;
  Entity(Entity&& objMove) = default;
  /**
   * @brief Explicit deletion of copy operator to ensure that
   * every entity is unique
   *
   * @param objCopy
   */
  Entity& operator=(Entity const& objCopy) = delete;
  Entity& operator=(Entity&& objMove) = default;
  virtual ~Entity() = default;
  /**
   * @brief Getter to retrieve entity name
   *
   * @return std::string&
   */
  std::string& Name()
  {
    return _name;
  }
  /**
   * @brief Getter to retrieve information whether entity is visible
   *
   * @return flag representing visibility
   */
  bool& Visible()
  {
    return _visible;
  }
  /**
   * @brief Getter to retrieve unique entity id
   *
   * @return uint64_t
   */
  uint64_t ID() const
  {
    return _id;
  }
  /**
   * @brief Virtual function responsible for initializing the entities
   * derivative objects
   *
   */
  virtual void Initialize() {}
  /**
   * @brief Virtual function responsible for calculating physics during
   * simulation runtime for derivative objects
   *
   */
  virtual void Calculate() {}
  /**
   * @brief Virtual function responsible for scheduling drawing commands of
   * derived objects
   *
   */
  virtual void Draw() const {}
  /**
   * @brief Method responsible for binding entities uniform values shared for
   * all derived objects
   *
   * @param program_id id of a program to bind unifroms to
   */
  virtual void Bind(uint32_t program_id) const;
  /**
   * @brief Method that creates handlers to specific entity properties to expose
   * them to user interface
   *
   * @return Essentials::DetailControls map containing the name of a control and
   * handler responsible for the control
   */
  virtual Essentials::DetailControls Details();
  /**
   * @brief Getter for a unique terrain id for entities that are registered as
   * terrain
   *
   * @return uint64_t
   */
  uint64_t GetTerrainId();
  /**
   * @brief Getter for buffer that contains all information about physic
   * properties of an entity
   *
   * @return ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
   */
  ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
  GetPhysicsBuffer();
  /**
   * @brief Getter for a color type of an entity
   *
   * @return Essentials::ColorProperty
   */
  virtual Essentials::ColorProperty GetColorType() const
  {
    return Essentials::ColorProperty::NONE;
  }
  /**
   * @brief Getter for physics type of an entity. DYNAMIC is reserved for moving
   * entities and STATIC for stationary entities
   *
   * @return Essentials::PhysicsType
   */
  virtual Essentials::PhysicsType GetPhysicsType() const
  {
    return static_cast<Essentials::PhysicsType>(_physics_type.GetValue());
  }
  /**
   * @brief Setter for a unique terrain id, used when entity is registered as
   * terrain
   *
   * @param terrain_id
   */
  void SetTerrainId(uint64_t const terrain_id);
  /**
   * @brief Virtual getter responsible for retrieving properties of a shape for
   * a given derived objects
   *
   * @return Essentials::ShapeProperties
   */
  virtual Essentials::ShapeProperties GetShapeProperties()
  {
    return Essentials::ShapeProperties{};
  }

 protected:
  /**
   * @brief Protected constructor to disallow creating an entity object and
   * reserve this functionality for derived classes
   *
   * @param physics physics type
   * @param mesh_size mesh size needed for entities that are made of many
   * independent particles
   */
  Entity(Essentials::PhysicsType physics,
         glm::ivec3 const& mesh_size = {1, 1, 1});
};
