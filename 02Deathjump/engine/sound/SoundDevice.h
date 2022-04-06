#pragma once
#include <AL\alc.h>

#define SD_INIT SoundDevice::init();
#define LISTENER SoundDevice::get()

class SoundDevice{

public:
	static SoundDevice* get();
	static void init();
	static void shutDown();

	void getLocation(float &x, float& y, float& z);
	void getOrientation(float &ori);
	float getGain();

	void setLocation(const float& x, const float& y, const float& z);
	void setOrientation(
		const float& atx, const float& aty, const float& atz,
		const float& upx, const float& upy, const float& upz);
	void setGain(const float& val);

private:
	SoundDevice();
	~SoundDevice();

	ALCdevice* m_alCDevice;
	ALCcontext* m_alCContext;
};

