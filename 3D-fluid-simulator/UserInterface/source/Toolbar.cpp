#include "Toolbar.h"
#include <GLFW/glfw3.h>
#include <Camera.h>

bool Toolbar::_fullscreen = false;
std::unordered_map<Essentials::SimulationState, std::string> Toolbar::simulationState{
	{Essentials::SimulationState::IDLE, "Start"},
	{Essentials::SimulationState::INIT, "Start"},
	{Essentials::SimulationState::SIMULATION, "Stop"},
	{Essentials::SimulationState::GEN_FRAME, "Stop"},
};

Toolbar::Toolbar(ImVec2 const& size, ImVec2 const& position)
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		//ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize)
{}

void Toolbar::Generate() {
	ImGui::SetNextWindowPos(_position);
	ImGui::SetNextWindowSize(_size);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::Begin("##Toolbar", nullptr, _flags)) {
		if (ImGui::Button((simulationState[simulatorInstance.GetSimulationState()] + " simulation").c_str())) {
			simulatorInstance.SetSimulationState(simulatorInstance.GetSimulationState() == Essentials::SimulationState::SIMULATION ?
				Essentials::SimulationState::INIT :
				Essentials::SimulationState::SIMULATION);
		}
		if (simulatorInstance.GetSimulationState() != Essentials::SimulationState::SIMULATION) {
			ImGui::SameLine();
			if (ImGui::Button("Generate frame")) {
				simulatorInstance.SetSimulationState(Essentials::SimulationState::GEN_FRAME);
			}
		}
		ImGui::SameLine(_size.x - 300);
		ImGui::Text("%mouse speed: %.0f", Camera::GetCamera().GetMoveSpeed());
		ImGui::SameLine(_size.x - 150);
		ImGui::Text("%.0f", ImGui::GetIO().Framerate);
		ImGui::SameLine(_size.x - 100);
		if (ImGui::Button("_##Minimize", { 30, 20 })) {
			glfwIconifyWindow(glfwGetCurrentContext());
		}
		
		ImGui::SameLine(_size.x - 65);
		if (ImGui::Button("m##Fullscreen", { 30, 20 })) {
			GLFWmonitor* currentMonitor = _fullscreen ? nullptr : glfwGetPrimaryMonitor();
			glfwSetWindowMonitor(glfwGetCurrentContext(), currentMonitor, 0, 0, 1920, 1080, 60);
			_fullscreen = !_fullscreen;
		}

		ImGui::PushStyleColor(ImGuiCol_Button, { 1, 0, 0, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1, 0.4, 0.4, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1, 0.7, 0.7, 1 });
		ImGui::SameLine(_size.x - 30);
		if (ImGui::Button("X##Exit", { 30, 20 })) {
			glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
		}
		ImGui::PopStyleColor(3);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}
