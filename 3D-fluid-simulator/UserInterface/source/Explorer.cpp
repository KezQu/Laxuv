#include "Explorer.h"

#include <cstdint>
#include <string>

#include "Essentials.h"
#include "Interface.h"
#include "Simulator.h"
#include "imgui.h"

uint64_t Explorer::selected = 0;
Explorer::entity_selector_t Explorer::entity_select = {
    Essentials::EntityType::NONE, Essentials::EntityShape::NONE};

Explorer::Explorer(ImVec2 const& size, ImVec2 const& position)
    : Interface(size, position,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_AlwaysAutoResize)
{
}

void Explorer::Generate()
{
  // Schedule position and size of the next imgui window
  ImGui::SetNextWindowPos(_position);
  ImGui::SetNextWindowSize(_size);

  // Specify global style of generating the controls
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {5, 0});

  // Create imgui main Explorer window
  if (ImGui::Begin("##Explorer", nullptr, _flags))
  {
    CreateEntitySelector();
    CreateEntityExplorer();
    CreateWorldControls();
    CreateEntityControls();
  }
  ImGui::PopStyleVar(4);

  ImGui::End();
}

void Explorer::CreateEntitySelector()
{
  // Create child window with the specified size, responsible for containing
  // controls which allow to create new entities
  if (ImGui::BeginChild("##SelectEntity", ImVec2(_size.x, 20)))
  {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(_size.x / 3.);
    ImGui::Combo("##EntityType", (int32_t*)&entity_select.first,
                 Essentials::EntityTypeTolist());
    ImGui::SameLine();
    ImGui::SetNextItemWidth(_size.x / 3.);
    ImGui::Combo("##EntityShape", (int32_t*)&entity_select.second,
                 Essentials::EntityShapeTolist());
    ImGui::SameLine();
    // Create new Entity with a selected shape after it was submitted by
    // clicking the button
    if (ImGui::Button("Add Entity##AddEntity", {_size.x / 3.f, 0.f}) &&
        entity_select.first != Essentials::EntityType::NONE &&
        entity_select.second != Essentials::EntityShape::NONE)
    {
      Simulator::GetInstance()->CreateEntity(entity_select.first,
                                             entity_select.second);
    }
  }
  ImGui::EndChild();
}

void Explorer::CreateEntityExplorer()
{
  // Create a child window containing scrollable list of present entities inside
  // a Simulator
  if (ImGui::BeginChild("##ObjectExplorer", ImVec2(_size.x, _size.y / 4 - 10)))
  {
    // Create a table of entities based on the Entities stored within Simulator
    // instance
    if (ImGui::BeginTable("##NamesTable", 2, ImGuiTableFlags_Borders,
                          ImVec2(_size.x, _size.y / 4 - 10)))
    {
      // Create table definition with sizing flags
      ImGui::TableSetupColumn("##Visibility", ImGuiTableColumnFlags_WidthFixed,
                              20.);
      ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch);
      // Create rows based on the current entities
      for (auto& [k, v] : Simulator::GetInstance()->GetEntities())
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        // Create a checkbox allowing to hide entity in the simulation context
        ImGui::Checkbox(("##visible" + std::to_string(v->ID())).c_str(),
                        &v->Visible());
        ImGui::TableSetColumnIndex(1);
        if (ImGui::Selectable(
                (v->Name() + " ##objectLookup" + std::to_string(v->ID()))
                    .c_str()))
        {
          selected = k;
        }
      }
    }
    ImGui::EndTable();
  }
  ImGui::EndChild();
}

void Explorer::CreateWorldControls()
{
  // Define a child window containing exposed world and simulator properties
  if (ImGui::BeginChild("##WorldDetails", ImVec2(_size.x, _size.y / 4 - 10)))
  {
    // Define a table containing exposed global properties with a short
    // description
    if (ImGui::BeginTable("##WorldDetailsTable", 2, ImGuiTableFlags_Borders,
                          ImVec2(_size.x, _size.y / 4 - 10)))
    {
      ImGui::TableSetupColumn("##WorldInfoName",
                              ImGuiTableColumnFlags_WidthFixed, _size.x / 3.);
      ImGui::TableSetupColumn("##WorldInfoValue",
                              ImGuiTableColumnFlags_WidthStretch);
      for (auto& info : Simulator::GetInstance()->GetDetails())
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", info.first.c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1);
        info.second();

        ImGui::Spacing();
      }
    }
    ImGui::EndTable();
  }
  ImGui::EndChild();
}

void Explorer::CreateEntityControls()
{
  // Create a child window containing dynamic controls based on the chosen
  // entity
  if (ImGui::BeginChild("##EntityControls", ImVec2(_size.x, _size.y / 2 - 10)))
  {
    // Create a table with properties based on the entity type for a selected
    // entity from the explorer
    if (ImGui::BeginTable("##ControlsTable", 2, ImGuiTableFlags_Borders,
                          ImVec2(_size.x, _size.y / 2 - 10)) &&
        selected != 0)
    {
      ImGui::TableSetupColumn("##InfoName", ImGuiTableColumnFlags_WidthFixed,
                              _size.x / 3.);
      ImGui::TableSetupColumn("##InfoValues",
                              ImGuiTableColumnFlags_WidthStretch);

      auto& simulatorInstance = Simulator::GetInstance();
      for (auto& info :
           simulatorInstance->GetEntities().at(selected)->Details())
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%s", info.first.c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1);
        info.second();

        ImGui::Spacing();
      }
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      // Define a "delete" button allowing to delete entity from the simulation
      if (ImGui::Button("Delete", ImVec2(-1, 50)))
      {
        simulatorInstance->Delete(selected);
        selected = 0;
      }
      ImGui::TableSetColumnIndex(1);
      // Define "Recompile" button to restore initial state of the entity when
      // artifacts occur in the simulation
      if (ImGui::Button("Recompile", ImVec2(-1, 50)))
      {
        auto old_sim_state_saved =
            Simulator::GetInstance()->GetSimulationState();
        Simulator::GetInstance()->SetSimulationState(
            Essentials::SimulationState::INIT);
        simulatorInstance->GetEntities()[selected]->Initialize();
        Simulator::GetInstance()->SetSimulationState(old_sim_state_saved);
      }
    }
    ImGui::EndTable();
  }
  ImGui::EndChild();
}