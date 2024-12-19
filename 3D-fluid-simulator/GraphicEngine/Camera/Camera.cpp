#include <Camera.h>

#include "Uniform.h"

// Initialize camera with a way that the simulation scene is perceived
Camera::Camera(WorldSpace area)
    : _worldSize{area},
      _projection{glm::infinitePerspective(
          glm::radians(60.f), _worldSize.width / _worldSize.height, 10.f)}
{
}

Camera& Camera::GetCamera()
{
  // Create a singleton camera object since only one camera can be present in
  // the scene
  static Camera cameraObj({2.f, 2.f, 2.f});
  return cameraObj;
}

Uniform<glm::mat4, float> Camera::View()
{
  // Create a view matrix based on the camera position and orientation, needed
  // to calculate which objects are visible by the camera
  auto view = glm::lookAt(_position, _position + _forwardDir, _upDir);
  return Uniform<glm::mat4, float>{view, "view"};
}

Uniform<glm::mat4, float> const Camera::Projection() const
{
  // Create a matrix that translates object to the perspective way the camera
  // perceives world
  return Uniform<glm::mat4, float>{_projection, "projection"};
}
Uniform<glm::vec2, float> const Camera::Viewport() const
{
  // Retrieve cached OpenGL context size
  return Uniform<glm::vec2, float>{
      glm::vec2{_worldSize.width, _worldSize.height}, "viewport"};
}
void Camera::ProjectionRescale(int width, int height)
{
  // Recalculate world projection depending on a updated viewport size
  _worldSize.width = width;
  _worldSize.height = height;
  _worldSize.depth = 3 * glm::max(_worldSize.width, _worldSize.height);
  _projection = glm::infinitePerspective(
      glm::radians(60.f), _worldSize.width / _worldSize.height, 0.1f);
}
void Camera::Move(ImGuiKey direction)
{
  // Process user input to move the camera around the scene
  auto tmpRightDir = glm::normalize(glm::cross(_forwardDir, _upDir));
  glm::vec3 moveDir{0.f};
  switch (direction)
  {
    case ImGuiKey_W:
      moveDir += _forwardDir;
      break;
    case ImGuiKey_S:
      moveDir -= _forwardDir;
      break;
    case ImGuiKey_A:
      moveDir -= tmpRightDir;
      break;
    case ImGuiKey_D:
      moveDir += tmpRightDir;
      break;
    case ImGuiKey_Q:
      moveDir -= _upDir;
      break;
    case ImGuiKey_E:
      moveDir += _upDir;
      break;
    default:
      break;
  }
  _position += moveDir * _moveSpeed;
}
void Camera::Rotate(glm::vec3 rotation)
{
  // Process user input to change the camera orientation based on the vectors
  // that specify where the camera looks at
  rotation *= 0.1;
  glm::qua<float> lookingSpot = {0.f, _forwardDir};
  auto angle = glm::atan(rotation);
  lookingSpot = glm::rotate(lookingSpot, glm::radians(angle.x), _upDir);
  _forwardDir =
      glm::normalize(glm::vec3(lookingSpot.x, lookingSpot.y, lookingSpot.z));

  auto tmpRightDir = glm::normalize(glm::cross(_forwardDir, _upDir));
  lookingSpot = glm::rotate(lookingSpot, glm::radians(angle.y), tmpRightDir);
  auto nextforwardDir =
      glm::normalize(glm::vec3(lookingSpot.x, lookingSpot.y, lookingSpot.z));

  tmpRightDir = glm::cross(nextforwardDir, _upDir);
  if (glm::dot(tmpRightDir, tmpRightDir) > 5e-5)
  {
    _forwardDir = nextforwardDir;
  }
}

void Camera::AddMoveSpeed(float delta_speed)
{
  // Increase camera movement speed based on user preferences
  if (_moveSpeed <= 1)
  {
    _moveSpeed += delta_speed * 1e-1;
  }
  else
  {
    _moveSpeed += delta_speed;
  }
  if (_moveSpeed < 0)
  {
    _moveSpeed = 0;
  }
}

float Camera::GetMoveSpeed()
{
  // Allow retrieving camera speed to expose value to user interface
  return _moveSpeed;
}
