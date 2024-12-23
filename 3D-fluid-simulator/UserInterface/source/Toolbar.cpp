#include "Toolbar.h"

#include "Camera.h"
#include "Essentials.h"
#include "Interface.h"
#include "Simulator.h"
#include "imgui.h"

float Toolbar::static_timestep = 1.f;

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
  // Specify the position and size of the next imgui window to be displayed in
  // the application window
  ImGui::SetNextWindowPos(_position);
  ImGui::SetNextWindowSize(_size);

  auto& simulatorInstance = Simulator::GetInstance();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  // Define the main toolbar window
  if (ImGui::Begin("##Toolbar", nullptr, _flags))
  {
    // Define different controls based on the simulation state
    if (simulatorInstance->GetSimulationState() !=
        Essentials::SimulationState::GRAPHS)
    {
      // Define Buttons allowing to manipulate simulation state
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
      // Define a control for static timestep to be changed by the user
      ImGui::DragFloat("##timestep_value", &static_timestep, 0.1f, 0.1f, 1000.f,
                       "%.1f ms", ImGuiSliderFlags_AlwaysClamp);
      ImGui::SameLine();
      // Allow user to change between static and dynamic timestep further used
      // in the simulation
      if (ImGui::Button(simulatorInstance->IsStaticDtUsed()
                            ? "Use program latency"
                            : "Use static timestep"))
      {
        simulatorInstance->ToggleTimesetType();
      }
      ImGui::PopItemWidth();
      simulatorInstance->UpdateDeltaTime(static_timestep);

      ImGui::SameLine();
      // Allow user to change into the graphs mode
      if (ImGui::Button("Graphs mode"))
      {
        simulatorInstance->SetSimulationState(
            Essentials::SimulationState::GRAPHS);
      }
    }
    else
    {
      // Define controls for the graphs mode
      if (ImGui::Button("Generate graphs"))
      {
        simulatorInstance->CreateGraphs();
      }
      ImGui::SameLine();
      // Allow user to get back to the paused simulation
      if (ImGui::Button("Simulation mode"))
      {
        simulatorInstance->SetSimulationState(
            Essentials::SimulationState::IDLE);
      }
    }

    ImGui::SameLine(_size.x - 200);
    // Display current camera movement speed
    ImGui::Text("mouse speed: %.1f", Camera::GetCamera().GetMoveSpeed());
    // Process framerate form the imgui library and display it to the user
    std::string framerate =
        std::to_string(static_cast<int>(ImGui::GetIO().Framerate)) + " FPS";
    auto FPS_text_size = ImGui::CalcTextSize(framerate.c_str()).x;
    ImGui::SameLine(_size.x - FPS_text_size);
    ImGui::Text("%s", framerate.c_str());
  }
  ImGui::End();
  ImGui::PopStyleVar();
}
