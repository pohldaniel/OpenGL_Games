#ifndef __shaderH__
#define __shaderH__

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "Vector.h"

class Shader {

public:
	Shader();
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

	
	void attachShader(GLuint compShader);
	void linkShaders();
	static GLuint LoadShaderProgram(GLenum type, const char *pszFilename);
	static GLuint LoadShaderProgram(GLenum type, std::string buffer);
	GLuint m_program;

protected:

	void createProgramFromFile(std::string vertex, std::string fragment);
	void createProgram(std::string vertex, std::string fragment);
	void createProgramFromFile(std::string vertex, std::string fragment, std::string geometry);
	void createProgram(std::string vertex, std::string fragment, std::string geometry);
	void createProgramFromFile(std::string compute);
	void createProgram(std::string compute);

	void linkShaders(GLuint compShader);
	void linkShaders(GLuint vertShader, GLuint fragShader);
	void linkShaders(GLuint vertShader, GLuint fragShader, GLuint geoShader);
	
	void cleanup();

	static void ReadTextFile(const char *pszFilename, std::string &buffer);
	static GLuint CompileShader(GLenum type, const char *pszSource);

private:

	unsigned int getUnifromLocation(const std::string& name) const;

	mutable std::unordered_map<std::string, unsigned int> m_uniformLocationCache;
};
#endif // __shaderH__