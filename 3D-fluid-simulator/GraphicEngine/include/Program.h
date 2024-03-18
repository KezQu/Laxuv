#pragma once

#include <Shader.h>
#include <vector>

class Program {
private:
	std::vector<Shader> _shaderList;
	GLuint _id{ 0 };
public:
	Program();
	Program(std::initializer_list<std::pair<GLenum, std::string>> shaderSourceWithTypes);
	Program(Program const& objCopy) = delete;
	Program(Program&& objMove);
	Program& operator=(Program const& objCopy) = delete;
	Program& operator=(Program&& objMove);
	~Program();
	void AddShader(GLenum type, std::string const& filepath);
	void Link() const;
	bool isLinked() const;
	void Bind() const;
	void Unbind() const;
	GLuint const ID() const;
private:
	void AttachShaders() const;
	void DetachShaders() const;
};
