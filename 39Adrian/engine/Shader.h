#ifndef __shaderH__
#define __shaderH__

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <array>

#include "Vector.h"

class Shader {

public:

	Shader();
	Shader(const char* vertex, const char* fragment, bool fromFile = true);
	Shader(const char* vertex, const char* fragment, const char* geometry, bool fromFile = true);
	Shader(const char* vertex, const char* fragment, const char* tessControl, const char* tessEvaluation, bool fromFile = true);
	Shader(const char* compute, bool fromFile = true);
	Shader(Shader* shader);
	~Shader();

	void loadFromFile(const std::string& vertex, const std::string& fragment);
	void loadFromResource(const std::string& vertex, const std::string& fragment);
	void loadFromFile(const std::string& vertex, const std::string& fragment, const std::string& geometry);
	void loadFromResource(const std::string& vertex, const std::string& fragment, const std::string& geometry);
	void loadFromFile(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation);
	void loadFromResource(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation);
	void loadFromFile(const std::string& compute);
	void loadFromResource(const std::string& compute);
	
	Shader& get();

	void use(bool force = true);
	void unuse(bool force = true);
	void use() const;
	void unuse() const;
	void loadMatrix(const char* location, const Matrix4f& matrix, bool trans = false) const;
	void loadMatrix(const char* location, const float matrix[16], bool trans = false) const;
	void loadMatrixArray(const char* location, const std::vector<Matrix4f> matrixArray, const unsigned short count, bool trans = false) const;

	void loadVector(const char* location, Vector4f vector) const;
	void loadVector(const char* location, Vector3f vector) const;
	void loadVector(const char* location, Vector2f vector) const;

	void loadVector(const char* location, const float vector[4]) const;
	//void loadVector(const char* location, const float vector[3]) const;
	//void loadVector(const char* location, const float vector[2]) const;

	void loadVector(const char* location, float v0, float v1, float v2, float v3) const;
	void loadVector(const char* location, float v0, float v1, float v2) const;
	void loadVector(const char* location, float v0, float v1) const;

	void loadVector(const char* location, std::array<int, 3> vector) const;
	void loadVector(const char* location, std::array<int, 2> vector) const;
	void loadVectorArray(const char* location, const std::vector<std::array<float, 4>> vectorArray) const;


	void loadFloat(const char* location, float value) const;
	void loadFloat2(const char* location, float value[2]) const;
	void loadFloat3(const char* location, float value[3]) const;
	void loadFloat4(const char* location, float value[4]) const;
	void loadFloatArray(const char* location, float *value, const unsigned short count) const;
	void loadBool(const char* location, bool value) const;
	void loadInt(const char* location, int value) const;
	void loadUnsignedInt(const char* location, unsigned int value) const;
	void attachShader(GLuint compShader, bool reload = false);
	void linkShaders();
	bool inUse();
	unsigned int getUnifromLocation(const std::string& name) const;
	const GLuint& getProgram() const;
	
	static GLuint LoadShaderProgram(GLenum type, const char *pszFilename);
	static GLuint LoadShaderProgram(GLenum type, std::string buffer);
	static GLuint LoadShaderProgram(const std::string& vertex, const std::string& fragment);
	static void SetIncludeFromFile(const char *includeName, const char* filename);
	static void Unuse();

protected:

	GLuint m_program;

private:

	void createProgramFromFile(const std::string& vertex, const std::string& fragment);
	void createProgram(const std::string& vertex, const std::string& fragment);
	void createProgramFromFile(const std::string& vertex, const std::string& fragment, const std::string& geometry);
	void createProgram(const std::string& vertex, const std::string& fragment, const std::string& geometry);
	void createProgramFromFile(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation);
	void createProgram(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation);
	void createProgramFromFile(const std::string& compute);
	void createProgram(const std::string& compute);

	void linkShaders(GLuint compShader);
	static void LinkShaders(GLuint vertShader, GLuint fragShader, const GLuint& program);
	void linkShaders(GLuint vertShader, GLuint fragShader, GLuint geoShader);
	void linkShaders(GLuint vertShader, GLuint fragShader, GLuint tesscShader, GLuint tesseShader);
	
	void cleanup();

	static void ReadTextFile(const char *pszFilename, std::string &buffer);
	static GLuint CompileShader(GLenum type, const char *pszSource);
	static void DeleteShader(GLuint& program);

	bool m_use;
	mutable std::unordered_map<std::string, unsigned int> m_uniformLocationCache;
};
#endif // __shaderH__