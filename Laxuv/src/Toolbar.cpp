#include "Toolbar.h"

bool Toolbar::_fullscreen = true;

Toolbar::Toolbar(ImVec2 const& size, ImVec2 const& position)
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
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
		ImGui::SameLine(1855);
		if (ImGui::Button("_##Minimize", { 30, 20 })) {
			glfwIconifyWindow(glfwGetCurrentContext());
		}
		//ImGui::SameLine(1820);
		//if (ImGui::Button("m##Fullscreen", { 30, 20 })) {
		//	std::pair<int, int> winSize;
		//	if(_fullscreen){
		//		winSize = { 1280, 720 };
		//		glfwSetWindowMonitor(glfwGetCurrentContext(), nullptr, 100, 100, winSize.first, winSize.second, 0);
		//	}
		//	else {
		//		winSize = { 1920, 1080 };
		//		glfwSetWindowMonitor(glfwGetCurrentContext(), glfwGetPrimaryMonitor(), 0, 0, winSize.first, winSize.second, 60);
		//	}
		//	//glfwSetWindowSize(glfwGetCurrentContext(), winSize.first, winSize.second);
		//	_fullscreen != _fullscreen;
		//}
		ImGui::SameLine(1890);

		ImGui::PushStyleColor(ImGuiCol_Button, { 1, 0, 0, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 1, 0.4, 0.4, 1 });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 1, 0.7, 0.7, 1 });
		if (ImGui::Button("X##Exit", { 30, 20 })) {
			glfwSetWindowShouldClose(glfwGetCurrentContext(), GLFW_TRUE);
		}
		ImGui::PopStyleColor(3);
	}
	ImGui::End();
	ImGui::PopStyleVar();
}
