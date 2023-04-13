#pragma once
#ifndef __SHADER_H__
#define __SHADER_H__

#include <GL/glew.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <unordered_map>
#include <string>

#include "engine/Vector.h"

class ShaderVo {
public:
	ShaderVo();
	~ShaderVo();

	GLuint CreateProgram(char* vertexShader, char* fragmentShader);
	GLuint CreateProgram(char* vertexShader, char* geomShader, char* fragmentShader);

	inline void ActivateProgram() { glUseProgram(m_program); }
	inline void DeactivateProgram() { glUseProgram(0); }

	void SetBool(GLchar* name, bool value) const;
	void SetInt(GLchar* name, int value) const;
	void SetFloat(const char* name, float value) const;
	void SetVec2(const char* name, glm::vec2 value) const;
	void SetVec3(const char* name, glm::vec3 value) const;
	void SetVec4(GLchar* name, glm::vec4 value) const;
	void SetMat4(GLchar* name, glm::mat4 value) const;

	void loadMatrix(const char* location, const Matrix4f& matrix, bool trans = false);
	void loadVector(const char* location, Vector3f vector);
	void loadInt(const char* location, int value);
	GLuint GetShaderProgram() { return m_program; }

private:
	enum { VERT_SHADER, FRAG_SHADER, TOTAL_SHADERS };

	GLuint m_shaderID, m_program;

	std::string LoadShaderFromFile(char* shaderFile);
	GLuint CreateShader(GLenum, std::string);

	unsigned int getUnifromLocation(const std::string& name) const;

	mutable std::unordered_map<std::string, unsigned int> m_uniformLocationCache;
};

#endif // !__SHADER_H__