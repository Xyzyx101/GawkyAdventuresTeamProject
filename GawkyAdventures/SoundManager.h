#ifndef _SOUNDMANAGER_H_
#define _SOUNDMANAGER_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>

using namespace std;

class SoundManager
{
public:
	SoundManager();
	SoundManager(const SoundManager&);
	~SoundManager();

	bool Init(HWND);
	void Shutdown();
	bool PlayWaveFile(string name);

private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

	bool InitDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadWaveFile(char*, IDirectSoundBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**);

	IDirectSound8* mDirectSound;
	IDirectSoundBuffer* mPrimaryBuffer;
	IDirectSoundBuffer8* mSecondBuffer;
	IDirectSoundBuffer8* mBGMusic1Buffer;
};

#endif