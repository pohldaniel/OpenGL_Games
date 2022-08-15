#pragma once

#include <iostream>
#include <ctime>
#include <vector>
#include <array>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <cctype>
#include <unordered_map>

#include "Vector.h"
#include "Extension.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"
#include "AssetManger.h"
#include "..\Miniball\Miniball.h"

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
						int shininess;													\n \
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
								int shininess;													\n \
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
						int shininess;													\n \
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
						int shininess;														\n \
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

class ObjModel;
struct BoundingBox {

	void createBuffer(ObjModel& model);
	void drawRaw();

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;

	Vector3f position;
	Vector3f size;
};

struct BoundingSphere {

	void createBuffer(ObjModel& model);
	void drawRaw();

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;


	int m_uResolution = 49;
	int m_vResolution = 49;

	float m_radius;
	Vector3f m_position;
};

struct ConvexHull {
	void createBuffer(const char* filename, Vector3f &rotate, float degree, Vector3f& translate, float scale, bool useConvhull, ObjModel& model);
	void drawRaw();

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
};

struct IndexBufferCreator {

	std::vector<std::array<int, 10>> face;
	std::vector<float> positionCoordsIn;
	std::vector<float> normalCoordsIn;
	std::vector<float> textureCoordsIn;

	std::vector <float> vertexBufferOut;
	std::vector<unsigned int> indexBufferOut;

	void createIndexBuffer();

private:
	std::map<int, std::vector<int>> m_vertexCache;
	int addVertex(int hash, const float *pVertex, int stride);
};


class Mesh;
class ObjModel {
	friend Mesh;
	friend BoundingBox;
	friend BoundingSphere;
	friend ConvexHull;

public:

	ObjModel();
	~ObjModel();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void rotate(const Vector3f& axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void drawRaw();
	void drawRawInstanced();
	void draw(Camera& camera);
	void drawInstanced(Camera& camera);

	void createAABB();
	void createSphere();
	void createConvexHull(const char* a_filename, bool useConvhull = true);
	void createConvexHull(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale, bool useConvhull = true);

	void drawAABB();
	void drawSphere();
	void drawHull();

	bool loadObject(const char* filename);
	bool loadObject(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale);

	std::string getMltPath();
	std::string getModelDirectory();
	BoundingBox& getAABB();
	BoundingSphere& getBoundingSphere();
	ConvexHull& getConvexHull();
	Transform& getTransform();

	void generateTangents();
	void generateNormals();
	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	std::vector<Mesh*> getMeshes();

	void initAssets(bool instanced = false);
	void initAssets(AssetManager<Shader>& shaderManager, AssetManager<Texture>& textureManager, bool instanced = false);

	static void UpdateViewUbo(const Camera& camera);

private:
	//size values
	unsigned int m_size, m_numVertices, m_numIndices, m_stride, m_offset, m_numberOfBytes;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;

	std::vector<Mesh*> m_mesh;
	
	int m_numberOfMeshes;
	int m_numberOfTriangles;
	unsigned int m_numberOfVertices;
	std::string m_mltPath;
	std::string m_modelDirectory;
	bool m_hasMaterial;
	Vector3f m_center;
	
	BoundingBox aabb;
	BoundingSphere boundingSphere;
	ConvexHull convexHull;

	std::unordered_map<int, Shader*> m_shader;
	std::unordered_map<int, Texture> m_textures;

	AssetManager<Shader> m_shaderManager;
	AssetManager<Texture> m_textureManager;

	Transform m_transform;

	static unsigned int s_materialUbo;
	static const unsigned int s_materialBinding = 3;

	static unsigned int s_viewUbo;
	static const unsigned int s_viewBinding = 4;
};

class Mesh {

	friend ObjModel;

public:
	///////////////////////////////geometry content
	struct Material {		
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float shininess;
		std::string diffuseTexPath;
		std::string bumpMapPath;
		std::string displacementMapPath;
	};

	Mesh(std::string mltName, int numberTriangles, ObjModel* model);
	Mesh(int numberTriangles, ObjModel* model);
	~Mesh();

	void drawRaw();
	void drawRawInstanced();
	void setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies);
	Material& getMaterial();
	
	std::vector<float>& getVertexBuffer();
	std::vector<unsigned int>& getIndexBuffer();
	int getStride();

private:
	bool readMaterial();

	ObjModel* m_model;
	std::string m_mltName;
	Material m_material;	
	///////////////////////////////////////OpenGL content////////////////// to do seperate it 

	void createBuffer();
	void generateTangents();
	void generateNormals();
	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);
	short m_numBuffers;
	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_vboInstances;
	unsigned int m_ibo = 0;
	

	unsigned int m_drawCount;
	unsigned int m_instanceCount = 0;
	
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride;

	void updateMaterialUbo(unsigned int& ubo);
};