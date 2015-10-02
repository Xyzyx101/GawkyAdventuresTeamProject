#ifndef _SOUNDSYSTEM_H_
#define _SOUNDSYSTEM_H_

#include <fmod.hpp>
#include <fmod_errors.h>
#include <windows.h>
#include <mmsystem.h>

class SoundSystem 
{
public:
	SoundSystem();
	~SoundSystem();

	FMOD::System     *system;
	FMOD::Sound      *sound1, *sound2, *sound3, *music;
	FMOD::Channel    *channel1, *channel2, *channel3, *musicChannel;
	FMOD_RESULT      result;
	unsigned int     version;
	int              key;

	bool Init(HWND);
	void UpdateSound();
	void ShutdownSound();

	FMOD_CAPS caps;
};

#endif

