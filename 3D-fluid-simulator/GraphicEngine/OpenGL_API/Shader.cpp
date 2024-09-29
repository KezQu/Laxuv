#include "Shader.h"

#include <utility>

Shader::Shader(GLenum type, std::string const& filepath) : _type{type}
{
  _id = _(glCreateShader(type));
  AddSource(filepath);
}

// Shader::Shader(Shader const& objCopy)
//	:_source{objCopy._source},
//	_type{ objCopy._type }
//{
//	_id = _(glCreateShader(_type));
// }

Shader::Shader(Shader&& objMove)
{
  *this = std::move(objMove);
}

// Shader& Shader::operator=(Shader const& objCopy)
//{
//	this->~Shader();
//	_source = objCopy._source;
//	_type = objCopy._type;
//	_id = _(glCreateShader(_type));
//
//	return *this;
// }

Shader& Shader::operator=(Shader&& objMove)
{
  if (this->_id != objMove._id)
  {
    this->~Shader();
    _id = std::exchange(objMove._id, 0);
  }
  _source = std::exchange(objMove._source, {});
  return *this;
}

Shader::~Shader()
{
  if (glIsShader(_id))
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
  _(glCompileShader(_id));
  int32_t compileStatus{};
  _(glGetShaderiv(_id, GL_COMPILE_STATUS, &compileStatus));
  if (compileStatus == GL_FALSE)
  {
    int32_t logLength{};
    _(glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength));
    std::vector<char> logInfo(logLength, '\0');
    _(glGetShaderInfoLog(_id, logLength, nullptr, logInfo.data()));
    LOG << "Unable to compile " << logInfo.data() << "." << std::endl;
  }
}

bool Shader::isCompiled() const
{
  int32_t compileStatus{};
  _(glGetShaderiv(_id, GL_COMPILE_STATUS, &compileStatus));
  return compileStatus == GL_TRUE ? true : false;
}

void Shader::AddSource(std::string const& filepath)
{
  std::filesystem::path filePath(__FILE__);
  std::ifstream shaderFile(filePath.parent_path().parent_path().string() +
                           "/shaders" + filepath);
  if (!shaderFile.is_open())
  {
    LOG << "Unable to open shader : " << filepath << std::endl;
  }
  else
  {
    while (!shaderFile.eof())
    {
      char line[1024];
      shaderFile.getline(line, 1024, '\n');
      _source += std::string(line) + "\n";
    }
  }
  char const* sourceCode = _source.c_str();
  _(glShaderSource(_id, 1, &sourceCode, 0));
}
