#include <Explorer.h>

std::vector<Object> Explorer::ObjectVector = {};

Explorer::Explorer(ImVec2 const& size, ImVec2 const& position) 
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize)
{}
void Explorer::Generate() {
	ImGui::SetNextWindowPos(_position);
	ImGui::SetNextWindowSize(_size);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::Begin("##Explorer", nullptr, _flags)) {
		for (size_t i = 0; i < ObjectVector.size(); i++) {
			ImGui::Selectable(("##objectLookup" + std::to_string(i)).c_str());
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}