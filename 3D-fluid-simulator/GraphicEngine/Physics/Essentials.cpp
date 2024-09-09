#include "Essentials.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, PhysicsProperties const& particle) {
	out << "- - - - - - - -" << std::endl;
	out << "(" << particle.force.x << "," << particle.force.y << "," << particle.force.z << "," << particle.force.w << ")" << std::endl;
	out << "(" << particle.velocity.x << "," << particle.velocity.y << "," << particle.velocity.z << "," << particle.velocity.w << ")" << std::endl;
	out << "(" << particle.position.x << "," << particle.position.y << "," << particle.position.z << "," << particle.position.w << ")" << std::endl;
	out << "(" << particle.VolumeDensityPressureMass.x << "," 
		<< particle.VolumeDensityPressureMass.y << "," 
		<< particle.VolumeDensityPressureMass.z << "," << 
		particle.VolumeDensityPressureMass.w << ")" << std::endl;
	out << "[";
	for (auto& neighbour_id : particle.neighbours) {
		out << neighbour_id << ", ";
	}
	out << "]" << std::endl;
	out << "- - - - - - - -" << std::endl;
	return out;
}