#include "BuiltInShader.h"

extern const unsigned int BuiltInShader::materialBinding = 0;
extern const unsigned int BuiltInShader::viewBinding = 1;
extern const unsigned int BuiltInShader::lightBinding = 2;
extern const unsigned int BuiltInShader::matrixBinding = 3;
extern const unsigned int BuiltInShader::sphereBinding = 4;

extern unsigned int BuiltInShader::materialUbo = 0;
extern unsigned int BuiltInShader::viewUbo = 0;
extern unsigned int BuiltInShader::lightUbo = 0;
extern unsigned int BuiltInShader::matrixUbo = 0;
extern unsigned int BuiltInShader::sphereUbo = 0;

extern void BuiltInShader::UpdateViewUbo(const Camera& camera) {
	glBindBuffer(GL_UNIFORM_BUFFER, viewUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, &camera.getViewMatrix()[0][0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}