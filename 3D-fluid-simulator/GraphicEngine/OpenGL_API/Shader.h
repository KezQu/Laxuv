#pragma once

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <vector>

#include "Debug.h"
#include "GL/glew.h"

/**
 * @brief Class providing RAII functionality for the OpenGL shader object
 *
 */
class Shader
{
 private:
  std::string _source;
  uint32_t _id{0};
  GLenum _type;

 public:
  /**
   * @brief Constructs new shader object with a given type and path to the
   * source code
   *
   * @param type Type of the shader determining the part of the pipeline its
   * part of
   * @param filepath path to the source code of the shader
   */
  Shader(GLenum type, std::string const& filepath);
  /**
   * @brief Explicit deletion of copy constructor to ensure that every shader is
   * unique
   *
   * @param objCopy
   */
  Shader(Shader const& objCopy) = delete;
  Shader(Shader&& objMove);
  /**
   * @brief Explicit deletion of copy operator to ensure that every shader is
   * unique
   *
   * @param objCopy
   */
  Shader& operator=(Shader const& objCopy) = delete;
  Shader& operator=(Shader&& objMove);
  ~Shader();
  /**
   * @brief Getter to retrieve id of the shader object
   *
   * @return uint32_t const
   */
  uint32_t const ID() const;
  /**
   * @brief Compile shaders' source code
   *
   */
  void Compile() const;
  /**
   * @brief Check if shader was compiled before
   *
   * @return flag indicating if shader was compiled
   */
  bool isCompiled() const;
  /**
   * @brief Method that allows to add source code to the empty shader object
   *
   * @param filepath path to the source code
   */
  void AddSource(std::string const& filepath);
};
