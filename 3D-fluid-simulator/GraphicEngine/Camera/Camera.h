#pragma once

#include <imgui.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/matrix.hpp>

#include "Uniform.h"

/**
 * @brief Singleton representing the camera present in the simulation
 *
 */
class Camera
{
 private:
  struct WorldSpace
  {
    float width{}, height{}, depth{};
  } _worldSize;
  glm::mat4 _projection;
  glm::vec3 _position{0.f, 300.f, 500.f};
  glm::vec3 _upDir{0.f, 1.f, 0.f};
  glm::vec3 _forwardDir{0.f, -0.3f, -1.f};
  float _moveSpeed{10};

 private:
  /**
   * @brief Allows for creation a camera object for predefined world size
   *
   * @param area
   */
  Camera(WorldSpace area);

 public:
  /**
   * @brief Explicit deletion of specific operator and constructors to prevent
   * changing the camera singleton
   *
   * @param objCopy
   */
  Camera(Camera const& objCopy) = delete;
  Camera(Camera&& objMove) = delete;
  Camera& operator=(Camera const& objCopy) = delete;
  Camera& operator=(Camera&& objMove) = delete;
  /**
   * @brief Allows to get the singleton object
   *
   * @return Camera&
   */
  static Camera& GetCamera();
  /**
   * @brief Returns the view matrix for a specified camera object
   *
   * @return Uniform<glm::mat4, float> view matrix
   */
  Uniform<glm::mat4, float> View();
  /**
   * @brief Returns the projection matrix for a camera object for a given
   * context size
   *
   * @return Uniform<glm::mat4, float> projection matrix
   */
  Uniform<glm::mat4, float> const Projection() const;
  /**
   * @brief Returns cached size of a viewport for a given OpenGL context
   *
   * @return Uniform<glm::mat4, float> Vector containing width and height of a
   * viewport
   */
  Uniform<glm::vec2, float> const Viewport() const;
  /**
   * @brief Helper function needed to rescale projection matrix depending on a
   * width and height ratio of a viewport
   *
   * @param width width of a context
   * @param height height of a context
   */
  void ProjectionRescale(int width, int height);
  /**
   * @brief Method that allows to move camera in a specified direction in three
   * dimensions
   *
   * @param direction direction enum value
   */
  void Move(ImGuiKey direction);
  /**
   * @brief Method that allows to rotate camera in three planes
   *
   * @param rotation rotation vector
   */
  void Rotate(glm::vec3 rotation);
  /**
   * @brief Method that allows to change the camera movement speed
   *
   * @param delta_speed relative speed
   */
  void AddMoveSpeed(float delta_speed);
  /**
   * @brief Getter that allows to retrieve actual camera speed
   *
   * @return float camera movement speed value
   */
  float GetMoveSpeed();
};