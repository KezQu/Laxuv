#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <string>
#include <type_traits>

#include "Debug.h"
#include "GL/glew.h"
#include "imgui.h"

template <typename T>
concept has_z = requires(T value)
{
  value.z;
};

template <typename T>
concept has_w = requires(T value)
{
  value.w;
};

template <typename T>
concept is_vec4 = requires(T value)
{
  value.x;
  value.y;
  value.z;
  value.w;
};

template <typename T>
concept is_vec3 = requires(T value)
{
  value.x;
  value.y;
  value.z;
}
&&!has_w<T>;

template <typename T>
concept is_vec2 = requires(T value)
{
  value.x;
  value.y;
}
&&!has_z<T>;

template <typename T>
concept is_mat4 = requires(T value)
{
  value[0].x;
  value[1].y;
  value[2].z;
  value[3][0];
};

template <typename T, typename U>
concept is_div = requires(T value1, U value2)
{
  value1 / value2;
}
|| requires(T value1, U value2)
{
  value1.x / value2;
};
template <typename T, typename U>
concept is_mul = requires(T value1, U value2)
{
  value1* value2;
}
|| requires(T value1, U value2)
{
  value1.x* value2;
};

/**
 * @brief Helper struct that stores properties of the value
 *
 * @tparam T
 */
template <typename T>
struct ValueProperties
{
  T _min;
  T _max;
  float _scale;
  std::string _precision;

  /**
   * @brief Constructs new object for an integer value type
   *
   */
  explicit ValueProperties(
      T min = std::numeric_limits<T>::min() / static_cast<T>(2),
      T max = std::numeric_limits<T>::max() / static_cast<T>(2),
      float scale = 1.f,
      std::string precision = "%d") requires std::integral<T> ||
      (std::integral<T> && (is_vec2<T> || is_vec3<T> || is_mat4<T>))
      : _min{min},
  _max{max}, _scale{scale}, _precision{precision}
  {
  }
  /**
   * @brief Constructs new object for a floating point value type
   *
   */
  explicit ValueProperties(
      T min = -std::numeric_limits<T>::max() / static_cast<T>(2),
      T max = std::numeric_limits<T>::max() / static_cast<T>(2),
      float scale = 1.f,
      std::string precision = "%.1f") requires std::floating_point<T> ||
      (std::floating_point<T> && (is_vec2<T> || is_vec3<T> || is_mat4<T>))
      : _min{min},
  _max{max}, _scale{scale}, _precision{precision}
  {
  }
};

/**
 * @brief Wrapper for uniform values based on the SFINAE concept depending on
 * the type of the value stored in the uniform
 *
 * @tparam T Type of the value stored
 * @tparam PropertyT Type of the value properties
 */
template <typename T, typename PropertyT = T>
class Uniform
{
 private:
  std::string _uniform_name;
  ValueProperties<PropertyT> _properties;
  T _value;

 public:
  using ui_control = std::function<void()>;
  /**
   * @brief Creates an uniform wrapper for a given value with specified
   * properties
   *
   * @param value value to be stored in the uniform
   * @param uniform_name name of the uniform corresponding to the value name in
   * the shaders
   * @param properties value properties
   */
  Uniform(T value, std::string uniform_name,
          ValueProperties<PropertyT> properties = ValueProperties<PropertyT>{})
      : _value{value}, _uniform_name{uniform_name}, _properties{properties}
  {
  }
  /**
   * @brief Map uniform to the given program id
   *
   * @param program_id id of the program to bind the uniform to
   */
  void MapUniform(uint32_t program_id) const
  {
    // Checks whether uniform exists and if exists binds it to the program
    SetUniformIfExists(program_id);
  }
  /**
   * @brief Assignment operator overload to allow assigning raw value to the
   * internal uniform value
   *
   * @param value value to assign to
   * @return Uniform<T, PropertyT> const&
   */
  Uniform<T, PropertyT>& operator=(T value)
  {
    _value = value;
    return *this;
  }
  /**
   * @brief overload to allow adding raw value to the internal value stored in
   * Uniform
   *
   * @tparam U Type of the value to add to
   * @param value Value to add to internal value
   * @return Uniform<T, PropertyT>&
   */
  template <std::convertible_to<T> U>
  Uniform<T, PropertyT>& operator+=(U value)
  {
    _value += value;
    return *this;
  }
  /**
   * @brief overload to allow dividing internal uniform value by raw value
   *
   * @tparam U Type of the value to divide by
   * @param value Value to used in division
   * @return Uniform<T, PropertyT>&
   */
  template <is_div<T> U>
  Uniform<T, PropertyT>& operator/=(U value)
  {
    _value /= value;
    return *this;
  }
  /**
   * @brief Multiplication overload allowing to retrieve result of the
   * multiplication of the internal value with specified value
   *
   * @tparam U Type of the value to multiply by
   * @param value
   * @return decltype(_value * value)
   */
  template <is_mul<T> U>
  auto operator*(U value) -> decltype(_value * value)
  {
    return _value * value;
  }
  /**
   * @brief Overload allowing to retrieve result of the
   * division of the internal value by specified value
   *
   * @tparam U Type of the value to divide by
   * @param value
   * @return decltype(_value * value)
   */
  template <is_div<T> U>
  auto operator/(U value) -> decltype(_value / value)
  {
    return _value / value;
  }
  /**
   * @brief Get the uniform internal value
   *
   * @return T&
   */
  T& GetValue()
  {
    return _value;
  }
  /**
   * @brief Get the uniform internal value (read-only)
   *
   * @return T&
   */
  T const& GetValue() const
  {
    return _value;
  }
  /**
   * @brief Get the uniform properties
   *
   * @return ValueProperties<PropertyT>&
   */
  ValueProperties<PropertyT>& GetProperties()
  {
    return _properties;
  }

  /**
   * @brief Method that allows to expose uniform value to the user interface by
   * providing the differrent control depending on a value type
   *
   * @param action_callback Callback that is invoked when the value has changed
   * @return ui_control
   */
  ui_control ExposeToUI(std::function<void()> const action_callback =
                            nullptr) requires std::signed_integral<T>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragInt(("##" + _uniform_name).c_str(), &_value, 1.f,
                         _properties._min, _properties._max,
                         _properties._precision.c_str(),
                         ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }
  ui_control ExposeToUI(std::function<void()> const action_callback =
                            nullptr) requires std::unsigned_integral<T>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragScalar(("##" + _uniform_name).c_str(), ImGuiDataType_U32,
                            (void*)&_value, 1.f, (const void*)&_properties._min,
                            (const void*)&_properties._max,
                            _properties._precision.c_str(),
                            ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }

  ui_control ExposeToUI(std::function<void()> const action_callback =
                            nullptr) requires std::floating_point<T>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragFloat(("##" + _uniform_name).c_str(), &_value, 0.1f,
                           _properties._min, _properties._max,
                           _properties._precision.c_str(),
                           ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }

  ui_control ExposeToUI(std::function<void()> const action_callback =
                            nullptr) requires is_vec2<T>&& std::
      floating_point<typename T::value_type>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragFloat2(("##" + _uniform_name).c_str(),
                            glm::value_ptr(_value), 0.1f, _properties._min,
                            _properties._max, _properties._precision.c_str(),
                            ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }

  ui_control ExposeToUI(
      std::function<void()> const action_callback =
          nullptr) requires is_vec3<T>&& std::integral<typename T::value_type>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragInt3(("##" + _uniform_name).c_str(),
                          glm::value_ptr(_value), 1.f, _properties._min,
                          _properties._max, _properties._precision.c_str(),
                          ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }

  ui_control ExposeToUI(std::function<void()> const action_callback =
                            nullptr) requires is_vec3<T>&& std::
      floating_point<typename T::value_type>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragFloat3(("##" + _uniform_name).c_str(),
                            glm::value_ptr(_value), 0.1f, _properties._min,
                            _properties._max, _properties._precision.c_str(),
                            ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }

  ui_control ExposeToUI(std::function<void()> const action_callback =
                            nullptr) requires is_vec4<T>&& std::
      floating_point<typename T::value_type>
  {
    auto const callback = action_callback;
    return [this, callback]() {
      if (ImGui::DragFloat4(("##" + _uniform_name).c_str(),
                            glm::value_ptr(_value), 0.1f, _properties._min,
                            _properties._max, _properties._precision.c_str(),
                            ImGuiSliderFlags_AlwaysClamp))
      {
        if (callback) callback();
      }
    };
  }
  ///////////////////////////////////////////////////////////////////////////////
 private:
  /**
   * @brief Method that retrieves location of a uniform in the specified program
   * and binds it using correct OpenGL function based on a uniform value type
   *
   * @param program_id id that the program is bound to
   */
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
      requires is_vec2<T>&& std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform2fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
      requires is_vec3<T>&& std::signed_integral<typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(glm::vec3{_value} * _properties._scale);
    if (location != -1) glUniform3iv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
      requires is_vec3<T>&& std::unsigned_integral<typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(glm::vec3{_value} * _properties._scale);
    if (location != -1)
      glUniform3uiv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
      requires is_vec3<T>&& std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform3fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
      requires is_vec3<T>&& std::same_as<double, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform3dv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
      requires is_vec4<T>&& std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1) glUniform4fv(location, 1, glm::value_ptr(scaled_value));
  }

  void SetUniformIfExists(uint32_t program_id) const
      requires is_mat4<T>&& std::same_as<float, typename T::value_type>
  {
    auto location = CheckIfExists(program_id, _uniform_name);
    T scaled_value = static_cast<T>(_value * _properties._scale);
    if (location != -1)
      glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(scaled_value));
  }

  /**
   * @brief Helper method that checks if uniform exists in the given program
   * under specified name
   *
   * @param program_id id of a program in which the uniform should be present
   * @param uniform_name name of the unifrom to check
   * @return uint32_t retrieved location of a uniform in the program or -1 if
   * not found
   */
  uint32_t CheckIfExists(uint32_t const program_id,
                         std::string const uniform_name) const
  {
    auto location = glGetProgramResourceLocation(program_id, GL_UNIFORM,
                                                 uniform_name.c_str());
    return location;
  }
};
