#include "Shader.h"

Shader::Shader(GLenum type, std::string const& filepath){
	_id = _(glCreateShader(type));
	AddSource(type, filepath);
}
Shader::Shader(Shader&& objMove){
	*this = std::move(objMove);
}
Shader& Shader::operator=(Shader&& objMove) {
	this->~Shader();
	_source = std::exchange(objMove._source, {});
	_id = std::exchange(objMove._id, 0);
	return *this;
}

Shader::~Shader()
{
	if (_id != 0) {
		_(glDeleteShader(_id));
	}
}
GLuint const Shader::ID() const {
	return _id;
}

void Shader::Compile() const
{
	_(glCompileShader(_id));
	GLint compileStatus{};
	_(glGetShaderiv(_id, GL_COMPILE_STATUS, &compileStatus));
	if (compileStatus == GL_FALSE) {
		GLint logLength{};
		_(glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLength));
		std::vector<GLchar> logInfo(logLength, '\0');
		_(glGetShaderInfoLog(_id, logLength, nullptr, logInfo.data()));
		Log() << "Unable to compile " << logInfo.data() << "." << std::endl;
	}
}

bool Shader::isCompiled() const {
	GLint compileStatus{};
	_(glGetShaderiv(_id, GL_COMPILE_STATUS, &compileStatus));
	return compileStatus == GL_TRUE ? true : false;
}

void Shader::AddSource(GLenum type, std::string const& filepath)
{
	std::filesystem::path filePath(__FILE__);
	std::ifstream shaderFile(filePath.parent_path().string() + filepath);
	if (!shaderFile.is_open()) {
		Log() << "Unable to open shader : " << filepath << std::endl;
	}
	else {
		while (!shaderFile.eof()) {
			char line[1024];
			shaderFile.getline(line, 1024, '\n');
			_source += std::string(line) + "\n";
		}
	}
	GLchar const* sourceCode = _source.c_str();
	_(glShaderSource(_id, 1, &sourceCode, 0));
}