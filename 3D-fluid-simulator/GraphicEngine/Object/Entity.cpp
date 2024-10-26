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
                       ImGui::Combo("##Physics_type", (int32_t*)&this->_physics,
                                    Essentials::PhysTypesTolist());
                     }});
  return details;
}

Entity::Entity(Essentials::PhysicsType physics, glm::ivec3 const& particle)
    : _physicsDispatch{particle},
      _id{Entity::_internalID++},
      _name{"Entity" + std::to_string(_id)},
      _physics{physics}
{
}
