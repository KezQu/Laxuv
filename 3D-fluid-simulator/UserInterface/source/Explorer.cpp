#include "Explorer.h"

#include <cstdint>
#include <string>

#include "Essentials.h"
#include "Interface.h"
#include "Line.h"
#include "Object.h"
#include "Particles.h"
#include "Point.h"
#include "Qube.h"
#include "Simulator.h"
#include "Sphere.h"
#include "Square.h"
#include "VertexArray.h"
#include "imgui.h"

uint64_t Explorer::selected = 0;
Explorer::entity_selector_t Explorer::entity_select = {0, 0};

Explorer::Explorer(ImVec2 const& size, ImVec2 const& position)
    : Interface(size, position,
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar |
                    // ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_AlwaysAutoResize)
{
}

void Explorer::Generate()
{
  ImGui::SetNextWindowPos(_position);
  ImGui::SetNextWindowSize(_size);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
  // ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {0, 0});

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
  if (ImGui::BeginChild("##SelectEntity", ImVec2(_size.x, 20)))
  {
    ImGui::SameLine();
    ImGui::SetNextItemWidth(_size.x / 3.);
    ImGui::Combo("##EntityType", &entity_select.first, " \0Object\0Particle\0");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(_size.x / 3.);
    ImGui::Combo("##EntityShape", &entity_select.second,
                 Essentials::DistShapeTolist());
    ImGui::SameLine();
    if (ImGui::Button("Add Entity##AddEntity", {_size.x / 3.f, 0.f}) &&
        entity_select.first != 0 && entity_select.second != 0)
    {
      SelectEntity();
    }
  }
  ImGui::EndChild();
}

void Explorer::CreateEntityExplorer()
{
  if (ImGui::BeginChild("##ObjectExplorer", ImVec2(_size.x, _size.y / 4 - 10)))
  {
    if (ImGui::BeginTable("##NamesTable", 2, ImGuiTableFlags_Borders,
                          ImVec2(_size.x, _size.y / 4 - 10)))
    {
      ImGui::TableSetupColumn("##Visibilty", ImGuiTableColumnFlags_WidthFixed,
                              20.);
      ImGui::TableSetupColumn("##Name", ImGuiTableColumnFlags_WidthStretch);
      for (auto& [k, v] : simulatorInstance.GetEntities())
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
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
  if (ImGui::BeginChild("##WorldDetails", ImVec2(_size.x, _size.y / 4 - 10)))
  {
    if (ImGui::BeginTable("##WorldDetailsTable", 2, ImGuiTableFlags_Borders,
                          ImVec2(_size.x, _size.y / 4 - 10)))
    {
      ImGui::TableSetupColumn("##WorldInfoName",
                              ImGuiTableColumnFlags_WidthFixed, _size.x / 2.);
      ImGui::TableSetupColumn("##WorldInfoValue",
                              ImGuiTableColumnFlags_WidthStretch);
      for (auto& info : simulatorInstance.GetDetails())
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(info.first.c_str());

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
  if (ImGui::BeginChild("##EntityControls", ImVec2(_size.x, _size.y / 2 - 10)))
  {
    if (ImGui::BeginTable("##ControlsTable", 2, ImGuiTableFlags_Borders,
                          ImVec2(_size.x, _size.y / 2 - 10)) &&
        selected != 0)
    {
      ImGui::TableSetupColumn("##InfoName", ImGuiTableColumnFlags_WidthFixed,
                              _size.x / 2.);
      ImGui::TableSetupColumn("##InfoValues",
                              ImGuiTableColumnFlags_WidthStretch);

      for (auto& info : simulatorInstance.GetEntities().at(selected)->Details())
      {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text(info.first.c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-1);
        info.second();

        ImGui::Spacing();
      }
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      if (ImGui::Button("Delete", ImVec2(-1, -1)))
      {
        simulatorInstance.Delete(selected);
        selected = 0;
      }
      ImGui::TableSetColumnIndex(1);
      if (ImGui::Button("Recompile", ImVec2(-1, -1)))
      {
        simulatorInstance.GetEntities()[selected]->Initialize();
      }
    }
    ImGui::EndTable();
  }
  ImGui::EndChild();
}

void Explorer::SelectEntity()
{
  switch (entity_select.second)
  {
    case 1:
      entity_select.first == 1
          ? simulatorInstance.Append(Object{new Point{}})
          : simulatorInstance.Append(Particles{new Point{}, 1U});
      break;
    case 2:
      entity_select.first == 1
          ? simulatorInstance.Append(Object{new Line{}})
          : simulatorInstance.Append(Particles{new Line{}, 1U});
      break;
    case 3:
      entity_select.first == 1
          ? simulatorInstance.Append(Object{new Square{}})
          : simulatorInstance.Append(Particles{new Square{Vertex{}, 1.f}, 1U});
      break;
    case 4:
      entity_select.first == 1
          ? simulatorInstance.Append(Object{new Qube{}})
          : simulatorInstance.Append(Particles{new Qube{Vertex{}, 1.f}, 1U});
      break;
    case 5:
      entity_select.first == 1
          ? simulatorInstance.Append(Object{new Sphere{}})
          : simulatorInstance.Append(Particles{new Sphere{Vertex{}, 1.f}, 1U});
      break;
    default:
      break;
  }
}
