#pragma once

#include <memory>
#include "engine/Camera.h"
#include "engine/Shader.h"
#include "Physics.h"

#define MOUSEPICKER_VERTEX	"#version 410 core										                    \n \
																					                    \n \
							layout(location = 0) in vec3 i_position;				                    \n \
							layout(location = 1) in vec2 i_texCoord;				                    \n \
																					                    \n \
							uniform mat4 u_projection;                                                  \n \
							uniform mat4 u_view;                                                        \n \
							uniform mat4 u_model;					                                    \n \
							out vec2 v_texCoord;									                    \n \
																					                    \n \
							void main() {											                    \n \
								gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);	\n \
								v_texCoord = i_texCoord;							                    \n \
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
	int m_userIndex;
	void* m_userPoiner;
	btCollisionObject* m_collisionObject;

	MousePickCallback(const btVector3& origin, const btVector3& target, int collisionFilterGroup = btBroadphaseProxy::DefaultFilter, int collisionFilterMask = btBroadphaseProxy::AllFilter) : btCollisionWorld::ClosestRayResultCallback(origin, target) {
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
		m_origin = origin;
		m_target = target;
		m_userIndex = -1;
		m_userPoiner = nullptr;
		m_collisionObject = nullptr;
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}
};

class MousePickCallbackAll : public btCollisionWorld::AllHitsRayResultCallback {

public:

	btVector3 m_origin;
	btVector3 m_target;
	int index, m_userIndex1, m_userIndex2;
	void* m_userPointer1;
	void* m_userPointer2;
	btCollisionObject* m_collisionObject;

	MousePickCallbackAll(const btVector3& origin, const btVector3& target, int collisionFilterGroup = btBroadphaseProxy::DefaultFilter, int collisionFilterMask = btBroadphaseProxy::AllFilter) : btCollisionWorld::AllHitsRayResultCallback(origin, target) {
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
		m_origin = origin;
		m_target = target;
		index = -1;
		m_userIndex1 = -1;
		m_userIndex2 = -1;
		m_userPointer1 = nullptr;
		m_userPointer2 = nullptr;
		m_collisionObject = nullptr;
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
		return AllHitsRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

};


class MousePicker {

public:

	MousePicker();
	~MousePicker();

	void drawPicker(const Camera& camera);	
	void updatePosition(unsigned int posX, unsigned int posY, const Camera& camera);
	void updatePositionOrthographic(unsigned int posX, unsigned int posY, const Camera& camera);
	bool updatePositionOrthographicAll(unsigned int posX, unsigned int posY, const Camera& camera, btCollisionObject* collisonObject = nullptr);
	bool click(unsigned int posX, unsigned int posY, const Camera& camera, btCollisionObject* collisonObject1 = nullptr, btCollisionObject* collisonObject2 = nullptr);
	bool clickAll(unsigned int posX, unsigned int posY, const Camera& camera, btCollisionObject* collisonObject = nullptr);
	bool clickOrthographic(unsigned int posX, unsigned int posY, const Camera& camera, btCollisionObject* collisonObject1 = nullptr, btCollisionObject* collisonObject2 = nullptr);
	bool clickOrthographicAll(unsigned int posX, unsigned int posY, const Camera& camera, btCollisionObject* collisonObject = nullptr, int userIndex1 = -1);
	void setHasPicked(bool value);
	void setPosition(const Vector3f& pos);
	void setIsActivated(bool isactivated);
	const MousePickCallback& getCallback();
	const MousePickCallbackAll& getCallbackAll();
	float getPickingDistance();

	static std::unique_ptr<Shader>& GetShader();

private:

	void createBuffer();

	MousePickCallback m_callback;	
	MousePickCallbackAll m_callbackAll;

	bool m_debug = true;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;

	float m_pickingDistance;
	Matrix4f m_model;
	bool m_hasPicked;
	bool m_isActivated;

	static std::unique_ptr<Shader> s_shader;
};