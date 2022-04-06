#include <stdio.h>
#include <vector>
#include <iostream>
#include "AL\al.h"
#include "SoundDevice.h"

static SoundDevice* _instance = nullptr;

SoundDevice* SoundDevice::get(){
	init();
	return _instance;
}

void SoundDevice::init(){
	if (_instance == nullptr)
		_instance = new SoundDevice();
}

void SoundDevice::shutDown() {
	delete _instance;
}

void SoundDevice::getLocation(float& x, float& y, float& z){
	alGetListener3f(AL_POSITION, &x, &y, &z);
}

void SoundDevice::getOrientation(float& ori){
	alGetListenerfv(AL_ORIENTATION, &ori);
}

float SoundDevice::getGain(){
	float curr_gain;
	alGetListenerf(AL_GAIN, &curr_gain);
	return curr_gain;
}

void SoundDevice::setLocation(const float& x, const float& y, const float& z){
	alListener3f(AL_POSITION, x, y, z);
}

void SoundDevice::setOrientation(const float& atx, const float& aty, const float& atz, const float& upx, const float& upy, const float& upz){
	std::vector<float> ori;
	ori.push_back(atx);
	ori.push_back(aty);
	ori.push_back(atz);
	ori.push_back(upx);
	ori.push_back(upy);
	ori.push_back(upz);
	alListenerfv(AL_ORIENTATION, ori.data());
}

void SoundDevice::setGain(const float& val){
	// clamp between 0 and 5
	float newVol = val;
	if (newVol < 0.f)
		newVol = 0.f;
	else if (newVol > 5.f)
		newVol = 5.f;

	alListenerf(AL_GAIN, newVol);
}

SoundDevice::SoundDevice(){
	m_alCDevice = alcOpenDevice(nullptr); // nullptr = get default device
	if (!m_alCDevice)
		throw("failed to get sound device");

	m_alCContext = alcCreateContext(m_alCDevice, nullptr);  // create context
	if (!m_alCContext)
		throw("Failed to set sound context");

	if (!alcMakeContextCurrent(m_alCContext))   // make context current
		throw("failed to make context current");

	const ALCchar* name = nullptr;
	if (alcIsExtensionPresent(m_alCDevice, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(m_alCDevice, ALC_ALL_DEVICES_SPECIFIER);
	if (!name || alcGetError(m_alCDevice) != AL_NO_ERROR)
		name = alcGetString(m_alCDevice, ALC_DEVICE_SPECIFIER);
}

SoundDevice::~SoundDevice(){
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_alCContext);
	alcCloseDevice(m_alCDevice);
}
