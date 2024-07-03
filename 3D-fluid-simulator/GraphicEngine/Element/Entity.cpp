#include <Entity.h>

std::uint64_t Entity::_internalID = 0;

std::string& Entity::Name() {
	return _name;
}

std::string Entity::ID() const
{
	return std::to_string(_id);
}

void Entity::Draw() const {
}

Entity::details_map Entity::Details() {
	details_map details;
	details.push_back({ "Name", { [=]() {return std::ref(this->Name()); }, DetailsType::STRING } });
	return details;
}

Entity::Entity()
	:_id{ Entity::_internalID++ },
	_name{ "Entity" + std::to_string(_id) }
{}
