#include "Properties.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, PhysicsProperties const& particle) {
	out << "- - - - - - - -" << std::endl;
	out << "(" << particle.acceleration.x << "," << particle.acceleration.y << "," << particle.acceleration.z << ")" << std::endl;
	out << "(" << particle.velocity.x << "," << particle.velocity.y << "," << particle.velocity.z << ")" << std::endl;
	out << "(" << particle.position.x << "," << particle.position.y << "," << particle.position.z << ")" << std::endl;
	out << particle.mass.x << std::endl;
	out << "- - - - - - - -" << std::endl;
	return out;
}