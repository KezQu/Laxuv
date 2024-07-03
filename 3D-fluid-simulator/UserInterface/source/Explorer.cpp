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

	static std::uint64_t selected = 0;
	if (ImGui::Begin("##Explorer", nullptr, _flags)) {
		if (ImGui::BeginChild("##ObjectExplorer", ImVec2(_size.x, _size.y / 2), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
			for (std::uint64_t i = 0; i < EntitiesVector.size(); ++i) {
				if (ImGui::Selectable((EntitiesVector.at(i)->Name() + " ##objectLookup" + EntitiesVector.at(i)->ID()).c_str())) {
					selected = i;
				}
			}
		}
		ImGui::EndChild();
		if (ImGui::BeginChild("##ObjectDetails", ImVec2(_size.x, _size.y / 2), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
			if (ImGui::BeginTable("##DetailsTable", 2, ImGuiTableFlags_None, ImVec2(_size.x, _size.y / 2))) {
				for (auto& info : EntitiesVector.at(selected)->Details()) {
					ImGui::TableNextRow(); 
					ImGui::TableSetColumnIndex(0);
					ImGui::Text(info.first.c_str());
	
					ImGui::TableSetColumnIndex(1);
					switch (info.second.second)
					{
					case DetailsType::STRING:
						ImGui::InputText(("##" + info.first).c_str(), &std::get<DetailsType::STRING>(info.second.first)(), ImGuiInputTextFlags_EnterReturnsTrue);
						break;
					case DetailsType::INT:
						ImGui::InputInt(("##" + info.first).c_str(), &std::get<DetailsType::INT>(info.second.first)(), ImGuiInputTextFlags_EnterReturnsTrue);
						break;
					case DetailsType::FLOAT:
						ImGui::InputFloat(("##" + info.first).c_str(), &std::get<DetailsType::FLOAT>(info.second.first)(), ImGuiInputTextFlags_EnterReturnsTrue);
						break;
					case DetailsType::VEC2:
						ImGui::InputFloat2(("##" + info.first).c_str(), glm::value_ptr(std::get<DetailsType::VEC2>(info.second.first)()), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
						break;
					case DetailsType::VEC3:
						ImGui::InputFloat3(("##" + info.first).c_str(), glm::value_ptr(std::get<DetailsType::VEC3>(info.second.first)()), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
						break;
					case DetailsType::VEC4:
						ImGui::InputFloat4(("##" + info.first).c_str(), glm::value_ptr(std::get<DetailsType::VEC4>(info.second.first)()), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
						break;
					default:
						break;
					}
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();

	}
	ImGui::End();
	ImGui::PopStyleVar();
}