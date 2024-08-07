#include "Essentials.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, PhysicsProperties const& particle) {
	out << "- - - - - - - -" << std::endl;
	//out << "(" << particle.forceMass.x << "," << particle.forceMass.y << "," << particle.forceMass.z << ")" << std::endl;
	//out << "(" << particle.velocity.x << "," << particle.velocity.y << "," << particle.velocity.z << "," << particle.velocity.w << ")" << std::endl;
	out << "(" << particle.position.x << "," << particle.position.y << "," << particle.position.z << "," << particle.position.w << ")" << std::endl;
	//out << particle.forceMass.w << std::endl;
	out << "- - - - - - - -" << std::endl;
	return out;
}