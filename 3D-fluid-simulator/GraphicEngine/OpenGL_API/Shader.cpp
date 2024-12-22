#include "Shader.h"

#include <utility>

Shader::Shader(GLenum type, std::string const& filepath) : _type{type}
{
  // Create shader object with a given type
  _id = _(glCreateShader(type));
  // Retrieve source code from a file present at a given filepath
  AddSource(filepath);
}

Shader::Shader(Shader&& objMove)
{
  *this = std::move(objMove);
}

Shader& Shader::operator=(Shader&& objMove)
{
  // Ensure that the shader object is not moved to itself
  if (this->_id != objMove._id)
  {
    this->~Shader();
    // Move shader id to preserve bound to it OpenGL shader and exchange it with
    // default id value
    _id = std::exchange(objMove._id, 0);
  }
  _source = std::exchange(objMove._source, {});
  return *this;
}

Shader::~Shader()
{
  // Request OpenGL to delete shader bound to the id if id points to the valid
  // shader object
  if (glIsShader(_id) == GL_TRUE)
  {
    _(glDeleteShader(_id));
  }
}

GLuint const Shader::ID() const
{
  return _id;
}

void Shader::Compile() const
{
  // Compile shader source code
  _(glCompileShader(_id));
  int32_t compileStatus{};
  _(glGetShaderiv(_id, GL_COMPILE_STATUS, &compileStatus));
  // Check if shader was compiled successfully
  if (compileStatus == GL_FALSE)
  {
    // Retrieve compilation error and log it into the application console
    int32_t logLength{};
    _(glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength));
    std::vector<char> logInfo(logLength, '\0');
    _(glGetShaderInfoLog(_id, logLength, nullptr, logInfo.data()));
    LOG << "Unable to compile " << logInfo.data() << "." << std::endl;
  }
}

bool Shader::isCompiled() const
{
  // Request OpenGL for the compilation status of the shader bound to the _id
  int32_t compileStatus{};
  _(glGetShaderiv(_id, GL_COMPILE_STATUS, &compileStatus));
  return compileStatus == GL_TRUE ? true : false;
}

void Shader::AddSource(std::string const& filepath)
{
  // Recreate absolute path to the shader source code
  std::filesystem::path filePath(__FILE__);
  std::ifstream shaderFile(filePath.parent_path().parent_path().string() +
                           "/shaders" + filepath);
  // Check whether the file was opened successfully
  if (!shaderFile.is_open())
  {
    LOG << "Unable to open shader : " << filepath << std::endl;
  }
  else
  {
    // Save the content of a shader source file to the memory
    while (!shaderFile.eof())
    {
      char line[1024];
      shaderFile.getline(line, 1024, '\n');
      _source += std::string(line) + "\n";
    }
  }
  char const* sourceCode = _source.c_str();
  // Use the retrieved source code and attach it to the shader object bound to
  // the _id
  _(glShaderSource(_id, 1, &sourceCode, 0));
}
