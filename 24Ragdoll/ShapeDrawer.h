#pragma once
#include <GL/glew.h>

#include <Memory>
#include "engine/Shader.h"
#include "engine/Camera.h"
#include "Physics.h"
#include "RandomColor.h"

#define SHAPEDRAWER_VERTEX	"#version 410 core															\n \
																										\n \
							layout(location = 0) in vec3 i_position;									\n \
																										\n \
							uniform mat4 u_projection;													\n \
							uniform mat4 u_view;														\n \
							uniform mat4 u_model = mat4(1.0);											\n \
																										\n \
							void main() {																\n \
								gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);	\n \
							}"


#define SHAPEDRAWER_FRGAMENT	"#version 410 core		\n \
														\n \
								uniform vec4 u_color;	\n \
								out vec4 color;			\n \
														\n \
								void main() {			\n \
									color = u_color;	\n \
								}"

class ShapeDrawer {

public:

	struct ShapeCacheConvex {
		ShapeCacheConvex(btConvexShape* s) : m_shapehull(s) {}
		btShapeHull m_shapehull;
		Vector4f m_color;
	};

	struct ShapeCache{
		
		Vector4f m_color;
	};

	
	~ShapeDrawer();

	void init(size_t size = 400);
	void shutdown();
	void drawDynmicsWorld(btDynamicsWorld* dynamicsWorld);
	
	void drawShape(btScalar* m,  btCollisionShape* shape);

	void setCamera(const Camera& camera);
	

	static ShapeDrawer& Get();

private:

	ShapeDrawer() = default;
	ShapeCacheConvex* cacheConvex(btCollisionShape* shape);
	ShapeCache* cache(btCollisionShape* shape);

	size_t m_maxTriangles = 0;
	
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;

	
	const Camera* m_camera;
	btAlignedObjectArray<ShapeCacheConvex*> m_shapecachesConvex;
	btAlignedObjectArray<ShapeCache*> m_shapecaches;

	RandomColor randomColor;

	static ShapeDrawer s_instance;
	static std::unique_ptr<Shader> s_shader;
};