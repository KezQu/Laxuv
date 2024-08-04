#include <Entity.h>

uint64_t Entity::_internalID = 0U;

std::string& Entity::Name() {
	return _name;
}

bool& Entity::Visible()
{
	return _visible;
}

uint64_t Entity::ID() const
{
	return _id;
}
void Entity::Initialize() {
}

void Entity::Calculate() {
}

void Entity::Draw() const {
}

void Entity::UpdateBoundingDimensions() {
}

Entity::details_map Entity::Details() {
	details_map details;
	details.push_back({ "Name", { [=]() {return std::ref(this->Name()); }, DetailsType::STRING } });
	return details;
}

Entity::Entity(PhysicsType physics)
	:_physicsDispatch{ { 0,0,0 } },
	_id{ Entity::_internalID++ },
	_name{ "Entity" + std::to_string(_id) },
	_physics{physics}
{}
