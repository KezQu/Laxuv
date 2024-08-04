#pragma once

#include <Interface.h>
#include <unordered_map>
#include <Simulator.h>

class Toolbar : public Interface {
private:
	static bool _fullscreen;
	static std::unordered_map<SimulationState, std::string> simulationState;
public:
	Toolbar(ImVec2 const& size, ImVec2 const& position);
	void Generate() override;
};
