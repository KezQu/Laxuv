#include "Log.h"

std::deque<std::ostringstream> Log::logHistory = {};

Log::Log()
	:Interface({ 0,0 }, { 0,0 }, 0)
{
	logHistory.push_back({});
	if (logHistory.size() > 20) logHistory.pop_front();
}

Log::Log(ImVec2 const& size, ImVec2 const& position)
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize)
{}

void Log::Generate()
{
	ImGui::SetNextWindowPos(_position);
	ImGui::SetNextWindowSize(_size);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::Begin("##LogConsole", nullptr, _flags)){
		std::string logOutput{};
		for (auto& log : logHistory)
			logOutput += log.str();
		ImGui::TextWrapped(logOutput.c_str());
	}
	ImGui::End();
	ImGui::PopStyleVar();
}