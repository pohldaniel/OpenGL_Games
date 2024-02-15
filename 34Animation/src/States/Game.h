#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/scene/SceneNode.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <States/StateMachine.h>

#include "ModelBone.h"
#include "Bone.h"
#include "AnimationState.h"
#include "Animation.h"

static const float BONE_SIZE_THRESHOLD = 0.05f;

struct GeometryDesc{
	float lodDistance;
	unsigned vbRef;
	unsigned ibRef;
	unsigned drawStart;
	unsigned drawCount;
};



class Game : public State, public MouseEventListener, public KeyboardEventListener {

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
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	AssimpAnimatedModel assimpAnimated;
	void readMdl(std::string path);
	void CreateBones();
	void RemoveBones();
	void UpdateAnimation();

	std::vector<std::array<float, 3>> positions;
	std::vector<std::array<float, 3>> normals;
	std::vector <std::array<float, 2>> uvCoords;

	std::vector<std::array<short, 3>> faces;
	std::vector<std::vector<GeometryDesc>> geomDescs;
	std::vector<ModelBone> modelBones;
	BoundingBox boundingBox;
	unsigned short numBones = 0;

	Animation* animation;
	Bone* rootBone;
	Bone* *bones;
	Matrix4f* skinMatrices;
	std::vector<std::shared_ptr<AnimationState>> animationStates;
};