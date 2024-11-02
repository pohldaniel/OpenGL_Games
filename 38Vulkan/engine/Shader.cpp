#include "Shader.h"

Shader::Shader() : m_use(false) {
	m_program = glCreateProgram();
}

Shader::Shader(const char* vertex, const char* fragment, bool fromFile) : m_use(false) {
	if (fromFile)
		createProgramFromFile(vertex, fragment);
	else
		createProgram(vertex, fragment);
}

Shader::Shader(const char* vertex, const char* fragment, const char* geometry, bool fromFile) : m_use(false) {
	if (fromFile)
		createProgramFromFile(vertex, fragment, geometry);
	else
		createProgram(vertex, fragment, geometry);
}

Shader::Shader(const char* vertex, const char* fragment, const char* tessControl, const char* tessEvaluation, bool fromFile) : m_use(false) {
	if (fromFile)
		createProgramFromFile(vertex, fragment, tessControl, tessEvaluation);
	else
		createProgram(vertex, fragment, tessControl, tessEvaluation);
}

Shader::Shader(const char* compute, bool fromFile) : m_use(false) {
	if (fromFile)
		createProgramFromFile(compute);
	else
		createProgram(compute);
}

Shader::Shader(Shader* shader) : m_use(false) {
	m_program = shader->m_program;
}

Shader::~Shader() {
	cleanup();
}

void Shader::loadFromFile(const std::string& vertex, const std::string& fragment) {
	createProgramFromFile(vertex, fragment);
}

void Shader::loadFromResource(const std::string& vertex, const std::string& fragment) {
	createProgram(vertex, fragment);
}

void Shader::loadFromFile(const std::string& vertex, const std::string& fragment, const std::string& geometry) {
	createProgramFromFile(vertex, fragment, geometry);
}

void Shader::loadFromResource(const std::string& vertex, const std::string& fragment, const std::string& geometry) {
	createProgram(vertex, fragment, geometry);
}

void Shader::loadFromFile(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation) {
	createProgramFromFile(vertex, fragment, tessControl, tessEvaluation);
}

void Shader::loadFromResource(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation) {
	createProgram(vertex, fragment, tessControl, tessEvaluation);
}

void  Shader::loadFromFile(const std::string& compute) {
	createProgram(compute);
}

void  Shader::loadFromResource(const std::string& compute) {
	createProgram(compute);
}

Shader& Shader::get() {
	return *this;
}

unsigned int Shader::getUnifromLocation(const std::string& name) const {

	if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end()) 
		return m_uniformLocationCache[name];
	
	unsigned int location = glGetUniformLocation(m_program, name.c_str());
	m_uniformLocationCache[name] = location;
	return location;
}

void Shader::use() {
	//if (!m_use) {
		glUseProgram(m_program);
		m_use = true;
	//}
}

void Shader::unuse() {
	//if (m_use) {
		glUseProgram(0);
		m_use = false;
	//}
}

void Shader::use() const {
	glUseProgram(m_program);
}

void Shader::unuse() const {
	glUseProgram(0);
}

void Shader::loadMatrix(const char* location, const Matrix4f& matrix, bool trans) const {
	glUniformMatrix4fv(getUnifromLocation(location), 1, trans, &matrix[0][0]);
}

void Shader::loadMatrix(const char* location, const float matrix[16], bool trans) const {
	glUniformMatrix4fv(getUnifromLocation(location), 1, trans, matrix);
}

void Shader::loadMatrixArray(const char* location, const std::vector<Matrix4f> matrixArray, const unsigned short count, bool trans) const {
	glUniformMatrix4fv(getUnifromLocation(location), count, trans, matrixArray[0][0]);
}

void Shader::loadVector(const char* location, Vector4f vector) const {
	glUniform4fv(getUnifromLocation(location), 1, &vector[0]);
}

void Shader::loadVector(const char* location, Vector3f vector) const {
	glUniform3fv(getUnifromLocation(location), 1, &vector[0]);
}

void Shader::loadVector(const char* location, Vector2f vector) const {
	glUniform2fv(getUnifromLocation(location), 1, &vector[0]);
}

void Shader::loadVector(const char* location, const float vector[4]) const {
	glUniform4fv(getUnifromLocation(location), 1, vector);
}

/*void Shader::loadVector(const char* location, const float vector[3]) const {
	glUniform3fv(getUnifromLocation(location), 1, vector);
}

void Shader::loadVector(const char* location, const float vector[2]) const {
	glUniform2fv(getUnifromLocation(location), 1, vector);
}*/

void Shader::loadVector(const char* location, float v0, float v1) const {
	glUniform2f(getUnifromLocation(location), v0, v1);
}

void Shader::loadVector(const char* location, float v0, float v1, float v2) const {
	glUniform3f(getUnifromLocation(location), v0, v1, v2);
}

void Shader::loadVector(const char* location, float v0, float v1, float v2, float v3) const {
	glUniform4f(getUnifromLocation(location), v0, v1, v2, v3);
}

void Shader::loadVector(const char* location, std::array<int, 3> vector) const {
	glUniform3iv(getUnifromLocation(location), 1, &vector[0]);
}

void Shader::loadVector(const char* location, std::array<int, 2> vector) const {
	glUniform2iv(getUnifromLocation(location), 1, &vector[0]);
}

void Shader::loadVectorArray(const char* location, const std::vector<std::array<float, 4>> vectorArray) const {
	glUniform4fv(getUnifromLocation(location), static_cast<int>(vectorArray.size()), vectorArray.data()->data());
}

void Shader::loadFloat(const char* location, float value) const {
	glUniform1f(getUnifromLocation(location), value);
}

void Shader::loadFloat2(const char* location, float value[2]) const {
	glUniform2f(getUnifromLocation(location), value[0], value[1]);
}

void Shader::loadFloat3(const char* location, float value[3]) const {
	glUniform1fv(getUnifromLocation(location), 3, value);
}

void Shader::loadFloat4(const char* location, float value[4]) const {
	glUniform4f(getUnifromLocation(location), value[0], value[1], value[2], value[3]);
}

void Shader::loadFloatArray(const char* location, float *value, const unsigned short count) const {
	glUniform1fv(getUnifromLocation(location), count, value);
}

void Shader::loadInt(const char* location, int value) const {
	glUniform1i(getUnifromLocation(location), value);
}

void Shader::loadUnsignedInt(const char* location, unsigned int value) const {
	glUniform1ui(getUnifromLocation(location), value);
}

void Shader::loadBool(const char* location, bool value) const {
	glUniform1i(getUnifromLocation(location), value);
}

void Shader::createProgramFromFile(const std::string& vertex, const std::string& fragment) {
	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, vertex.c_str());
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, fragment.c_str());
	m_program = glCreateProgram();
	linkShaders(vshader, fshader);
}

void Shader::createProgram(const std::string& vertex, const std::string& fragment) {
	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, vertex);
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, fragment);
	m_program = glCreateProgram();
	linkShaders(vshader, fshader);
}

void Shader::createProgramFromFile(const std::string& vertex, const std::string& fragment, const std::string& geometry) {
	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, vertex.c_str());
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, fragment.c_str());
	GLuint gshader = LoadShaderProgram(GL_GEOMETRY_SHADER, geometry.c_str());
	m_program = glCreateProgram();
	linkShaders(vshader, fshader, gshader);
}

void Shader::createProgram(const std::string& vertex, const std::string& fragment, const std::string& geometry) {
	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, vertex);
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, fragment);
	GLuint gshader = LoadShaderProgram(GL_GEOMETRY_SHADER, geometry);
	m_program = glCreateProgram();
	linkShaders(vshader, fshader, gshader);
}

void Shader::createProgramFromFile(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation) {
	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, vertex.c_str());
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, fragment.c_str());
	GLuint tcshader = LoadShaderProgram(GL_TESS_CONTROL_SHADER, tessControl.c_str());
	GLuint teshader = LoadShaderProgram(GL_TESS_EVALUATION_SHADER, tessEvaluation.c_str());
	m_program = glCreateProgram();
	linkShaders(vshader, fshader, tcshader, teshader);
}

void Shader::createProgram(const std::string& vertex, const std::string& fragment, const std::string& tessControl, const std::string& tessEvaluation) {
	GLuint vshader = LoadShaderProgram(GL_VERTEX_SHADER, vertex);
	GLuint fshader = LoadShaderProgram(GL_FRAGMENT_SHADER, fragment);
	GLuint tcshader = LoadShaderProgram(GL_TESS_CONTROL_SHADER, tessControl);
	GLuint teshader = LoadShaderProgram(GL_TESS_EVALUATION_SHADER, tessEvaluation);
	m_program = glCreateProgram();
	linkShaders(vshader, fshader, tcshader, teshader);
}

void Shader::createProgramFromFile(const std::string& compute) {
	GLuint cshader = LoadShaderProgram(GL_COMPUTE_SHADER, compute.c_str());
	m_program = glCreateProgram();
	linkShaders(cshader);
}

void Shader::createProgram(const std::string& compute) {
	GLuint cshader = LoadShaderProgram(GL_COMPUTE_SHADER, compute);
	m_program = glCreateProgram();
	linkShaders(cshader);
}

void Shader::ReadTextFile(const char *pszFilename, std::string &buffer) {
	std::ifstream file(pszFilename, std::ios::binary);
	if (file.is_open()) {
		file.seekg(0, std::ios::end);

		std::ifstream::pos_type fileSize = file.tellg();

		buffer.resize(static_cast<unsigned int>(fileSize));
		file.seekg(0, std::ios::beg);
		file.read(&buffer[0], fileSize);
	}
}

GLuint Shader::LoadShaderProgram(GLenum type, std::string buffer) {
	GLuint shader = 0;
	if (buffer.length() > 0) {
		shader = CompileShader(type, reinterpret_cast<const char *>(&buffer[0]));
	}

	return shader;
}

GLuint Shader::LoadShaderProgram(GLenum type, const char *pszFilename) {
	GLuint shader = 0;
	std::string buffer;
	ReadTextFile(pszFilename, buffer);

	if (buffer.length() > 0) {
		shader = CompileShader(type, reinterpret_cast<const char *>(&buffer[0]));
	}else {
		std::cout << "Could not find: " << pszFilename << std::endl;
	}

	return shader;
}

GLuint Shader::CompileShader(GLenum type, const char *pszSource) {
	std::cout << pszSource << std::endl;
	GLuint shader = glCreateShader(type);

	if (shader) {
		GLint compiled = 0;

		glShaderSource(shader, 1, &pszSource, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled) {

			GLsizei logSizeInfo = 0;	
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSizeInfo);

			std::string logInfo;
			logInfo.resize(logSizeInfo);
			glGetShaderInfoLog(shader, logSizeInfo, &logSizeInfo, &logInfo[0]);
			std::cout << "Compile status: \n" << logInfo << std::endl;

			GLsizei logSizeProgram = 0;
			std::string logProgram;
			logProgram.resize(256);

			glGetProgramInfoLog(shader, 256, &logSizeProgram, &logProgram[0]);
			std::cout << "Error Message: \n" << logProgram << std::endl;
		}
	}
	return shader;
}

void Shader::linkShaders(GLuint vertShader, GLuint fragShader) {
	if (m_program) {
		GLint linked = 0;

		if (vertShader)
			glAttachShader(m_program, vertShader);

		if (fragShader)
			glAttachShader(m_program, fragShader);

		glLinkProgram(m_program);

		glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

		if (!linked) {
			GLsizei logSizeInfo = 0;	
			glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &logSizeInfo);

			std::string logInfo;
			logInfo.resize(logSizeInfo);
			glGetShaderInfoLog(m_program, logSizeInfo, &logSizeInfo, &logInfo[0]);
			std::cout << "Compile status: \n" << logInfo << std::endl;

			GLsizei logSizeProgram = 0;
			std::string logProgram;
			logProgram.resize(256);

			glGetProgramInfoLog(m_program, 256, &logSizeProgram, &logProgram[0]);
			std::cout << "Error Message: \n" << logProgram << std::endl;
		}

		// Mark the two attached shaders for deletion. These two shaders aren't
		// deleted right now because both are already attached to a shader
		// program. When the shader program is deleted these two shaders will
		// be automatically detached and deleted.
		if (vertShader)
			glDeleteShader(vertShader);

		if (fragShader)
			glDeleteShader(fragShader);
	}
}

void Shader::linkShaders(GLuint vertShader, GLuint fragShader, GLuint geoShader) {

	if (m_program) {

		GLint linked = 0;

		if (vertShader)
			glAttachShader(m_program, vertShader);

		if (fragShader)
			glAttachShader(m_program, fragShader);

		if (geoShader)
			glAttachShader(m_program, geoShader);

		glLinkProgram(m_program);

		glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

		if (!linked) {

			GLsizei logSizeInfo = 0;
			glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &logSizeInfo);

			std::string logInfo;
			logInfo.resize(logSizeInfo);
			glGetShaderInfoLog(m_program, logSizeInfo, &logSizeInfo, &logInfo[0]);
			std::cout << "Compile status: \n" << logInfo << std::endl;

			GLsizei logSizeProgram = 0;
			std::string logProgram;
			logProgram.resize(256);

			glGetProgramInfoLog(m_program, 256, &logSizeProgram, &logProgram[0]);
			std::cout << "Error Message: \n" << logProgram << std::endl;
		}

		if (vertShader)
			glDeleteShader(vertShader);

		if (fragShader)
			glDeleteShader(fragShader);

		if (geoShader)
			glDeleteShader(geoShader);

	}

}

void Shader::linkShaders(GLuint vertShader, GLuint fragShader, GLuint tesscShader, GLuint tesseShader) {
	if (m_program) {

		GLint linked = 0;

		if (vertShader)
			glAttachShader(m_program, vertShader);

		if (fragShader)
			glAttachShader(m_program, fragShader);

		if (tesscShader)
			glAttachShader(m_program, tesscShader);

		if (tesseShader)
			glAttachShader(m_program, tesseShader);

		glLinkProgram(m_program);

		glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

		if (!linked) {

			GLsizei logSizeInfo = 0;
			glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &logSizeInfo);

			std::string logInfo;
			logInfo.resize(logSizeInfo);
			glGetShaderInfoLog(m_program, logSizeInfo, &logSizeInfo, &logInfo[0]);
			std::cout << "Compile status: \n" << logInfo << std::endl;

			GLsizei logSizeProgram = 0;
			std::string logProgram;
			logProgram.resize(256);

			glGetProgramInfoLog(m_program, 256, &logSizeProgram, &logProgram[0]);
			std::cout << "Error Message: \n" << logProgram << std::endl;
		}

		if (vertShader)
			glDeleteShader(vertShader);

		if (fragShader)
			glDeleteShader(fragShader);

		if (tesscShader)
			glDeleteShader(tesscShader);

		if (tesseShader)
			glDeleteShader(tesseShader);

	}
}

void Shader::linkShaders(GLuint compShader) {

	if (m_program) {
		GLint linked = 0;

		if (compShader)
			glAttachShader(m_program, compShader);

		glLinkProgram(m_program);

		glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

		if (!linked) {
			GLsizei logSizeInfo = 0;
			glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &logSizeInfo);

			std::string logInfo;
			logInfo.resize(logSizeInfo);
			glGetShaderInfoLog(m_program, logSizeInfo, &logSizeInfo, &logInfo[0]);
			std::cout << "Compile status: \n" << logInfo << std::endl;

			GLsizei logSizeProgram = 0;
			std::string logProgram;
			logProgram.resize(256);

			glGetProgramInfoLog(m_program, 256, &logSizeProgram, &logProgram[0]);
			std::cout << "Error Message: \n" << logProgram << std::endl;
		}

		if (compShader)
			glDeleteShader(compShader);

	}
}

void Shader::attachShader(GLuint shader, bool reload) {
	if (m_program) {

		if (reload) {
			glDeleteProgram(m_program);
			m_program = glCreateProgram();
			m_uniformLocationCache.clear();

		}

		if (shader) {
			glAttachShader(m_program, shader);
			glDeleteShader(shader);
		}
	}
}

void Shader::linkShaders() {
	if (m_program) {
		GLint linked = 0;

		glLinkProgram(m_program);

		glGetProgramiv(m_program, GL_LINK_STATUS, &linked);

		if (!linked) {
			GLsizei logSizeInfo = 0;
			glGetShaderiv(m_program, GL_INFO_LOG_LENGTH, &logSizeInfo);

			std::string logInfo;
			logInfo.resize(logSizeInfo);
			glGetShaderInfoLog(m_program, logSizeInfo, &logSizeInfo, &logInfo[0]);
			std::cout << "Compile status: \n" << logInfo << std::endl;

			GLsizei logSizeProgram = 0;
			std::string logProgram;
			logProgram.resize(256);

			glGetProgramInfoLog(m_program, 256, &logSizeProgram, &logProgram[0]);
			std::cout << "Error Message: \n" << logProgram << std::endl;
		}
	
	}
}

bool Shader::inUse() {
	return m_use;
}

void Shader::cleanup() {
	if (m_program) {
		glDeleteProgram(m_program);
		m_program = 0;
	}
}

void  Shader::Unuse() {
	glUseProgram(0);
}

void Shader::SetIncludeFromFile(const char *includeName, const char* filename) {
	char tmpstr[200];
	sprintf(tmpstr, "%s/%s", filename, includeName);
	
	std::string buffer;
	ReadTextFile(filename, buffer);

	sprintf(tmpstr, "/%s", includeName);
	glNamedStringARB(GL_SHADER_INCLUDE_ARB, static_cast<int>(strlen(tmpstr)), tmpstr, static_cast<int>(buffer.length()), reinterpret_cast<const char *>(&buffer[0]));
}

const GLuint& Shader::getProgram() const {
	return m_program;
}