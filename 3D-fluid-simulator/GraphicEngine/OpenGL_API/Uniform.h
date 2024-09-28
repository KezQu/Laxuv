#pragma once

#include <Debug.h>
#include <GL/glew.h>

#include <concepts>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <type_traits>

template <typename T>
concept is_ivec3 = requires(T value) {
  {
    value.x
  } -> std::signed_integral;
  value.y;
  value.z;
};
template <typename T>
concept is_uvec3 = requires(T value) {
  {
    value.x
  } -> std::unsigned_integral;
  value.y;
  value.z;
};
template <typename T>
concept is_vec3 = requires(T value) {
  {
    value.x
  } -> std::same_as<float>;
  value.y;
  value.z;
};
template <typename T>
concept is_dvec3 = requires(T value) {
  {
    value.x
  } -> std::same_as<double>;
  value.y;
  value.z;
};
template <typename T>
concept is_mat4 = requires(T value) {
  {
    value[0][0]
  } -> std::same_as<float>;
  glm::mat4::length() == 4;
};

template <typename T>
class Uniform
{
 private:
  std::string _uniform_name;
  T _value;

 public:
  Uniform(T value, std::string uniform_name)
      : _value{value}, _uniform_name{uniform_name}
  {
  }
  void MapUniform(uint32_t program_id) const
  {
    SetUniformIfExists(program_id);
  }

  Uniform<T> const& operator=(T value) const
  {
    _value = value;
    return *this;
  }
  Uniform<T>& operator=(T value)
  {
    _value = value;
    return *this;
  }

 private:
  typename std::enable_if_t<std::is_integral_v<T> && std::is_signed_v<T>, void>
  SetUniformIfExists(uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform1i(location, value);
  }

  typename std::enable_if_t<std::is_unsigned_v<T>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform1ui(location, value);
  }

  typename std::enable_if_t<std::is_same_v<T, double>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform1d(location, value);
  }

  typename std::enable_if_t<std::is_same_v<T, float>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform1f(location, value);
  }
  typename std::enable_if_t<is_ivec3<T>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform3iv(location, 1, glm::value_ptr(value));
  }
  typename std::enable_if_t<is_uvec3<T>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform3uiv(location, 1, glm::value_ptr(value));
  }
  typename std::enable_if_t<is_vec3<T>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform3fv(location, 1, glm::value_ptr(value));
  }
  typename std::enable_if_t<is_dvec3<T>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform3dv(location, 1, glm::value_ptr(value));
  }
  typename std::enable_if_t<is_mat4<T>, void> SetUniformIfExists(
      uint32_t program_id)
  {
    auto location = CheckIfExists(program_id, uniform_name);
    if (location != -1) glUniform4fv(location, 1, glm::value_ptr(value));
  }
  uint32_t CheckIfExists(uint32_t const program_id,
                         std::string const uniform_name)
  {
    auto location = glGetProgramResourceLocation(program_id, GL_UNIFORM,
                                                 uniform_name.c_str());
    return location;
  }
};
