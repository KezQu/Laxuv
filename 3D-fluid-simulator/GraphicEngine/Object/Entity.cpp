#include <Entity.h>
#include <Essentials.h>

uint64_t Entity::_internalID = 0U;

details::details_map Entity::Details()
{
  details::details_map details;
  details.push_back({"Name",
                     {[this]() { return std::ref(this->Name()); },
                      details::DetailsType::STRING}});
  return details;
}

Entity::Entity(Essentials::PhysicsType physics)
    : _physicsDispatch{{0, 0, 0}},
      _id{Entity::_internalID++},
      _name{"Entity" + std::to_string(_id)},
      _physics{physics}
{
}
