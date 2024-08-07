#pragma once

#include <GL/glew.h>
#include <Debug.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>


class Shader {
private:
	std::string _source;
	uint32_t _id{ 0 };
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
