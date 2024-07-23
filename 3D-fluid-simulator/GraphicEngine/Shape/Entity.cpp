#include <Entity.h>

std::uint64_t Entity::_internalID = 0;

std::string& Entity::Name() {
	return _name;
}

bool& Entity::Visible()
{
	return _visible;
}

std::uint64_t Entity::ID() const
{
	return _id;
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
