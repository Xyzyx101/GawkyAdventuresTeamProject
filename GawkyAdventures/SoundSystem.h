#ifndef _SOUNDSYSTEM_H_
#define _SOUNDSYSTEM_H_

#include <fmod.hpp>
#include <windows.h>
#include <string>
using namespace std;

class SoundSystem 
{
public:
	SoundSystem();
	~SoundSystem();

	FMOD::System     *system;
	FMOD::Sound      *sound1, *sound2, *sound3, *music;
	FMOD::Channel    *channel1, *channel2, *channel3, *musicChannel;
	FMOD_RESULT      result;
	FMOD_CAPS		 caps;
	unsigned int     version;
	int              key;

	bool Init(HWND hWnd);
	void Play(string soundName);
};

#endif

