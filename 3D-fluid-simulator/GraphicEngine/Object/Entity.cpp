#include <Entity.h>
#include <Essentials.h>
#include <imgui.h>

uint64_t Entity::_internalID = 0U;

details::detail_controls_t Entity::Details()
{
  details::detail_controls_t details;
  details.push_back({"Name", [this]() { ImGui::Text(this->Name().c_str()); }});
  details.push_back({"Physics type", [this]()
                     {
                       ImGui::Combo("##Physics_type",
                                    (int32_t*)&this->_physics_type.GetValue(),
                                    Essentials::PhysTypesTolist());
                     }});
  return details;
}

void Entity::Bind(uint32_t program_id) const
{
  _physics_type.MapUniform(program_id);
}

Entity::Entity(Essentials::PhysicsType physics, glm::uvec3 const& mesh_size)
    : _physicsDispatch{mesh_size},
      _mesh_size{mesh_size},
      _id{Entity::_internalID++},
      _name{"Entity" + std::to_string(_id)}
{
  _physics_type = static_cast<uint32_t>(physics);
}
