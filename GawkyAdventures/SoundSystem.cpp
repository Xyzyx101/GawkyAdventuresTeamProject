#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem()
{
}

bool SoundSystem::Init(HWND hWnd) {
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK) { 
		MessageBox(hWnd, L"FMOD_Ex System_Create Failed!", L"FMOD Error", MB_OK);
		return false; }
	result = system->getVersion(&version);
	if (result != FMOD_OK) { 
		MessageBox(hWnd, L"FMOD_Ex getVersion Failed!", L"FMOD Error", MB_OK);
		return false; }
	
	if (version < FMOD_VERSION) {
		MessageBox(hWnd, L"FMOD_Ex Version Issue!", L"FMOD Error", MB_OK);
		return false;
	}

	int numdrivers = 0;
	char name[256];
	FMOD_CAPS caps;
	FMOD_SPEAKERMODE speakermode; 

	result = system->getNumDrivers(&numdrivers);
	if (result != FMOD_OK) { 
		MessageBox(hWnd, L"FMOD_Ex getNUmDrivers Failed!", L"FMOD Error", MB_OK);
		return false; }
	if (numdrivers == 0) {
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex setOutput Failed!", L"FMOD Error", MB_OK);
			return false; }
	}
	else {
		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex getDriverCaps Failed!", L"FMOD Error", MB_OK);
			return false; }
		// set the selected speaker mode
		result = system->setSpeakerMode(speakermode);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex setSpeakerMode Failed!", L"FMOD Error", MB_OK);
			return false; }
		if (caps & FMOD_CAPS_HARDWARE_EMULATED) {
			/*
				User has 'Acceleration' slider set to off. Bad for latency!
				Might want to warn the user about this.
			*/
			MessageBox(hWnd, L"Hardware Acceleration is off! You should change that.", L"FMOD Notice", MB_OK);
			result = system->setDSPBufferSize(1024, 10);
			if (result != FMOD_OK) { 
				MessageBox(hWnd, L"FMOD_Ex setDSPBufferSize Failed!", L"FMOD Error", MB_OK);
				return false; }
		}

		result = system->getDriverInfo(0, name, 256, 0);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex getDriverInfo Failed!", L"FMOD Error", MB_OK);
			return false; }
		if (strstr(name, "SigmaTel")) {
			/*
				Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
				PCM floating point output seems to solve it.
			*/
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			if (result != FMOD_OK) { 
				MessageBox(hWnd, L"FMOD_Ex setSoftwareFormat Failed!", L"FMOD Error", MB_OK);
				return false; }
		}
		result = system->init(100, FMOD_INIT_NORMAL, 0);
		if (result == FMOD_ERR_OUTPUT_CREATEBUFFER) {
			// Speakermode not selected/supported by soundcard. Switch to stereo.
			result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			if (result != FMOD_OK) { 
				MessageBox(hWnd, L"FMOD_Ex setSpeakerMode(Stereo) Failed!", L"FMOD Error", MB_OK);
				return false; }
			// re-init
			result = system->init(100, FMOD_INIT_NORMAL, 0);
		}
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex system->Init Failed!", L"FMOD Error", MB_OK);
			return false; }
		// create sound1
		result = system->createSound("Sound\\quack.wav", FMOD_HARDWARE, 0, &sound1);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex createSound(sound1) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// setup sound1
		result = sound1->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex setMode(sound1) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// create sound2
		result = system->createSound("Sound\\quacksaid.wav", FMOD_HARDWARE, 0, &sound2);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex createSound(sound2) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// setup sound2
		result = sound2->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex setMode(sound2) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// create sound3
		result = system->createSound("Sound\\quack.wav", FMOD_HARDWARE, 0, &sound3);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex createSound(sound3) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// sound3 settings
		result = sound3->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex setMode(sound3) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// create a stream for our BGMusic
		result = system->createStream("Sound\\ElectroSong.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &music);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex createStream(BGMusic) Failed!", L"FMOD Error", MB_OK);
			return false; }
		// play the BGMusic
		result = system->playSound(FMOD_CHANNEL_FREE, music, false, &musicChannel);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex playSound Failed!", L"FMOD Error", MB_OK);
			return false; }

		// set volume
		result = musicChannel->setVolume(0.05f);
		if (result != FMOD_OK) { 
			MessageBox(hWnd, L"FMOD_Ex setVolume Failed!", L"FMOD Error", MB_OK);
			return false; }

		channel1 = 0;
		channel2 = 0;
		channel3 = 0;

	}

	return true;
}

void SoundSystem::Play(string soundName) {
	bool channelPlaying = false;

	if (soundName == "quack") {
		if (channel1) {
			result = channel1->isPlaying(&channelPlaying);
		}
		if (!channelPlaying) {
			result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel1);
			if (result != FMOD_OK) { return; }
		}
		channelPlaying = false;
	}

	system->update();
}
