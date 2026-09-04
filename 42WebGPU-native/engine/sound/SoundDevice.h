#pragma once
#include <AL/alc.h>

class SoundDevice{

public:
	static SoundDevice* get();
	static void Init();
	static void ShutDown();

private:

	SoundDevice();
	~SoundDevice();

	ALCdevice* m_alCDevice = nullptr;
	ALCcontext* m_alCContext = nullptr;

	static SoundDevice* Instance;
};

