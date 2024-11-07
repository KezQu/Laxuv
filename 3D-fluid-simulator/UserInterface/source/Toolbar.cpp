#include "Toolbar.h"

#include "Camera.h"
#include "Essentials.h"
#include "Interface.h"
#include "Simulator.h"
#include "imgui.h"

float Toolbar::static_timestep = 6.f;

Toolbar::Toolbar(ImVec2 const& size, ImVec2 const& position)
    : Interface(size, position,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoDecoration |
                    ImGuiWindowFlags_AlwaysAutoResize)
{
}

void Toolbar::Generate()
{
  ImGui::SetNextWindowPos(_position);
  ImGui::SetNextWindowSize(_size);

  auto& simulatorInstance = Simulator::GetInstance();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  if (ImGui::Begin("##Toolbar", nullptr, _flags))
  {
    if (simulatorInstance->GetSimulationState() !=
        Essentials::SimulationState::GRAPHS)
    {
      if (ImGui::Button(simulatorInstance->GetSimulationState() !=
                                Essentials::SimulationState::SIMULATION
                            ? "Start"
                            : "Pause"))
      {
        simulatorInstance->SetSimulationState(
            simulatorInstance->GetSimulationState() ==
                    Essentials::SimulationState::SIMULATION
                ? Essentials::SimulationState::IDLE
                : Essentials::SimulationState::SIMULATION);
      }
      if (simulatorInstance->GetSimulationState() !=
          Essentials::SimulationState::SIMULATION)
      {
        ImGui::SameLine();
        if (ImGui::Button("One frame"))
        {
          simulatorInstance->SetSimulationState(
              Essentials::SimulationState::GEN_FRAME);
        }
      }
      else
      {
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
          simulatorInstance->SetSimulationState(
              Essentials::SimulationState::INIT);
        }
      }
      ImGui::SameLine();
      ImGui::PushItemWidth(100.f);
      ImGui::DragFloat("##timestep_value", &static_timestep, 0.1f, 0.f, 1000.f,
                       "%.1f ms", ImGuiSliderFlags_AlwaysClamp);
      ImGui::SameLine();
      if (ImGui::Button(simulatorInstance->IsStaticDtUsed()
                            ? "Use program latency"
                            : "Use static timestep"))
      {
        simulatorInstance->ToggleTimesetType();
      }
      ImGui::PopItemWidth();
      simulatorInstance->UpdateDeltaTime(static_timestep);

      ImGui::SameLine();
      if (ImGui::Button("Graphs mode"))
      {
        simulatorInstance->SetSimulationState(
            Essentials::SimulationState::GRAPHS);
      }
    }
    else
    {
      if (ImGui::Button("Generate graphs"))
      {
        simulatorInstance->CreateGraphs();
      }
      ImGui::SameLine();
      if (ImGui::Button("Simulation mode"))
      {
        simulatorInstance->SetSimulationState(
            Essentials::SimulationState::IDLE);
      }
    }

    ImGui::SameLine(_size.x - 200);
    ImGui::Text("%mouse speed: %.1f", Camera::GetCamera().GetMoveSpeed());
    std::string framerate =
        std::to_string(static_cast<int>(ImGui::GetIO().Framerate)) + " FPS";
    auto FPS_text_size = ImGui::CalcTextSize(framerate.c_str()).x;
    ImGui::SameLine(_size.x - FPS_text_size);
    ImGui::Text(framerate.c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar();
}
