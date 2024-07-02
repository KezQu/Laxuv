#include <Entity.h>

std::uint64_t Entity::_internalID = 0;

std::string Entity::Name(std::string newName) {
	if (!newName.empty()) {
		_name = newName;
	}
	return _name;
}

void Entity::Draw() const {
}

Entity::Entity()
	:_id{ Entity::_internalID++ },
	_name{ "Entity" + std::to_string(_id) }
{}
