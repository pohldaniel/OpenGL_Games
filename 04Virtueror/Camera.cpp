#include "Camera.h"

#include <cmath>
#include <iostream>


Camera * Camera::mDefault = nullptr;
Camera * Camera::mDummy = nullptr;

Camera::Camera() : mFuncOnMove([] {}){
}

Camera::Camera(int w, int h): mFuncOnMove([] {}), mWidth(w), mHeight(h){
}

void Camera::MoveX(float delta){
	mXf += delta;

	mXd = std::roundf(mXf);

	//mFuncOnMove();
}

void Camera::MoveY(float delta){
	mYf += delta;

	mYd = std::roundf(mYf);

	//mFuncOnMove();
}

void Camera::ResetPosition(){
	mXf = 0.f;
	mYf = 0.f;

	mXd = 0;
	mYd = 0;
	
	//mFuncOnMove();
}

void Camera::CenterToPoint(int x, int y){
	mXf = x - (mWidth * 0.5f);
	mYf = (mHeight * 0.5f) - y;

	mXd = std::roundf(mXf);
	mYd = std::roundf(mYf);

	//mFuncOnMove();
}

void Camera::SetPosition(int x, int y){
	// same position -> do nothing
	if (mXd == x && mYd == y)
		return;

	mXd = x;
	mYd = y;

	mXf = static_cast<float>(x);
	mYf = static_cast<float>(y);

	//mFuncOnMove();
}

void Camera::SetX(int x){
	// same position -> do nothing
	if (mXd == x)
		return;

	mXd = x;

	mXf = static_cast<float>(x);

	//mFuncOnMove();
}

void Camera::SetY(int y){
	// same position -> do nothing
	if (mYd == y)
		return;

	mYd = y;

	mYf = static_cast<float>(y);

	//mFuncOnMove();
}

void Camera::CreateDefaultCamera(){
	if (nullptr == mDefault)
		mDefault = new Camera;
}

void Camera::DestroyDefaultCamera(){
	delete mDefault;
	mDefault = nullptr;
}

void Camera::CreateDummyCamera(){
	if (nullptr == mDummy)
		mDummy = new Camera;
}

void Camera::DestroyDummyCamera(){
	delete mDummy;
	mDummy = nullptr;
}