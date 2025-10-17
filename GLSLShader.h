#pragma once
#include <string>
#include <unordered_map>
#include <sstream>
#include <glm/glm.hpp>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class GLSLShader {
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
public:
	GLSLShader(const std::string& filepath);
	GLSLShader();
	~GLSLShader();

	void Bind() const;
	void Unbind() const;

private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexGLSLShader, const std::string& fragmentGLSLShader);
};