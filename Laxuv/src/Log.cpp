#include "Log.h"

std::ostringstream Log::LogHistory = {};

Log::Log()
	:Interface({ 0,0 }, { 0,0 }, 0)
{}

Log::Log(ImVec2 const& size, ImVec2 const& position)
	:Interface(size, position, ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_AlwaysAutoResize)
{}

void Log::Generate()
{
	ImGui::SetNextWindowPos(_position);
	ImGui::SetNextWindowSize(_size);
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0,0 });
	if (ImGui::Begin("##LogConsole", nullptr, _flags)) {
		ImGui::TextWrapped(LogHistory.str().c_str());
	}
	ImGui::End();
	ImGui::PopStyleVar();
}

Log& Log::operator<<(std::string const& LogMessage)
{
	auto timestamp = std::chrono::system_clock().now();
	LogHistory << "[" << std::format("{:%T}", std::chrono::floor<std::chrono::seconds>(timestamp)) << "] " << LogMessage << std::endl;
	return *this;
}

Log::operator std::ostringstream&()
{
	return LogHistory;
}
