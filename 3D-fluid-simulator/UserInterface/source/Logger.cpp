#include "Logger.h"

std::deque<std::string> Logger::_logHistory;

Logger::Logger(std::ostringstream& log, ImVec2 const& size,
               ImVec2 const& position)
    : Interface(size, position,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_AlwaysAutoResize)
{
  // Trim outdated logs to speed up generation
  if (_logHistory.size() > 10) _logHistory.pop_front();
  if (log.str() != "")
    _logHistory.push_back(std::exchange(log, std::ostringstream()).str());
}

void Logger::Generate()
{
  // Specify the position and size of the next imgui window to be displayed in
  // the application window
  ImGui::SetNextWindowPos(_position);
  ImGui::SetNextWindowSize(_size);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  // Create a main logger window
  if (ImGui::Begin("Console ##LogConsole", nullptr, _flags))
  {
    // Display the log history retrieved from the stream
    if (ImGui::BeginChild("##content", ImVec2(0, 0), ImGuiChildFlags_None,
                          ImGuiWindowFlags_HorizontalScrollbar))
    {
      for (auto& message : _logHistory)
      {
        ImGui::TextWrapped("%s", message.c_str());
      }
    }
  }
  // Allow for auto scrolling for better user experience
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);
  ImGui::EndChild();
  ImGui::End();
  ImGui::PopStyleVar();
}
