#ifndef __spriteH__
#define __spriteH__

#include <memory>
#include "Object.h"
#include "Rect.h"
#include "Shader.h"

#define SPRITE_VERTEX      "#version 410 core										                    \n \
																					                    \n \
							layout(location = 0) in vec3 i_position;				                    \n \
                            layout(location = 1) in vec2 i_texCoord;				                    \n \
																					                    \n \
                            flat out int layer;                                                         \n \
                            out vec2 texCoord;                                                          \n \
                            out vec4 vertColor;                                                         \n \
																					                    \n \
							uniform bool u_flip = false;                                                \n \
							uniform mat4 u_transform = mat4(1.0);					                    \n \
                            uniform vec4 u_texRect = vec4(0.0, 0.0, 1.0, 1.0);                          \n \
							uniform vec4 u_color = vec4(1.0);                                           \n \
							uniform int u_layer = 0;                                                    \n \
                                                                                                        \n \
							void main() {											                    \n \
								gl_Position = u_transform * vec4(i_position, 1.0);	                    \n \
								layer = u_layer;									                    \n \
                                                                                                        \n \
                                texCoord.y =  u_flip ? 1.0 - i_texCoord.y : i_texCoord.y;               \n \
                                texCoord.x = i_texCoord.x * (u_texRect.z - u_texRect.x) + u_texRect.x;  \n \
                                texCoord.y = i_texCoord.y * (u_texRect.w - u_texRect.y) + u_texRect.y;	\n \
                                vertColor = u_color;                                                    \n \
							}"


#define SPRITE_FRGAMENT    "#version 410 core                                                        \n \
														                                             \n \
                            flat in int layer;		                                                 \n \
                            in vec2 texCoord;                                                        \n \
                            in vec4 vertColor;                                                       \n \
                                                                                                     \n \
                            out vec4 outColor;                                                       \n \
														                                             \n \
                            uniform sampler2DArray u_texture;                                        \n \
                                                                                                     \n \
                            void main() {                                                           \n \
                                 outColor = vertColor * texture(u_texture, vec3(texCoord, layer));   \n \
                            }"

class Sprite : public Object2D{

public:

	Sprite();

	void draw(const TextureRect& rect, const Vector4f& color = Vector4f::ONE);
	void draw(const Vector4f& color = Vector4f::ONE);

	void draw2(const TextureRect& rect, const Vector4f& color = Vector4f::ONE, const Matrix4f& worldTransformation = Matrix4f::IDENTITY);
	void draw2(const Vector4f& color = Vector4f::ONE, const Matrix4f& worldTransformation = Matrix4f::IDENTITY);

	void setShader(Shader* shader);
	void resetShader();
	Shader* getShader();

	static void Init(unsigned int width, unsigned int height);
	static void Resize(unsigned int width, unsigned int height);
	static void CleanUp();
	static void setTextureUnit(int unit);
	static Shader* GetShader();
	static void UnuseShader();

private:

	void static DrawQuad();
	Shader* m_shader;

	static Matrix4f Orthographic;
	static std::unique_ptr<Shader> SpriteShader;
	static unsigned int Vao;
	static unsigned int Vbo;
};
#endif