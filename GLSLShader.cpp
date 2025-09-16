#include "GLSLShader.h"

#include <iostream>
#include <fstream>
#include <GL/glew.h>

GLSLShader::GLSLShader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

GLSLShader::~GLSLShader()
{
	glDeleteProgram(m_RendererID);
}

void GLSLShader::Bind() const
{
	glUseProgram(m_RendererID);
}

void GLSLShader::Unbind() const
{
	glUseProgram(0);
}

void GLSLShader::SetUniform1i(const std::string& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void GLSLShader::SetUniform1f(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void GLSLShader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void GLSLShader::SetUniform4f(const std::string& name, float * v/*v0, float v1, float v2, float v3*/)
{
	glUniform4f(GetUniformLocation(name), v[0], v[1], v[2], v[3]);
}

void GLSLShader::SetUniform3f(const std::string& name, float v0, float v1, float v2)
{
	glUniform3f(GetUniformLocation(name), v0, v1, v2);
}
void GLSLShader::SetUniform3f(const std::string& name, float* v/*v0, float v1, float v2, float v3*/)
{
	glUniform3f(GetUniformLocation(name), v[0], v[1], v[2]);
}

void GLSLShader::SetUniform3fv(const std::string& name, glm::vec3& v0)
{
	glUniform3fv(GetUniformLocation(name),1, &v0[0]);
}

void GLSLShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int GLSLShader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}
	int location = glGetUniformLocation(m_RendererID, name.c_str());
	if (location == -1) {
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	std::cout << name << ":" << location << std::endl;
	return location;
}

ShaderProgramSource GLSLShader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				//set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				//set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

unsigned int GLSLShader::CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		free(message);
		return 0;
	}

	return id;
}

unsigned int GLSLShader::CreateShader(const std::string& vertexGLSLShader, const std::string& fragmentGLSLShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexGLSLShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentGLSLShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}