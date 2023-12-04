#include "shaderRo.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <GL/glew.h>

ShaderRo::ShaderRo(const std::string& filepathVertex, const std::string& filepathFragment)
	: m_FilePathVertex(filepathVertex), m_FilePathFragment(filepathFragment), m_rendererID(0)
{
	std::string vertexShader = parseShader(filepathVertex);
	std::string fragmentShader = parseShader(filepathFragment);
	m_rendererID = createShader(vertexShader, fragmentShader);
}

ShaderRo::~ShaderRo() {
	glDeleteProgram(m_rendererID);
}

std::string ShaderRo::parseShader(const std::string& filepath) {
	std::ifstream stream(filepath);
	if (!stream) {
		return "";
	}

	std::string shader = "";
	std::string tempLine = "";
	while (getline(stream, tempLine)) {
		shader += tempLine + '\n';
	}
	stream.close();
	return shader.c_str();
}

unsigned int ShaderRo::compileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		auto const typeString = [type]() {
			switch (type) {
				case GL_VERTEX_SHADER: return "fragment";
				case GL_FRAGMENT_SHADER: return "vertex";
				case GL_TESS_EVALUATION_SHADER: return "tesselation evaluation";
				case GL_TESS_CONTROL_SHADER: return "tesselation control";
				case GL_GEOMETRY_SHADER: return "geometry";
				case GL_COMPUTE_SHADER: return "compute";
				default: return "unknown type";
			}
		}();

		
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int ShaderRo::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void ShaderRo::bind() const {
	glUseProgram(m_rendererID);
}

void ShaderRo::unbind() const {
	glUseProgram(0);
}

void ShaderRo::setUniform1i(const std::string& name, int value) {
	glUniform1i(getUniformLocation(name), value);
}

void ShaderRo::setUniform1f(const std::string& name, float v0) {
	glUniform1f(getUniformLocation(name), v0);
}

void ShaderRo::setUniform2f(const std::string& name, float v0, float v1) {
	glUniform2f(getUniformLocation(name), v0, v1);
}

void ShaderRo::setUniform3f(const std::string& name, float v0, float v1, float v2) {
	glUniform3f(getUniformLocation(name), v0, v1, v2);
}

void ShaderRo::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
	glUniform4f(getUniformLocation(name), v0, v1, v2, v3);
}

void ShaderRo::setUniform4f(const std::string& name, glm::vec4 v) {
	glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
}

void ShaderRo::setUniformMat4f(const std::string& name, const glm::mat4 matrix) {
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}

int ShaderRo::getUniformLocation(const std::string& name) {
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
		return m_UniformLocationCache[name];
	}

	int location = glGetUniformLocation(m_rendererID, name.c_str());
	if (location == -1) {
		
	}
	m_UniformLocationCache[name] = location;
	return location;
}
