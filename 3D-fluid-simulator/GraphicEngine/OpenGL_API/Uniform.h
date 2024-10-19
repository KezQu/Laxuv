#pragma once

#include <Debug.h>
#include <GL/glew.h>

#include <concepts>
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <type_traits>

template <typename T>
concept is_vec3 = requires(T value) {
  value.x;
  value.y;
  value.z;
};

template <typename T>
concept has_z = requires(T value) { value.z; };

template <typename T>
concept is_vec2 = requires(T value) {
  value.x;
  value.y;
} && !has_z<T>;

template <typename T>
concept is_mat4 = requires(T value) {
  value[0].x;
  value[1].y;
  value[2].z;
  value[3][0];
};

template <typename T, typename U>
concept is_div = requires(T value1, U value2) { value1 / value2; } ||
                 requires(T value1, U value2) { value1.x / value2; };
template <typename T, typename U>
concept is_mul = requires(T value1, U value2) { value1* value2; } ||
                 requires(T value1, U value2) { value1.x* value2; };

template <typename T>
class Uniform
{
 private:
  std::string _uniform_name;
  T _value;
  float _scale;

 public:
  Uniform(T value, std::string uniform_name, float scale = 1.f)
      : _value{value}, _scale{scale}, _uniform_name{uniform_name}
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
  template <std::convertible_to<T> U>
  Uniform<T>& operator+=(U value)
  {
    _value += value;
    return *this;
  }
  template <is_div<T> U>
  Uniform<T>& operator/=(U value)
  {
    _value /= value;
    return *this;
  }
  template <is_mul<T> U>
  auto operator*(U value) -> decltype(_value * value)
  {
    return _value * value;
  }
  template <is_div<T> U>
  auto operator/(U value) -> decltype(_value / value)
  {
    return _value / value;
  }
  T& GetValue()
  {
    return _value;
  }
  T const& GetValue() const
  {
    return _value;
  }

 private:
  void SetUniformIfExists(uint32_t program_id) const
    requires std::signed_integral<T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1) glUniform1i(location, static_cast<T>(_value * _scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires std::unsigned_integral<T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1) glUniform1ui(location, static_cast<T>(_value * _scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires std::same_as<float, T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1) glUniform1f(location, static_cast<T>(_value * _scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires std::same_as<double, T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1) glUniform1d(location, static_cast<T>(_value * _scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec2<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _scale);
    if (location != -1) glUniform2fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::signed_integral<T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _scale);
    if (location != -1) glUniform3iv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::unsigned_integral<T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _scale);
    if (location != -1)
      glUniform3uiv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _scale);
    if (location != -1) glUniform3fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::same_as<double, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _scale);
    if (location != -1) glUniform3dv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_mat4<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _scale);
    if (location != -1)
      glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(scaled_value));
  }

  uint32_t CheckIfExists(uint32_t const program_id,
                         std::string const uniform_name) const
  {
    auto location = glGetProgramResourceLocation(program_id, GL_UNIFORM,
                                                 uniform_name.c_str());
    return location;
  }
};
