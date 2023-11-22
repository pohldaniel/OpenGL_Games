#if !defined(EXTENSION_H)
#define EXTENSION_H
#define NOMINMAX
#include <windows.h>
#include "GL.H"


extern "C" {
#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_DYNAMIC_DRAW 0x88E8
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 1.2
	//
#define GL_BGR							  0x80E0
#define GL_BGRA                           0x80E1
#define GL_RGB8I						  0x8D8F
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_R8                             0x8229
#define GL_RGB32F_ARB					  0x8815
#define GL_RG							  0x8227
#define GL_R32F							  0x822E
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 1.4
	//
#define GL_ARRAY_BUFFER                   0x8892
#define GL_STATIC_DRAW                    0x88E4
#define GL_DEPTH_COMPONENT16              0x81A5
#define GL_GENERATE_MIPMAP                0x8191
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1						  0x84C1
#define GL_TEXTURE2						  0x84C2
#define	GL_TEXTURE3						  0x84C3
#define GL_TEXTURE4						  0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP       0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_DEPTH_COMPONENT24			  0x81A6
#define GL_DEPTH_COMPONENT32F             0x8CAC
#define GL_TEXTURE_COMPARE_MODE           0x884C
#define GL_COMPARE_REF_TO_TEXTURE         0x884E
#define GL_TEXTURE_COMPARE_FUNC           0x884D
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 1.5
	//
#define GL_ELEMENT_ARRAY_BUFFER                   0x8893
#define GL_BUFFER_SIZE                            0x8764

#define GL_MULTISAMPLE							  0x809D
	// GL types for handling large vertex buffer objects.
	typedef ptrdiff_t GLintptr;
	typedef ptrdiff_t GLsizeiptr;

	extern void glGenBuffers(GLsizei n, GLuint *buffers);
	extern void glBindBuffer(GLenum target, GLuint buffer);
	extern void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
	extern void glDeleteBuffers(GLsizei n, const GLuint *buffers);
	extern void glGenVertexArrays(GLsizei n, GLuint *arrays);
	extern void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params);
	extern void glActiveTexture(GLenum texture);

	extern void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 2.0
	//
	typedef char GLchar;

#define GL_COMPILE_STATUS                 0x8B81
#define GL_INFO_LOG_LENGTH                0x8B84
#define GL_LINK_STATUS                    0x8B82
#define GL_VALIDATE_STATUS				  0x8B83
#define GL_VERTEX_SHADER                  0x8B31
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_GEOMETRY_SHADER				  0x8DD9
#define GL_COMPUTE_SHADER				  0x91B9
#define GL_RENDERBUFFER                   0x8D41
#define GL_MAX_COLOR_ATTACHMENTS		  0x8CDF

	extern void glAttachShader(GLuint program, GLuint shader);
	extern void glCompileShader(GLuint shader);
	extern GLuint glCreateProgram();
	extern void glValidateProgram(GLuint program);
	extern GLuint glCreateShader(GLenum type);
	extern void glDeleteProgram(GLuint program);
	extern void glDeleteShader(GLuint shader);
	extern void glDetachShader(GLuint program, GLuint shader);
	extern void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
	extern void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, char *infoLog);
	extern void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
	extern void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, char *infoLog);
	extern GLint glGetUniformLocation(GLuint program, const char *name);
	extern void glLinkProgram(GLuint program);
	extern void glShaderSource(GLuint shader, GLsizei count, const char* *string, const GLint *length);
	extern void glUseProgram(GLuint program);
	extern void glUniform1i(GLint location, GLint v0);
	extern void glUniform1ui(GLint location, GLuint v0);
	extern void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
	extern void glUniform1f(GLint location, GLfloat v0);
	extern void glEnableVertexAttribArray(GLuint index);
	extern void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
	extern void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
	extern void glDisableVertexAttribArray(GLuint index);
	extern void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
	extern void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);

	extern void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
	extern void glUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

	extern void glUniform1fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glUniform2fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glUniform3fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glUniform4fv(GLint location, GLsizei count, const GLfloat *value);
	extern void glBindAttribLocation(GLuint program, GLuint index, const GLchar *name);
	extern GLint glGetAttribLocation(GLuint program, const GLchar *name);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OpenGL 4.0
	//
#define GL_FRAMEBUFFER                    0x8D40
#define GL_RGBA32F                        0x8814
#define GL_RGB32F						  0x8815
#define GL_RGBA16F						  0x881A
#define GL_RGB16F						  0x881B
#define GL_RG16F						  0x822F
#define GL_RG32F						  0x8230
#define GL_SRGB8_ALPHA8					  0x8C43
#define GL_DEPTH24_STENCIL8				  0x88F0
#define GL_DEPTH_STENCIL				  0x84F9
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1			  0x8CE1
#define GL_COLOR_ATTACHMENT2			  0x8CE2
#define GL_COLOR_ATTACHMENT3			  0x8CE3
#define GL_COLOR_ATTACHMENT4			  0x8CE4
#define GL_COLOR_ATTACHMENT5			  0x8CE5
#define GL_COLOR_ATTACHMENT6			  0x8CE6
#define GL_COLOR_ATTACHMENT7			  0x8CE7
#define GL_COLOR_ATTACHMENT8			  0x8CE8
#define GL_COLOR_ATTACHMENT9			  0x8CE9
#define GL_COLOR_ATTACHMENT10			  0x8CEA
#define GL_COLOR_ATTACHMENT11			  0x8CEB
#define GL_COLOR_ATTACHMENT12			  0x8CEC
#define GL_COLOR_ATTACHMENT13			  0x8CED
#define GL_COLOR_ATTACHMENT14			  0x8CEE
#define GL_COLOR_ATTACHMENT15			  0x8CEF
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_DEPTH_STENCIL_ATTACHMENT		  0x821A
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9

	extern void glBindFramebuffer(GLenum target, GLuint framebuffer);
	extern void glBindVertexArray(GLuint array);
	extern void glGenFramebuffers(GLsizei n, GLuint *ids);
	extern void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
	extern void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
	extern void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
	extern void glGenRenderbuffers(GLsizei n, GLuint * renderbuffers);
	extern void glRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
	extern void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
	extern GLenum glCheckFramebufferStatus(GLenum target);
	extern void glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	extern void glGenerateMipmap(GLenum target);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 3D Texture
	//
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_CLAMP_TO_BORDER 0x812D

	extern void glTexImage3D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * data);
	extern void glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint layer);
	extern void glDrawBuffers(GLsizei n, const GLenum *bufs);
	extern GLenum glCheckFramebufferStatus(GLenum target);
	extern void glFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);




#define GL_TEXTURE0_ARB 0x84C0
#define GL_TEXTURE1_ARB 0x84C1
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF
#define GL_READ_ONLY 0x88B8
#define GL_WRITE_ONLY 0x88B9
#define GL_READ_WRITE 0x88BA
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_UNIFORM_BUFFER_BINDING 0x8A28
#define GL_UNIFORM_BUFFER_START 0x8A29
#define GL_UNIFORM_BUFFER_SIZE 0x8A2A
#define GL_CLIP_DISTANCE0 GL_CLIP_PLANE0
#define GL_CLIP_DISTANCE1 GL_CLIP_PLANE1
#define GL_CLIP_DISTANCE2 GL_CLIP_PLANE2
#define GL_CLIP_DISTANCE3 GL_CLIP_PLANE3
#define GL_CLIP_DISTANCE4 GL_CLIP_PLANE4
#define GL_CLIP_DISTANCE5 GL_CLIP_PLANE5
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_UNSIGNED_INT_24_8 0x84FA
#define GL_HALF_FLOAT 0x140B
#define GL_STENCIL_ATTACHMENT 0x8D20
#define GL_STENCIL_INDEX8 0x8D48
#define GL_TEXTURE_SWIZZLE_RGBA 0x8E46
#define GL_PIXEL_PACK_BUFFER 0x88EB
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_DRAW 0x88E0
#define GL_FUNC_ADD 0x8006
#define GL_MIN 0x8007
#define GL_MAX 0x8008
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
#define GL_TEXTURE_DEPTH 0x8071
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_R11F_G11F_B10F 0x8C3A
#define GL_SRGB 0x8C40
#define GL_SRGB_ALPHA 0x8C42
#define GL_SAMPLES_PASSED 0x8914
#define GL_QUERY_RESULT 0x8866
#define GL_TEXTURE_RECTANGLE 0x84F5
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_FLOAT_RG32_NV 0x8887
#define GL_FLOAT_R32_NV 0x8885
#define GL_DEPTH_COMPONENT32F_NV 0x8DAB

	extern void glActiveTextureARB(GLenum texture);
	extern void glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);


	extern void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels);
	extern void glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);

	extern void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void * data);
	extern void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

	extern void glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	extern void glCopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);

	extern void glDeleteFramebuffers(GLsizei n, const GLuint * framebuffers);

	extern void glCopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);

	extern void *glMapBuffer(GLenum target, GLenum access);
	extern GLboolean glUnmapBuffer(GLenum target);

	extern void glVertexAttribDivisor(GLuint index, GLuint divisor);
	extern void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void * indices, GLsizei instancecount);
	extern void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, void *indices, GLint basevertex);
	extern void glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
	extern void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

	extern void glFramebufferTexture(GLenum target, GLenum attachment, GLuint texture, GLint level);
	extern void glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
	extern void glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
	extern void glGetTextureSubImage(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);

	extern void glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
	extern GLuint glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName);
	extern void glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
	extern void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);

	extern void glBlendEquation(GLenum mode);

	extern void glGetTextureParameterfv(GLuint texture, GLenum pname, GLfloat *params);
	extern void glGetTextureParameteriv(GLuint texture, GLenum pname, GLint *params);

	extern void glGenSamplers(GLsizei n, GLuint *samplers);
	extern void glSamplerParameterf(GLuint sampler, GLenum pname, GLfloat param);
	extern void glSamplerParameteri(GLuint sampler, GLenum pname, GLint param);
	extern void glBindSampler(GLuint unit, GLuint sampler);

	extern void glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
	extern void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);

	extern void glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
	extern void glMemoryBarrier(GLbitfield barriers);
	extern void glClearTexImage(GLuint texture, GLint level, GLenum format, GLenum type, const void * data);

	extern void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);

	extern void glBeginQuery(GLenum target, GLuint id);
	extern void glEndQuery(GLenum target);
	extern void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint * params);
	extern void glGenQueries(GLsizei n, GLuint * ids);
} // extern "C"
#endif