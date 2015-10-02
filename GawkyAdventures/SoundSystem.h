#ifndef _SOUNDSYSTEM_H_
#define _SOUNDSYSTEM_H_

#include <fmod.hpp>
#include <windows.h>
enum SOUND { QUACK, SAYQUACK, MOBDEATH };

class SoundSystem 
{
public:
	
	~SoundSystem();
	static void Play(SOUND soundName);
	static bool Init(HWND hWnd);
private:
	SoundSystem();
	static SoundSystem* instance;

	FMOD::System     *system;
	FMOD::Sound      *sound1, *sound2, *sound3, *music;
	FMOD::Channel    *channel1, *channel2, *channel3, *musicChannel;
	FMOD_RESULT      result;
	FMOD_CAPS		 caps;
	unsigned int     version;
	int              key;

	
};

#endif

