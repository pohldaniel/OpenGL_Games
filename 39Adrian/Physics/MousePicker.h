#pragma once

#include <memory>
#include "engine/Camera.h"
#include "engine/Shader.h"
#include "Physics.h"

#define MOUSEPICKER_VERTEX	"#version 410 core										\n \
																					\n \
							layout(location = 0) in vec3 i_position;				\n \
							layout(location = 1) in vec2 i_texCoord;				\n \
																					\n \
							uniform mat4 u_transform = mat4(1.0);					\n \
							out vec2 v_texCoord;									\n \
																					\n \
							void main() {											\n \
								gl_Position = u_transform * vec4(i_position, 1.0);	\n \
								v_texCoord = i_texCoord;							\n \
							}"


#define MOUSEPICKER_FRGAMENT	"#version 410 core																\n \
																												\n \
								uniform float u_radius = 0.7;													\n \
								in vec2 v_texCoord;																\n \
								out vec4 color;																	\n \
																												\n \
								float haloRing(vec2 uv, float radius, float thick){								\n \
									return clamp(-(abs(length(uv) - radius) * 100.0 / thick) + 0.9, 0.0, 1.0);	\n \
								}																				\n \
																												\n \
								void main() {																	\n \
									vec2 uv = v_texCoord * 2.0 - 1.0;											\n \
									vec3 ringColor = vec3(1.0, 1.0, 1.5);										\n \
									float intensity = haloRing (uv, u_radius, 16.0);							\n \
									color = vec4(ringColor, intensity);											\n \
								}"    

class MousePickCallback : public btCollisionWorld::ClosestRayResultCallback {

public:

	btVector3 m_origin;
	btVector3 m_target;

	MousePickCallback(const btVector3& origin, const btVector3& target, int collisionFilterGroup = btBroadphaseProxy::DefaultFilter, int collisionFilterMask = btBroadphaseProxy::AllFilter) : btCollisionWorld::ClosestRayResultCallback(origin, target) {
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
		m_origin = origin;
		m_target = target;
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

};


class MousePicker {

public:

	MousePicker();
	~MousePicker();

	void drawPicker(const Camera& camera);	
	void updatePosition(unsigned int posX, unsigned int posY, const Camera& camera);
	void updatePositionOrthographic(unsigned int posX, unsigned int posY, const Camera& camera);
	bool click(unsigned int posX, unsigned int posY, const Camera& camera);
	bool clickOrthographic(unsigned int posX, unsigned int posY, const Camera& camera);
	void setHasPicked(bool value);
	void setPosition(const Vector3f& pos);
	void setIsActivated(bool isactivated);
	const MousePickCallback& getCallback();
	float getPickingDistance();

private:

	void createBuffer();

	MousePickCallback m_callback;	

	bool m_debug = true;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	float m_pickingDistance;
	Matrix4f m_model;
	bool m_hasPicked;
	bool m_isActivated;

	static std::unique_ptr<Shader> s_shader;
};