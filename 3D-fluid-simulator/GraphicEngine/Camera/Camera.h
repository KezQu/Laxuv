#pragma once

#include <imgui.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

#include "Uniform.h"

class Camera
{
 private:
  struct WorldSpace
  {
    float width{}, height{}, depth{};
  } _worldSize;
  glm::mat4 _projection;
  glm::vec3 _position{0.f, 300.f, 1500.f};
  glm::vec3 _upDir{0.f, 1.f, 0.f};
  glm::vec3 _forwardDir{0.f, 0.f, -1.f};
  float _moveSpeed{10};

 private:
  Camera(WorldSpace area);

 public:
  Camera(Camera const& objCopy) = delete;
  Camera(Camera&& objMove) = delete;
  Camera& operator=(Camera const& objCopy) = delete;
  Camera& operator=(Camera&& objMove) = delete;
  static Camera& GetCamera();
  Uniform<glm::mat4, float> View();
  Uniform<glm::mat4, float> const Projection() const;
  Uniform<glm::vec2, float> const Viewport() const;
  void ProjectionRescale(int width, int height);
  void Move(ImGuiKey direction);
  void Rotate(glm::vec3 rotation);
  void AddMoveSpeed(float delta_speed);
  float GetMoveSpeed();
};