#include <Interface.h>


Simulator& Interface::simulatorInstance{ Simulator::GetInstance() };

Interface::Interface(ImVec2 const& size, ImVec2 const& position, ImGuiWindowFlags flags)
	:_size{ size },
	_position{position},
	_flags{ flags }
{}
Interface::~Interface()
{}