#pragma once

#include <GL/glew.h>
#include "Camera.h"

#define DIFFUSE_VS	"#version 410 core														\n \
																							\n \
					layout(location = 0) in vec3 i_position;								\n \
					layout(location = 1) in vec2 i_texCoord;								\n \
					layout(location = 2) in vec3 i_normal;									\n \
																							\n \
					uniform mat4 u_projection = mat4(1.0);									\n \
					uniform mat4 u_view = mat4(1.0);										\n \
					uniform mat4 u_model = mat4(1.0);										\n \
																							\n \
					void main() {															\n \
						gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);	\n \
					}"


#define DIFFUSE_FS	"#version 410 core													\n \
																						\n \
					layout (std140) uniform u_material {								\n \
						vec4 ambient;													\n \
						vec4 diffuse;													\n \
						vec4 specular;													\n \
						float shininess;												\n \
						float alpha;													\n \
					};																	\n \
																						\n \
					out vec4 color;														\n \
																						\n \
																						\n \
					void main() {														\n \
						color = vec4 (diffuse.xyz, 1.0);								\n \
					}"    

#define DIFFUSE_TEXTURE_VS	"#version 410 core													\n \
																								\n \
							layout(location = 0) in vec3 i_position;							\n \
							layout(location = 1) in vec2 i_texCoord;							\n \
							layout(location = 2) in vec3 i_normal;								\n \
																								\n \
							out vec2 v_texCoord;												\n \
																								\n \
							uniform mat4 u_projection = mat4(1.0);								\n \
							uniform mat4 u_view = mat4(1.0);									\n \
							uniform mat4 u_model = mat4(1.0);									\n \
																								\n \
							void main() {														\n \
							gl_Position =   u_projection * u_view * u_model * vec4(i_position, 1.0);	\n \
								v_texCoord = i_texCoord;										\n \
							}"


#define DIFFUSE_TEXTURE_FS	"#version 410 core													\n \
																								\n \
							layout (std140) uniform u_material {								\n \
								vec4 ambient;													\n \
								vec4 diffuse;													\n \
								vec4 specular;													\n \
								float shininess;												\n \
								float alpha;													\n \
							};																	\n \
																								\n \
							in vec2 v_texCoord;													\n \
							out vec4 color;														\n \
																								\n \
							uniform sampler2D u_texture;										\n \
																								\n \
							void main() {														\n \
								color = vec4(diffuse.xyz, 1.0) * texture(u_texture, v_texCoord);\n \
							}"    

#define DIFFUSE_INSTANCE_VS	"#version 410 core													\n \
																								\n \
					layout(location = 0) in vec3 i_position;									\n \
					layout(location = 1) in vec2 i_texCoord;									\n \
					layout(location = 2) in vec3 i_normal;										\n \
					layout(location = 3) in mat4 i_model;										\n \
																								\n \
					layout (std140) uniform u_view {											\n \
						mat4 view;																\n \
					};																			\n \
																								\n \
					uniform mat4 u_projection = mat4(1.0);										\n \
																								\n \
					void main() {																\n \
						gl_Position = u_projection * view * i_model  * vec4(i_position, 1.0);	\n \
					}"


#define DIFFUSE_INSTANCE_FS	"#version 410 core											\n \
																						\n \
					layout (std140) uniform u_material {								\n \
						vec4 ambient;													\n \
						vec4 diffuse;													\n \
						vec4 specular;													\n \
						float shininess;												\n \
						float alpha;													\n \
					};																	\n \
																						\n \
					out vec4 color;														\n \
																						\n \
																						\n \
					void main() {														\n \
						color = vec4 (diffuse.xyz, 1.0);								\n \
					}"    

#define DIFFUSE_TEXTURE_INSTANCE_VS	"#version 410 core												\n \
																									\n \
					layout(location = 0) in vec3 i_position;										\n \
					layout(location = 1) in vec2 i_texCoord;										\n \
					layout(location = 2) in vec3 i_normal;											\n \
					layout(location = 3) in mat4 i_model;											\n \
																									\n \
					out vec2 v_texCoord;															\n \
																									\n \
					layout (std140) uniform u_view {												\n \
						mat4 view;																	\n \
					};																				\n \
																									\n \
					uniform mat4 u_projection = mat4(1.0);											\n \
																									\n \
					void main() {																	\n \
						gl_Position =   u_projection *  view * i_model  * vec4(i_position, 1.0);	\n \
						v_texCoord = i_texCoord;													\n \
					}"


#define DIFFUSE_TEXTURE_INSTANCE_FS	"#version 410 core										\n \
																							\n \
					layout (std140) uniform u_material {									\n \
						vec4 ambient;														\n \
						vec4 diffuse;														\n \
						vec4 specular;														\n \
						float shininess;													\n \
						float alpha;														\n \
					};																		\n \
																							\n \
					in vec2 v_texCoord;														\n \
					out vec4 color;															\n \
																							\n \
					uniform sampler2D u_texture;											\n \
																							\n \
					void main() {															\n \
						color = vec4(diffuse.xyz, 1.0) * texture(u_texture, v_texCoord);	\n \
					}"

namespace BuiltInShader {
	

	extern unsigned int materialUbo;
	extern const unsigned int materialBinding;

	extern unsigned int viewUbo;
	extern const unsigned int viewBinding;

	extern unsigned int lightUbo;
	extern const unsigned int lightBinding;

	extern void UpdateViewUbo(const Camera& camera);
}