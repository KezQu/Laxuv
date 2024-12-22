#include "Program.h"

#include <initializer_list>
#include <iostream>
#include <string>
#include <utility>

#include "Debug.h"
#include "GL/glew.h"

Program::Program()
{
  // Create new program and bind it to the _id
  _id = _(glCreateProgram());
}

Program::Program(
    std::initializer_list<std::pair<GLenum, std::string>> shaderSourceWithTypes)
    : Program()
{
  // Add shaders to the memory
  for (auto& [type, filepath] : shaderSourceWithTypes)
  {
    AddShader(type, filepath);
  }
}

Program::Program(Program&& objMove)
    : _shaderList{std::exchange(objMove._shaderList, {})},
      _id{std::exchange(objMove._id, {})}
{
}

Program& Program::operator=(Program&& objMove)
{
  // Prevent trying to move object to ifself
  if ((glIsProgram(this->_id) == GL_TRUE) && (this->_id != objMove._id))
  {
    this->~Program();
    // Move program id to preserve bound to it OpenGL program and exchange it
    // with default id value
    _id = std::exchange(objMove._id, 0);
  }
  _shaderList = std::exchange(objMove._shaderList, {});
  return *this;
}

Program::~Program()
{
  // Request OpenGL to delete program bound to the id if id points to the valid
  // program
  if (glIsProgram(_id) == GL_TRUE)
  {
    _(glDeleteProgram(_id));
  }
  _id = 0;
}

void Program::AddShader(GLenum type, std::string const& filepath)
{
  // Create new shader at the end of the shader list containing shaders meant to
  // be used by current program
  _shaderList.emplace_back(type, filepath);
}

void Program::Link() const
{
  // Attach all shaders present in the objects' memory to the program
  AttachShaders();
  // Try to link program present at the _id
  _(glLinkProgram(_id));
  GLint linkStatus{};
  _(glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus));
  // Check if the linking was successful
  if (linkStatus == GL_FALSE)
  {
    // Retrieve linking error and log it into the application console
    GLint logLength{};
    _(glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLength));
    std::vector<GLchar> logInfo(logLength > 0 ? logLength : 1, '\0');
    _(glGetProgramInfoLog(_id, logLength, nullptr, logInfo.data()));
    LOG << "Unable to link " << logInfo.data() << " deleting program."
        << std::endl;
  }
  // After successful linking detach used shaders to allow their deletion
  DetachShaders();
}

bool Program::isLinked() const
{
  // Retrieve information whether the program was linked
  GLint linkStatus{};
  _(glGetProgramiv(_id, GL_LINK_STATUS, &linkStatus));
  return linkStatus == GL_TRUE ? true : false;
}

void Program::Bind() const
{
  // Request OpenGL to use executables present in program bound to the _id
  _(glUseProgram(_id));
}

void Program::Unbind() const
{
  // Request OpenGL to use program 0 which corresponds to invalid program which
  // effectively unbinds previously used the program
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
    // Compile stored shaders when their id is bound to valid shader and hasn't
    // been compiled before
    if (shader.ID() != 0)
    {
      if (!shader.isCompiled())
      {
        shader.Compile();
      }
      // Attach valid compiled shader to current program
      _(glAttachShader(_id, shader.ID()));
    }
  }
}

void Program::DetachShaders() const
{
  for (auto& shader : _shaderList)
  {
    // Detach all valid shaders from current program
    if (shader.ID() != 0)
    {
      _(glDetachShader(_id, shader.ID()));
    }
  }
}
