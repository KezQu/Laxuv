#pragma once

#include <Debug.h>
#include <GL/glew.h>
#include <imgui.h>

#include <concepts>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <string>
#include <type_traits>

template <typename T>
concept has_z = requires(T value) { value.z; };

template <typename T>
concept has_w = requires(T value) { value.w; };

template <typename T>
concept is_vec4 = requires(T value) {
  value.x;
  value.y;
  value.z;
  value.w;
};

template <typename T>
concept is_vec3 = requires(T value) {
  value.x;
  value.y;
  value.z;
} && !has_w<T>;

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
struct ValueProperties
{
  T _min;
  T _max;
  float _scale;
  std::string _precision;

  explicit ValueProperties(T min = std::numeric_limits<T>::min() /
                                   static_cast<T>(2),
                           T max = std::numeric_limits<T>::max() /
                                   static_cast<T>(2),
                           float scale = 1.f, std::string precision = "%d")
    requires std::integral<T> ||
                 (std::integral<T> && (is_vec2<T> || is_vec3<T> || is_mat4<T>))
      : _min{min}, _max{max}, _scale{scale}, _precision{precision}
  {
  }
  explicit ValueProperties(T min = -std::numeric_limits<T>::max() /
                                   static_cast<T>(2),
                           T max = std::numeric_limits<T>::max() /
                                   static_cast<T>(2),
                           float scale = 1.f, std::string precision = "%.1f")
    requires std::floating_point<T> ||
                 (std::floating_point<T> &&
                  (is_vec2<T> || is_vec3<T> || is_mat4<T>))
      : _min{min}, _max{max}, _scale{scale}, _precision{precision}
  {
  }
};

template <typename T, typename PropertyT = T>
class Uniform
{
 private:
  std::string _uniform_name;
  ValueProperties<PropertyT> _properties;
  T _value;

 public:
  using ui_control = std::function<void()>;

  Uniform(T value, std::string uniform_name,
          ValueProperties<PropertyT> properties = ValueProperties<PropertyT>{})
      : _value{value}, _uniform_name{uniform_name}, _properties{properties}
  {
  }

  void MapUniform(uint32_t program_id) const
  {
    SetUniformIfExists(program_id);
  }
  Uniform<T, PropertyT> const& operator=(T value) const
  {
    _value = value;
    return *this;
  }
  Uniform<T, PropertyT>& operator=(T value)
  {
    _value = value;
    return *this;
  }
  template <std::convertible_to<T> U>
  Uniform<T, PropertyT>& operator+=(U value)
  {
    _value += value;
    return *this;
  }
  template <is_div<T> U>
  Uniform<T, PropertyT>& operator/=(U value)
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
  ValueProperties<PropertyT>& GetProperties()
  {
    return _properties;
  }

  ui_control ExposeToUI()
    requires std::signed_integral<T>
  {
    return [this]()
    {
      ImGui::DragInt(("##" + _uniform_name).c_str(), &_value, 1.f,
                     _properties._min, _properties._max,
                     _properties._precision.c_str(),
                     ImGuiSliderFlags_AlwaysClamp);
    };
  }
  ui_control ExposeToUI()
    requires std::unsigned_integral<T>
  {
    return [this]()
    {
      ImGui::DragScalar(
          ("##" + _uniform_name).c_str(), ImGuiDataType_U32, (void*)&_value,
          1.f, (const void*)&_properties._min, (const void*)&_properties._max,
          _properties._precision.c_str(), ImGuiSliderFlags_AlwaysClamp);
    };
  }

  ui_control ExposeToUI()
    requires std::floating_point<T>
  {
    return [this]()
    {
      ImGui::DragFloat(("##" + _uniform_name).c_str(), &_value, 0.1f,
                       _properties._min, _properties._max,
                       _properties._precision.c_str(),
                       ImGuiSliderFlags_AlwaysClamp);
    };
  }

  ui_control ExposeToUI()
    requires is_vec2<T> && std::floating_point<typename T::value_type>
  {
    return [this]()
    {
      ImGui::DragFloat2(("##" + _uniform_name).c_str(), glm::value_ptr(_value),
                        0.1f, _properties._min, _properties._max,
                        _properties._precision.c_str(),
                        ImGuiSliderFlags_AlwaysClamp);
    };
  }

  ui_control ExposeToUI()
    requires is_vec3<T> && std::integral<typename T::value_type>
  {
    return [this]()
    {
      ImGui::DragInt3(("##" + _uniform_name).c_str(), glm::value_ptr(_value),
                      1.f, _properties._min, _properties._max,
                      _properties._precision.c_str(),
                      ImGuiSliderFlags_AlwaysClamp);
    };
  }

  ui_control ExposeToUI()
    requires is_vec3<T> && std::floating_point<typename T::value_type>
  {
    return [this]()
    {
      ImGui::DragFloat3(("##" + _uniform_name).c_str(), glm::value_ptr(_value),
                        0.1f, _properties._min, _properties._max,
                        _properties._precision.c_str(),
                        ImGuiSliderFlags_AlwaysClamp);
    };
  }

  ui_control ExposeToUI()
    requires is_vec4<T> && std::floating_point<typename T::value_type>
  {
    return [this]()
    {
      ImGui::DragFloat4(("##" + _uniform_name).c_str(), glm::value_ptr(_value),
                        0.1f, _properties._min, _properties._max,
                        _properties._precision.c_str(),
                        ImGuiSliderFlags_AlwaysClamp);
    };
  }
  ///////////////////////////////////////////////////////////////////////////////
 private:
  void SetUniformIfExists(uint32_t program_id) const
    requires std::signed_integral<T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1)
      glUniform1i(location, static_cast<T>(_value * _properties._scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires std::unsigned_integral<T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1)
      glUniform1ui(location, static_cast<T>(_value * _properties._scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires std::same_as<float, T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1)
      glUniform1f(location, static_cast<T>(_value * _properties._scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires std::same_as<double, T>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    if (location != -1)
      glUniform1d(location, static_cast<T>(_value * _properties._scale));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec2<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform2fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::signed_integral<typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform3iv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::unsigned_integral<typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1)
      glUniform3uiv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform3fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec3<T> && std::same_as<double, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform3dv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_vec4<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform4fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
    requires is_mat4<T> && std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
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
