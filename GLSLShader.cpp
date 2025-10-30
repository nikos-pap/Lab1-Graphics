#include "GLSLShader.h"
#include <iostream>
#include <fstream>
#include <GL/glew.h>

std::vector<uint8_t> ReadSPIRV(const std::string& filename) {
	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		std::cout << "Failed to open SPIR-V file: " << filename << std::endl;
		return std::vector<uint8_t>{};
	}

	file.seekg(0, std::ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> spirvData;
	spirvData.resize(fileSize);
	file.read(reinterpret_cast<char*>(spirvData.data()), fileSize);

	return spirvData;
}

GLSLShader::GLSLShader(const std::string& filepath)
	: m_FilePath{filepath}, m_RendererID{0}
{
	ParseShader(filepath);
}

GLSLShader::GLSLShader(const std::string& vertFilepath, const std::string& fragFilepath)
	: m_FilePath{ "" }, m_RendererID{ 0 }
{
	if (vertFilepath.find(".vert") != std::string::npos && fragFilepath.find(".frag") != std::string::npos) {
		ShaderProgramSource source;
		source.VertexSource = "";
		source.FragmentSource = "";
		std::ifstream vStream(vertFilepath);
		if (vStream.is_open()) {
			std::string line;
			std::stringstream ss;
			while (getline(vStream, line)) {
				ss << line << '\n';
			}
			source.VertexSource = ss.str();
		}
		else {
			std::cout << "Failed to open vertex shader file: " << vertFilepath << std::endl;
		}
		std::ifstream fStream(fragFilepath);
		if (fStream.is_open()) {
			std::string line;
			std::stringstream ss;
			while (getline(fStream, line)) {
				ss << line << '\n';
			}
			source.FragmentSource = ss.str();
		}
		else {
			std::cout << "Failed to open fragment shader file: " << fragFilepath << std::endl;
		}
		m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
	}
	else if (vertFilepath.find(".spv") != std::string::npos && fragFilepath.find(".spv") != std::string::npos) {
		std::vector<uint8_t> vertexSPVbytes = ReadSPIRV(vertFilepath);
		std::vector<uint8_t> fragmentSPVbytes = ReadSPIRV(fragFilepath);

		m_RendererID = CreateSpirVShader(vertexSPVbytes, fragmentSPVbytes);
	}
	else {
		std::cout << "Invalid vertex and/or fragment shader filename.\n";
	}
}

GLSLShader::GLSLShader()
	: m_RendererID{ 0 } {
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

void GLSLShader::ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath); // TODO: check if file opened successfully

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	if (filepath.find(".shader") != std::string::npos) {
		while (getline(stream, line)) {
			if (line.find("#shader") != std::string::npos) {
				if (line.find("vertex") != std::string::npos) {
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos) {
					type = ShaderType::FRAGMENT;
				}
			}
			else {
				ss[(int)type] << line << '\n';
			}
		}
		m_RendererID = CreateShader(ss[0].str(), ss[1].str());
	}
	else {
		if (filepath.find(".slang") != std::string::npos) {
			std::string source = std::string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
			std::vector<ShaderOutput> slangGLSLOutput = slangCompiler.compileToSPIRV(
				source,
				{ "vertexMain", "fragmentMain" });
			if (slangGLSLOutput.size() == 2) {
				m_RendererID = CreateSpirVShader(slangGLSLOutput[0].binaryData, slangGLSLOutput[1].binaryData);
				//std::cout << "Fragment GLSL Shader:\n" << slangGLSLOutput[1].asText() << std::endl;
				//m_RendererID = CreateShader(slangGLSLOutput[0].asText(), slangGLSLOutput[1].asText());
			}
		}
		else {
			std::cout << "Unsupported shader file format for parsing: " << filepath << std::endl;
		}
	}
}

uint32_t GLSLShader::CompileShader(uint32_t type, const std::string& source) {
	uint32_t id = glCreateShader(type);
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
uint32_t GLSLShader::CompileSpirVShader(uint32_t type, const std::vector<uint8_t>& SPV) {
	uint32_t id = glCreateShader(type);
	glShaderBinary(1, &id, GL_SHADER_BINARY_FORMAT_SPIR_V, SPV.data(), SPV.size());
	glSpecializeShader(id, "main", 0, 0, 0);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)malloc(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " SPIR-V shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		free(message);
		return 0;
	}
	return id;
}
uint32_t GLSLShader::CreateSpirVShader(const std::vector<uint8_t>& VertexSPV, const std::vector<uint8_t>& FragmentSPV) {
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileSpirVShader(GL_VERTEX_SHADER, VertexSPV);
	uint32_t fs = CompileSpirVShader(GL_FRAGMENT_SHADER, FragmentSPV);
	if ( 0 == vs || 0 == fs) {
		std::cout << "SPIR-V shader compilation failed." << std::endl;
		glDeleteProgram(program);
		return 0;
	}
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	int32_t isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		int32_t maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		char* infoLog = (char*)malloc(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
		std::cout << "Shader linking failed: " << infoLog << std::endl;
		free(infoLog);
		glDeleteProgram(program);
		return 0;
	}
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;

}

uint32_t GLSLShader::CreateShader(const std::string& vertexGLSLShader, const std::string& fragmentGLSLShader) {
	uint32_t program = glCreateProgram();
	uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexGLSLShader);
	uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentGLSLShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	int32_t isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		int32_t maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		char* infoLog = (char*)malloc(maxLength * sizeof(char));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);
		std::cout << "Shader linking failed: " << infoLog << std::endl;
		free(infoLog);
		glDeleteProgram(program);
		return 0;
	}
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}