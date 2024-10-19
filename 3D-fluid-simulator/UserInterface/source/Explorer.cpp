#include <Explorer.h>
#include <Object.h>
#include <Particles.h>
#include <ShapeInstance.h>

#include <variant>

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

  static std::uint64_t selected = 0;
  if (ImGui::Begin("##Explorer", nullptr, _flags))
  {
    static std::pair<int, int> objectSelect = {0, 0};
    if (ImGui::BeginChild("##SelectObject", ImVec2(_size.x, 20)))
    {
      ImGui::SameLine();
      ImGui::SetNextItemWidth(_size.x / 3.);
      ImGui::Combo("##EntityType", &objectSelect.first,
                   " \0Object\0Particle\0");
      ImGui::SameLine();
      ImGui::SetNextItemWidth(_size.x / 3.);
      ImGui::Combo("##EntityShape", &objectSelect.second,
                   " \0Point\0Line\0Square\0Qube\0Sphere\0");
      ImGui::SameLine();
      if (ImGui::Button("Add Entity##AddEntity", {_size.x / 3.f, 0.f}) &&
          objectSelect.first != 0 && objectSelect.second != 0)
      {
        switch (objectSelect.second)
        {
          case 1:
            objectSelect.first == 1
                ? simulatorInstance.Append(Object(new Point()))
                : simulatorInstance.Append(Particles(new Point(), 1U));
            break;
          case 2:
            objectSelect.first == 1
                ? simulatorInstance.Append(Object(new Line()))
                : simulatorInstance.Append(Particles(new Line(), 1U));
            break;
          case 3:
            objectSelect.first == 1
                ? simulatorInstance.Append(Object(new Square()))
                : simulatorInstance.Append(
                      Particles(new Square(Vertex{}, 1), 1U));
            break;
          case 4:
            objectSelect.first == 1
                ? simulatorInstance.Append(Object(new Qube()))
                : simulatorInstance.Append(
                      Particles(new Qube(Vertex{}, 1), 1U));
            break;
          case 5:
            objectSelect.first == 1
                ? simulatorInstance.Append(Object(new Sphere()))
                : simulatorInstance.Append(
                      Particles(new Sphere(Vertex{}, 1), 1U));
            break;
        }
      }
    }
    ImGui::EndChild();
    if (ImGui::BeginChild("##ObjectExplorer",
                          ImVec2(_size.x, _size.y / 4 - 10)))
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
          CreateProperty(info);

          ImGui::Spacing();
        }
      }
      ImGui::EndTable();
    }
    ImGui::EndChild();
    if (ImGui::BeginChild("##ObjectDetails", ImVec2(_size.x, _size.y / 2 - 10)))
    {
      if (ImGui::BeginTable("##DetailsTable", 2, ImGuiTableFlags_Borders,
                            ImVec2(_size.x, _size.y / 2 - 10)) &&
          selected != 0)
      {
        ImGui::TableSetupColumn("##InfoName", ImGuiTableColumnFlags_WidthFixed,
                                _size.x / 2.);
        ImGui::TableSetupColumn("##InfoValues",
                                ImGuiTableColumnFlags_WidthStretch);

        for (auto& info :
             simulatorInstance.GetEntities().at(selected)->Details())
        {
          ImGui::TableNextRow();
          ImGui::TableSetColumnIndex(0);
          ImGui::Text(info.first.c_str());

          ImGui::TableSetColumnIndex(1);
          ImGui::SetNextItemWidth(-1);
          CreateProperty(info);

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
  ImGui::End();
  ImGui::PopStyleVar(4);
}

void Explorer::CreateProperty(details::details_mapper& info)
{
  switch (info.second.second)
  {
    case details::DetailsType::BOOL:
      ImGui::Checkbox(
          ("##" + info.first).c_str(),
          &std::get<details::DetailsType::BOOL>(info.second.first)());
      break;
    case details::DetailsType::STRING:
      ImGui::InputText(
          ("##" + info.first).c_str(),
          &std::get<details::DetailsType::STRING>(info.second.first)(),
          ImGuiInputTextFlags_EnterReturnsTrue);
      break;
    case details::DetailsType::INT32:
      ImGui::DragInt(
          ("##" + info.first).c_str(),
          &std::get<details::DetailsType::INT32>(info.second.first)(), 1,
          INT_MIN / 2, INT_MAX / 2, "%d", ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::UINT8:
      ImGui::DragScalar(
          ("##" + info.first).c_str(), ImGuiDataType_U8,
          &std::get<details::DetailsType::UINT8>(info.second.first)(), 1,
          (void*)0, (void*)0, "%d", ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::UINT32:
      ImGui::DragScalar(
          ("##" + info.first).c_str(), ImGuiDataType_U32,
          &std::get<details::DetailsType::UINT32>(info.second.first)(), 1,
          (void*)0, (void*)0, "%d", ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::UINT64:
      ImGui::DragScalar(
          ("##" + info.first).c_str(), ImGuiDataType_U64,
          &std::get<details::DetailsType::UINT64>(info.second.first)(), 1,
          (void*)0, (void*)0, "%d", ImGuiSliderFlags_AlwaysClamp);
      break;
    // case details::DetailsType::DISTSHAPE:
    //   ImGui::DragScalar(
    //       ("##" + info.first).c_str(), ImGuiDataType_U8,
    //       &std::get<details::DetailsType::DISTSHAPE>(info.second.first)(),
    //       1, (void*)0, (void*)0, "%d", ImGuiSliderFlags_AlwaysClamp);
    //   break;
    case details::DetailsType::PHYSTYPE:
      ImGui::DragScalar(
          ("##" + info.first).c_str(), ImGuiDataType_U8,
          &std::get<details::DetailsType::PHYSTYPE>(info.second.first)(), 1,
          (void*)0, (void*)0, "%d", ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::FLOAT:
      ImGui::DragFloat(
          ("##" + info.first).c_str(),
          &std::get<details::DetailsType::FLOAT>(info.second.first)(), 1e-1f,
          -FLT_MAX / 2.f, FLT_MAX / 2.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::VEC2:
      ImGui::DragFloat2(("##" + info.first).c_str(),
                        glm::value_ptr(std::get<details::DetailsType::VEC2>(
                            info.second.first)()),
                        1e-1f, -FLT_MAX / 2.f, FLT_MAX / 2.f, "%.1f",
                        ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::VEC3:
      ImGui::DragFloat3(("##" + info.first).c_str(),
                        glm::value_ptr(std::get<details::DetailsType::VEC3>(
                            info.second.first)()),
                        1e-1f, -FLT_MAX / 2.f, FLT_MAX / 2.f, "%.1f",
                        ImGuiSliderFlags_AlwaysClamp);
      break;
    case details::DetailsType::VEC4:
      ImGui::DragFloat4(("##" + info.first).c_str(),
                        glm::value_ptr(std::get<details::DetailsType::VEC4>(
                            info.second.first)()),
                        1e-1f, -FLT_MAX / 2.f, FLT_MAX / 2.f, "%.1f",
                        ImGuiSliderFlags_AlwaysClamp);
      break;
    default:
      break;
  }
}
