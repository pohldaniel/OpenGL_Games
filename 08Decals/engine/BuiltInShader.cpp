#include "BuiltInShader.h"

extern const unsigned int BuiltInShader::materialBinding = 3;
extern const unsigned int BuiltInShader::viewBinding = 4;

extern unsigned int BuiltInShader::materialUbo = 0;
extern unsigned int BuiltInShader::viewUbo = 0;


extern void BuiltInShader::UpdateViewUbo(const Camera& camera) {
	glBindBuffer(GL_UNIFORM_BUFFER, viewUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &camera.getViewMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}