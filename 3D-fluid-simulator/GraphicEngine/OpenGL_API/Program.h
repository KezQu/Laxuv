#pragma once

#include <vector>

#include "Shader.h"

/**
 * @brief Class representing OpenGL program used further for rendering and
 * calculating essential data using Compute Shaders
 *
 */
class Program
{
 private:
  std::vector<Shader> _shaderList;
  GLuint _id{0};

 public:
  /**
   * @brief Create program without any shaders
   *
   */
  Program();
  /**
   * @brief Constructs new program object with initial shaders passed to the
   * constructor
   *
   * @param shaderSourceWithTypes initial shaders with their OpenGL types
   */
  Program(std::initializer_list<std::pair<GLenum, std::string>>
              shaderSourceWithTypes);
  /**
   * @brief Explicit deletion of copy constructor to disallow copying programs
   *
   * @param objCopy
   */
  Program(Program const& objCopy) = delete;
  Program(Program&& objMove);
  /**
   * @brief Explicit deletion of copy operator to disallow copying programs
   *
   * @param objCopy
   */
  Program& operator=(Program const& objCopy) = delete;
  Program& operator=(Program&& objMove);
  ~Program();
  /**
   * @brief Add new shader to current program of a specified type
   *
   * @param type type of the shader indicating the part of the rendering
   * pipeline they are part of
   * @param filepath filepath to the shader to be attached to the program
   */
  void AddShader(GLenum type, std::string const& filepath);
  /**
   * @brief Links shaders present in the memory to be included in the pipeline
   *
   */
  void Link() const;
  /**
   * @brief Checks whether current program was linked
   *
   * @return flag specifying the state whether the program was linked
   */
  bool isLinked() const;
  /**
   * @brief Binds current program to be a part of a rendering state
   *
   */
  void Bind() const;
  /**
   * @brief Unbinds current program from the rendering pipeline
   *
   */
  void Unbind() const;
  /**
   * @brief Getter to retrieve unique program id
   *
   * @return GLuint const
   */
  GLuint const ID() const;

 private:
  /**
   * @brief Helper method to compile and attach all shaders present in the
   * objects' memory
   *
   */
  void AttachShaders() const;
  /**
   * @brief Helper method to compile and attach all shaders present in the
   * objects' memory
   *
   */
  void DetachShaders() const;
};
