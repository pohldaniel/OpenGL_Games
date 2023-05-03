#include "cShader.h"

cShader::cShader(){}
cShader::~cShader(){}

std::string get_file_contents(const char *filename)
{
  std::ifstream ifs(filename);
  std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
  return content;
}

void glcppShaderSource(GLuint shader, std::string const &shader_string)
{
    GLchar const *shader_source = shader_string.c_str();
    GLint const shader_length = shader_string.size();

    glShaderSource(shader, 1, &shader_source, &shader_length);
}

void load_shader(GLuint shaderobject, char * const shadersourcefilename)
{
    glcppShaderSource(shaderobject,get_file_contents(shadersourcefilename));
}

void cShader::Load()
{
	GLuint shader;

	programs[PROGRAM_SIMPLE_TERRAIN] = glCreateProgram();
		shader = glCreateShader(GL_VERTEX_SHADER);
		load_shader(shader,"Shaders/simple.vert");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_SIMPLE_TERRAIN], shader);

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader(shader,"Shaders/terrain.frag");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_SIMPLE_TERRAIN], shader);
	glLinkProgram(programs[PROGRAM_SIMPLE_TERRAIN]);

	programs[PROGRAM_SIMPLE_LIGHTBEAM] = glCreateProgram();
		shader = glCreateShader(GL_VERTEX_SHADER);
		load_shader(shader,"Shaders/simple.vert");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_SIMPLE_LIGHTBEAM], shader);

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader(shader,"Shaders/lightbeam.frag");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_SIMPLE_LIGHTBEAM], shader);
	glLinkProgram(programs[PROGRAM_SIMPLE_LIGHTBEAM]);

	programs[PROGRAM_SIMPLE_LIGHTBALL] = glCreateProgram();
		shader = glCreateShader(GL_VERTEX_SHADER);
		load_shader(shader,"Shaders/simple.vert");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_SIMPLE_LIGHTBALL], shader);

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader(shader,"Shaders/lightball.frag");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_SIMPLE_LIGHTBALL], shader);
	glLinkProgram(programs[PROGRAM_SIMPLE_LIGHTBALL]);

	programs[PROGRAM_COMPLEX_NORMALMAP_LAVAGLOW] = glCreateProgram();
		shader = glCreateShader(GL_VERTEX_SHADER);
		load_shader(shader,"Shaders/complex.vert");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_COMPLEX_NORMALMAP_LAVAGLOW], shader);

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader(shader,"Shaders/lavaglow.frag");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_COMPLEX_NORMALMAP_LAVAGLOW], shader);
	glLinkProgram(programs[PROGRAM_COMPLEX_NORMALMAP_LAVAGLOW]);

	programs[PROGRAM_COMPLEX_NORMALMAP] = glCreateProgram();
		shader = glCreateShader(GL_VERTEX_SHADER);
		load_shader(shader,"Shaders/complex.vert");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_COMPLEX_NORMALMAP], shader);

		shader = glCreateShader(GL_FRAGMENT_SHADER);
		load_shader(shader,"Shaders/normalmap.frag");
		glCompileShader(shader);
		glAttachShader(programs[PROGRAM_COMPLEX_NORMALMAP], shader);
	glLinkProgram(programs[PROGRAM_COMPLEX_NORMALMAP]);

	glDeleteShader(shader);
}

void cShader::Activate(int program_id)
{
	glUseProgram(programs[program_id]);
	current_prog_id = program_id;
}

void cShader::Deactivate()
{
	glUseProgram(0);
}