#pragma once

#include <Debug.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>

class Shader {
private:
	std::string _source;
	GLuint _id{ 0 };
public:
	Shader(GLenum type, std::string const& filepath);
	Shader(Shader const& objCopy) = delete;
	Shader(Shader&& objMove);
	Shader& operator=(Shader const& objCopy) = delete;
	Shader& operator=(Shader&& objMove);
	~Shader();
	GLuint const& ID() const;
	void Compile() const;
	bool isCompiled() const;
	void AddSource(GLenum type, std::string const& filepath);
};
