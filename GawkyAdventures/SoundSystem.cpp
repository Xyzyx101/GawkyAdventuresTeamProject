#include "SoundSystem.h"

SoundSystem::SoundSystem()
{
}


SoundSystem::~SoundSystem()
{
}

bool SoundSystem::Init(HWND hWnd) {
	result = FMOD::System_Create(&system);
	if (result != FMOD_OK) { return false; }
	result = system->getVersion(&version);
	if (result != FMOD_OK) { return false; }

	if (version < FMOD_VERSION) {
		return false;
	}

	int numdrivers = 0;
	char name[256];
	FMOD_CAPS caps;
	FMOD_SPEAKERMODE speakermode; 

	result = system->getNumDrivers(&numdrivers);
	if (result != FMOD_OK) { return false; }
	if (numdrivers == 0) {
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		if (result != FMOD_OK) { return false; }
	}
	else {
		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		if (result != FMOD_OK) { return false; }
		// set the selected speaker mode
		result = system->setSpeakerMode(speakermode);
		if (result != FMOD_OK) { return false; }
		if (caps & FMOD_CAPS_HARDWARE_EMULATED) {
			/*
				User has 'Acceleration' slider set to off. Bad for latency!
				Might want to warn the user about this.
			*/
			result = system->setDSPBufferSize(1024, 10);
			if (result != FMOD_OK) { return false; }
		}

		result = system->getDriverInfo(0, name, 256, 0);
		if (result != FMOD_OK) { return false; }
		if (strstr(name, "SigmaTel")) {
			/*
				Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
				PCM floating point output seems to solve it.
			*/
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
			if (result != FMOD_OK) { return false; }
		}
		result = system->init(100, FMOD_INIT_NORMAL, 0);
		if (result == FMOD_ERR_OUTPUT_CREATEBUFFER) {
			/*
				Speakermode not selected/supported by soundcard. Switch to stereo.
			*/
			result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
			if (result != FMOD_OK) { return false; }
			// re-init
			result = system->init(100, FMOD_INIT_NORMAL, 0);
		}
		if (result != FMOD_OK) { return false; }

		result = system->createSound("Sound\\quack.wav", FMOD_HARDWARE, 0, &sound1);
		if (result != FMOD_OK) { return false; }

		result = sound1->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) { return false; }

		result = system->createSound("Sound\\quack.wav", FMOD_HARDWARE, 0, &sound2);
		if (result != FMOD_OK) { return false; }

		result = sound2->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) { return false; }

		result = system->createSound("Sound\\quack.wav", FMOD_HARDWARE, 0, &sound3);
		if (result != FMOD_OK) { return false; }

		result = sound3->setMode(FMOD_LOOP_OFF);
		if (result != FMOD_OK) { return false; }

		result = system->createStream("Sound\\ElectroSong.wav", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &music);
		if (result != FMOD_OK) { return false; }

		result = system->playSound(FMOD_CHANNEL_FREE, music, false, &musicChannel);
		if (result != FMOD_OK) { return false; }

		// set volume
		result = musicChannel->setVolume(0.05f);
		if (result != FMOD_OK) { return false; }

		channel1 = 0;
		channel2 = 0;
		channel3 = 0;

	}

	return true;
}

void SoundSystem::UpdateSound() {
	
	bool channelPlaying = false;
	if (channel1) {
	
		result = channel1->isPlaying(&channelPlaying);
	}
	if (GetAsyncKeyState( VK_SPACE ) && !channelPlaying) {
		result = system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel1);
		if (!result) { return; }
	}

	channelPlaying = false;
	/*
	if (channel2) {
		result = channel2->isPlaying(&channelPlaying);
	}
	if (GetAsyncKeyState('6') & 0x8000 && !channelPlaying) {
		result = system->playSound(FMOD_CHANNEL_FREE, sound2, false, &channel2);
		if (!result) { return; }
	}

	channelPlaying = false;
	if (channel3) {
		result = channel3->isPlaying(&channelPlaying);
	}
	if (GetAsyncKeyState('7') & 0x8000 && !channelPlaying) {
		result = system->playSound(FMOD_CHANNEL_FREE, sound3, false, &channel3);
		if (!result) { return; }
	}
	*/
	system->update();
}

void SoundSystem::ShutdownSound() {
	// cleanup if needed
}
