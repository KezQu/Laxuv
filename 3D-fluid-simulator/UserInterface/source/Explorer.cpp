#include <Explorer.h>

std::vector<std::unique_ptr<Entity>> Explorer::EntitiesVector = {};

Explorer::Explorer(ImVec2 const& size, ImVec2 const& position) 
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize)
{}

void Explorer::Generate() {
	ImGui::SetNextWindowPos(_position);
	ImGui::SetNextWindowSize(_size);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::Begin("##Explorer", nullptr, _flags)) {
		for (auto& entity: EntitiesVector) {
			ImGui::Selectable((entity->Name() + " ##objectLookup").c_str());
		}
	}
	ImGui::End();
	ImGui::PopStyleVar();
}