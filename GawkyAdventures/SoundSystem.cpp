#include "SoundSystem.h"
// each sound effect must;
//			a. have its own channel and sound pointer
//			b. be loaded in and setup in init
//			c. have unique name in the header enum SOUND
// call from anywhere by including SoundSystem in classes header
// and use SoundSystem::Play(enumName);

SoundSystem* SoundSystem::instance = new SoundSystem();
SoundSystem::SoundSystem() {}
SoundSystem::~SoundSystem() {}

bool SoundSystem::Init(HWND hWnd) {
	FMOD_RESULT result;
	
	result = FMOD::System_Create(&instance->system);
	if (result != FMOD_OK) {
		MessageBox(hWnd, L"FMOD_Ex System_Create Failed!", L"FMOD Error", MB_OK);
		return false;
	}
	result = instance->system->getVersion(&instance->version);
	if (result != FMOD_OK) {
		MessageBox(hWnd, L"FMOD_Ex getVersion Failed!", L"FMOD Error", MB_OK);
		return false;
	}
	if (instance->version < FMOD_VERSION) {
		MessageBox(hWnd, L"FMOD_Ex Version Issue!", L"FMOD Error", MB_OK);
		return false;
	}

	int numdrivers = 0;
	char name[256];
	FMOD_CAPS caps;
	FMOD_SPEAKERMODE speakermode;

	result = instance->system->getNumDrivers(&numdrivers);
	if (result != FMOD_OK) {
		MessageBox(hWnd, L"FMOD_Ex getNumDrivers Failed!", L"FMOD Error", MB_OK);
		return false;
	}
	if (numdrivers == 0) {
		result = instance->system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setOutput Failed!", L"FMOD Error", MB_OK);
			return false;
		}
	}
	else {
		result = instance->system->getDriverCaps(0, &caps, 0, &speakermode);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex getDriverCaps Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// set the selected speaker mode
		result = instance->system->setSpeakerMode(speakermode);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setSpeakerMode Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		if (caps & FMOD_CAPS_HARDWARE_EMULATED) {
			// User has 'Acceleration' slider set to off. Bad for latency!
			MessageBox(hWnd, L"Hardware Acceleration is off! You should change that.", L"FMOD Notice", MB_OK);
			result = instance->system->setDSPBufferSize(1024, 10);
			if (result != FMOD_OK) {
				MessageBox(hWnd, L"FMOD_Ex setDSPBufferSize Failed!", L"FMOD Error", MB_OK);
				return false;
			}
		}
		// get the driver info
		result = instance->system->getDriverInfo(0, name, 256, 0);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex getDriverInfo Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		if (strstr(name, "SigmaTel")) {
			// Sigmatel devices can crackle if the format is PCM 16bit.
			// So, set PCM floating point output, seems to solve it.
			result = instance->system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			if (result != FMOD_OK) {
				MessageBox(hWnd, L"FMOD_Ex setSoftwareFormat Failed!", L"FMOD Error", MB_OK);
				return false;
			}
		}
		// init fmod normally
		result = instance->system->init(100, FMOD_INIT_NORMAL, 0);
		if (result == FMOD_ERR_OUTPUT_CREATEBUFFER) {
			// Speakermode not selected/supported by soundcard. Switch to stereo.
			result = instance->system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			if (result != FMOD_OK) {
				MessageBox(hWnd, L"FMOD_Ex setSpeakerMode(Stereo) Failed!", L"FMOD Error", MB_OK);
				return false;
			}
			// re-init
			result = instance->system->init(100, FMOD_INIT_NORMAL, 0);
		}
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex system->Init Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// LOAD & SETUP SOUNDS
		// create sound1
		result = instance->system->createSound("Sound\\quack.wav", FMOD_HARDWARE, 0, &instance->sound1);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex createSound(sound1) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// setup sound1
		result = instance->sound1->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setMode(sound1) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// create sound2
		result = instance->system->createSound("Sound\\quacksaid.wav", FMOD_HARDWARE, 0, &instance->sound2);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex createSound(sound2) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// setup sound2
		result = instance->sound2->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setMode(sound2) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// create sound3
		result = instance->system->createSound("Sound\\mobdeath.mp3", FMOD_HARDWARE, 0, &instance->sound3);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex createSound(sound3) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// sound3 settings
		result = instance->sound3->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setMode(sound3) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// create sound4
		result = instance->system->createSound("Sound\\blop.wav", FMOD_HARDWARE, 0, &instance->sound4);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex createSound(sound4) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// sound4 settings
		result = instance->sound4->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setMode(sound4) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// LOAD & PLAY MUSIC
		// create a stream for our BGMusic
		result = instance->system->createStream("Sound\\ElectroSong.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &instance->music);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex createStream(BGMusic) Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		// play the BGMusic
		result = instance->system->playSound(FMOD_CHANNEL_FREE, instance->music, false, &instance->musicChannel);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex playSound Failed!", L"FMOD Error", MB_OK);
			return false;
		}
		
		// set volume (entire musicChannel - all sounds)
		result = instance->musicChannel->setVolume(0.05f);
		if (result != FMOD_OK) {
			MessageBox(hWnd, L"FMOD_Ex setVolume Failed!", L"FMOD Error", MB_OK);
			return false;
		}

		instance->channel1 = 0;
		instance->channel2 = 0;
		instance->channel3 = 0;
		instance->channel4 = 0;
	}
	return true;
}

void SoundSystem::Play(SOUND soundName) {
	bool channelPlaying = false;
	FMOD_RESULT result;

	switch (soundName) {
	case QUACK:
		if (instance->channel1) {
			result = instance->channel1->isPlaying(&channelPlaying);
		}
		if (!channelPlaying) {
			result = instance->system->playSound(FMOD_CHANNEL_FREE, instance->sound1, false, &instance->channel1);
			if (result != FMOD_OK) { return; }
		}
		channelPlaying = false;
		break;

	case SAYQUACK:
		if (instance->channel2) {
			result = instance->channel2->isPlaying(&channelPlaying);
		}
		if (!channelPlaying) {
			result = instance->system->playSound(FMOD_CHANNEL_FREE, instance->sound2, false, &instance->channel2);
			if (result != FMOD_OK) { return; }
		}
		channelPlaying = false;
		break;

	case MOBDEATH:
		if (instance->channel3) {
			result = instance->channel3->isPlaying(&channelPlaying);
		}
		if (!channelPlaying) {
			result = instance->system->playSound(FMOD_CHANNEL_FREE, instance->sound3, false, &instance->channel3);
			if (result != FMOD_OK) { return; }
		}
		channelPlaying = false;
		break;
	case PICKUP:
		if (instance->channel4) {
			result = instance->channel4->isPlaying(&channelPlaying);
		}
		if (!channelPlaying) {
			result = instance->system->playSound(FMOD_CHANNEL_FREE, instance->sound4, false, &instance->channel4);
			if (result != FMOD_OK) { return; }
		}
		channelPlaying = false;
		break;
	default:
		break;
	}
	instance->system->update();
}
