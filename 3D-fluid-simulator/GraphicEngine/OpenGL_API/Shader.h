#pragma once

#include <Debug.h>
#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <vector>

class Shader
{
 private:
  std::string _source;
  uint32_t _id{0};
  GLenum _type;

 public:
  Shader(GLenum type, std::string const& filepath);
  Shader(Shader const& objCopy) = delete;
  Shader(Shader&& objMove);
  Shader& operator=(Shader const& objCopy) = delete;
  Shader& operator=(Shader&& objMove);
  ~Shader();
  uint32_t const ID() const;
  void Compile() const;
  bool isCompiled() const;
  void AddSource(std::string const& filepath);
};
