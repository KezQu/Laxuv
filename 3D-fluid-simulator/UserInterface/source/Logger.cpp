#include "Logger.h"

std::deque<std::string> Logger::_logHistory;

Logger::Logger(std::ostringstream& log, ImVec2 const& size, ImVec2 const& position)
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize)
{
	if(_logHistory.size() > 10)
		_logHistory.pop_front();
	if(log.str() != "")
		_logHistory.push_back(std::exchange(log, std::ostringstream()).str());
}

void Logger::Generate()
{
	ImGui::SetNextWindowPos(_position);
	ImGui::SetNextWindowSize(_size);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::Begin("Console ##LogConsole", nullptr, _flags)){
		if (ImGui::BeginChild("##content", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
			for (auto& message : _logHistory) {
				ImGui::TextWrapped(message.c_str());
			}
		}
	}
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);
	ImGui::EndChild();
	ImGui::End();
	ImGui::PopStyleVar();
}