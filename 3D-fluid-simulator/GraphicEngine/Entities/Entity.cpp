#include "Entity.h"

#include <cstdint>

#include "Essentials.h"
#include "glm/fwd.hpp"
#include "imgui.h"

uint64_t Entity::_internalID = 0U;

Essentials::DetailControls Entity::Details()
{
  // Create a map of controls with corresponding name to later expose to UI
  Essentials::DetailControls details;
  details.push_back({"Name", [this]() { ImGui::Text(this->Name().c_str()); }});
  return details;
}

uint64_t Entity::GetTerrainId()
{
  return _terrain_id.GetValue();
}

void Entity::SetTerrainId(uint64_t const terrain_id)
{
  _terrain_id = terrain_id;
}
ShaderStorageBuffer<Essentials::ParticleBufferProperties> const&
Entity::GetPhysicsBuffer()
{
  return _physicsDispatch.GetParticleMeshBuffer();
}
void Entity::Bind(uint32_t program_id) const
{
  // Register entity specific uniforms to the given program to be able to
  // retrieve their values on a GPU side
  _physics_type.MapUniform(program_id);
  _terrain_id.MapUniform(program_id);
  _mesh_size.MapUniform(program_id);
}

Entity::Entity(Essentials::PhysicsType physics, glm::ivec3 const& mesh_size)
    : _physicsDispatch{mesh_size},
      _mesh_size{mesh_size, "MeshSize", ValueProperties{1, 100}},
      _id{Entity::_internalID++},
      _name{"Entity" + std::to_string(_id)}
{
  _physics_type = static_cast<uint32_t>(physics);
}
