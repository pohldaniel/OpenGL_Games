#include <stdio.h>
#include <vector>
#include <iostream>
#include "AL/al.h"
#include "SoundDevice.h"

SoundDevice* SoundDevice::Instance = nullptr;

SoundDevice* SoundDevice::get(){
	Init();
	return Instance;
}

void SoundDevice::Init(){
	if (Instance == nullptr)
		Instance = new SoundDevice();
}

void SoundDevice::ShutDown() {
	delete Instance;
}

SoundDevice::SoundDevice(){
	m_alCDevice = alcOpenDevice(nullptr);
	if (!m_alCDevice)
		throw("failed to get sound device");

	m_alCContext = alcCreateContext(m_alCDevice, nullptr);
	if (!m_alCContext)
		throw("Failed to set sound context");

	if (!alcMakeContextCurrent(m_alCContext))
		throw("failed to make context current");
}

SoundDevice::~SoundDevice(){
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_alCContext);
	alcCloseDevice(m_alCDevice);
}
