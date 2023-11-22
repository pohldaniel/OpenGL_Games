#ifndef __shaderH__
#define __shaderH__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Extension.h"
#include "Vector.h"

class Shader {

public:
	Shader() = default;
	Shader(const char* vertex, const char* fragment, bool fromFile = true);
	Shader(const char* vertex, const char* fragment, const char* geometry, bool fromFile = true);
	Shader(const char* compute, bool fromFile = true);
	Shader(Shader* shader);
	~Shader();

	void loadFromFile(std::string vertex, std::string fragment);
	void loadFromResource(std::string vertex, std::string fragment);
	void loadFromFile(std::string vertex, std::string fragment, std::string geometry);
	void loadFromResource(std::string vertex, std::string fragment, std::string geometry);
	void loadFromFile(std::string compute);
	void loadFromResource(std::string compute);
	Shader& get();

	void loadMatrix(const char* location, const Matrix4f& matrix, bool trans = false);
	void loadMatrixArray(const char* location, const std::vector<Matrix4f> matrixArray, const unsigned short count, bool trans = false);

	void loadVector(const char* location, Vector4f vector);
	void loadVector(const char* location, Vector3f vector);
	void loadVector(const char* location, Vector2f vector);
	void loadFloat(const char* location, float value);
	void loadFloat2(const char* location, float value[2]);
	void loadFloat3(const char* location, float value[3]);
	void loadFloat4(const char* location, float value[4]);
	void loadFloatArray(const char* location, float *value, const unsigned short count);
	void loadBool(const char* location, bool value);
	void loadInt(const char* location, int value);
	void loadUnsignedInt(const char* location, unsigned int value);

	GLuint m_program;

protected:

	GLuint createProgramFromFile(std::string vertex, std::string fragment);
	GLuint createProgram(std::string vertex, std::string fragment);
	GLuint createProgramFromFile(std::string vertex, std::string fragment, std::string geometry);
	GLuint createProgram(std::string vertex, std::string fragment, std::string geometry);
	GLuint createProgramFromFile(std::string compute);
	GLuint createProgram(std::string compute);

	void readTextFile(const char *pszFilename, std::string &buffer);
	GLuint loadShaderProgram(GLenum type, const char *pszFilename);
	GLuint loadShaderProgram(GLenum type, std::string buffer);
	GLuint compileShader(GLenum type, const char *pszSource);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader);
	GLuint linkShaders(GLuint vertShader, GLuint fragShader, GLuint geoShader);
	GLuint linkShaders(GLuint compShader);
	void cleanup();

private:

	unsigned int getUnifromLocation(const std::string& name) const;

	mutable std::unordered_map<std::string, unsigned int> m_uniformLocationCache;
};
#endif // __shaderH__