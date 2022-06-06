#pragma once
#include "engine/input/Event.h"
#include "Camera.h"

class CameraMapController {
public:
	CameraMapController(Camera * cam);

	const Camera * GetCamera() const;

	void SetLimits(int l, int r, int t, int b);

	void SetSpeed(float val);

	void CenterCameraToPoint(int x, int y);

	//void HandleKeyDown(sgl::core::KeyboardEvent & event);
	//void HandleKeyUp(sgl::core::KeyboardEvent & event);
	void HandleMouseMotion(Event::MouseMoveEvent& event);

	void Update(float delta);

private:
	Camera * mCamera = nullptr;

	float mSpeed;

	int mDirX;
	int mDirY;
	int mLimitL;
	int mLimitR;
	int mLimitT;
	int mLimitB;

	bool mKeyScrollX = false;
	bool mKeyScrollY = false;
	bool mMouseScrollX = false;
	bool mMouseScrollY = false;
};

inline const Camera * CameraMapController::GetCamera() const{
	return mCamera;
}

inline void CameraMapController::SetLimits(int l, int r, int t, int b){
	mLimitL = l;
	mLimitR = r;
	mLimitT = t;
	mLimitB = b;
}

inline void CameraMapController::SetSpeed(float val) { mSpeed = val; }