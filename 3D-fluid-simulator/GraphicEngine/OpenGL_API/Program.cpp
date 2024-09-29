#include "Program.h"

#include <utility>

Program::Program()
{
  _id = _(glCreateProgram());
}

Program::Program(
    std::initializer_list<std::pair<GLenum, std::string>> shaderSourceWithTypes)
    : Program()
{
  for (auto& [type, filepath] : shaderSourceWithTypes)
  {
    AddShader(type, filepath);
  }
}

// Program::Program(Program const& objCopy)
//	:_shaderList{ objCopy._shaderList }
//{
//	_id = _(glCreateProgram());
// }

Program::Program(Program&& objMove)
    : _shaderList{std::exchange(objMove._shaderList, {})},
      _id{std::exchange(objMove._id, {})}
{
}

// Program& Program::operator=(Program const& objCopy)
//{
//	this->~Program();
//	_id = _(glCreateProgram());
//	_shaderList = objCopy._shaderList;
//	return *this;
// }

Program& Program::operator=(Program&& objMove)
{
  if (this->_id != objMove._id)
  {
    this->~Program();
    _id = std::exchange(objMove._id, 0);
  }
  _shaderList = std::exchange(objMove._shaderList, {});
  return *this;
}

Program::~Program()
{
  if (_id != 0)
  {
    _(glDeleteProgram(_id));
  }
  _id = 0;
}

void Program::AddShader(GLenum type, std::string const& filepath)
{
  _shaderList.emplace_back(type, filepath);
}

void Program::Link() const
{
  AttachShaders();
  _(glLinkProgram(_id));
  GLint linkStatus{};
  _(glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus));
  if (linkStatus == GL_FALSE)
  {
    GLint logLength{};
    _(glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength));
    std::vector<GLchar> logInfo(logLength > 0 ? logLength : 1, '\0');
    _(glGetProgramInfoLog(_id, logLength, nullptr, logInfo.data()));
    LOG << "Unable to link " << logInfo.data() << " deleting program."
        << std::endl;
    std::cout << "Unable to link " << logInfo.data() << " deleting program."
              << std::endl;
  }
  DetachShaders();
}

bool Program::isLinked() const
{
  GLint linkStatus{};
  _(glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus));
  return linkStatus == GL_TRUE ? true : false;
}

void Program::Bind() const
{
  _(glUseProgram(_id));
}

void Program::Unbind() const
{
  _(glUseProgram(0));
}

GLuint const Program::ID() const
{
  return _id;
}

void Program::AttachShaders() const
{
  for (auto& shader : _shaderList)
  {
    if (shader.ID() != 0)
    {
      if (!shader.isCompiled())
      {
        shader.Compile();
      }
      _(glAttachShader(_id, shader.ID()));
    }
  }
}

void Program::DetachShaders() const
{
  for (auto& shader : _shaderList)
  {
    if (shader.ID() != 0)
    {
      _(glDetachShader(_id, shader.ID()));
    }
  }
}
