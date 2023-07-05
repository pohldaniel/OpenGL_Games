#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/MeshObject/Shape.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "StateMachine.h"
#include "Physics.h"
#include "Ragdoll.h"
#include "MousePicker.h"
#include "ShapeDrawer.h"
#include "SolidIO.h"

#include "lugaru/Graphic/ModelLu.hpp"

#include "urho3d/Graphics/Model.h"
#include "urho3d/Core/Context.h"
#include "urho3d/Core/Object.h"
#include "urho3d/IO/FileSystem.h"
#include "urho3d/Container/Ptr.h"
#include "urho3d/Scene/Scene.h"
#include "urho3d/Scene/Node.h"
#include "urho3d/Resource/ResourceCache.h"
#include "urho3d/Resource/XMLFile.h"

/*#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/Graphics/AnimatedModel.h>
#include <Urho3D/Graphics/AnimationController.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Graphics/Light.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/Zone.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/MemoryBuffer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Scene/Scene.h>*/


class Game : public State, public MouseEventListener, public KeyboardEventListener, public Urho3D::Object{

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

private:

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void removePickingConstraint();
	void pickObject(const btVector3& pickPos, const class btCollisionObject* hitObj);

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	MousePicker m_mousePicker;

	bool m_initUi = true;
	bool m_drawUi = true;

	btTypedConstraint* m_pickConstraint;
	btRigidBody* pickedBody = 0;
	btScalar mousePickClamping = 30.f;

	ModelLu model;
	Utils::SolidIO  solidConverter;

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	Shape m_sword;
	Shape m_rabbit;

	

	Urho3D::SharedPtr<Urho3D::Scene> scene_;
	Urho3D::SharedPtr<Urho3D::Node> node;
	//Urho3D::Model m_model;

	Urho3D::StringHash GetType() const;
	const Urho3D::String& GetTypeName() const;
	const Urho3D::TypeInfo* GetTypeInfo() const;
};
