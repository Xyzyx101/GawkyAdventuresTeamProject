#include "SoundManager.h"

SoundManager::SoundManager()
{
	mDirectSound = 0;
	mPrimaryBuffer = 0;
	mSecondBuffer = 0;
	mBGMusic1Buffer = 0;
}

SoundManager::~SoundManager()
{
}

bool SoundManager::Init(HWND hwnd) {
	bool result;
	/*
		First init the DirectSound API as well as the primary buffer.
		Then LoadWaveFile can be called which will load in the .wav audio file and init the secondary buffer with the audio info.
		After loading is complete then PlayWaveFile is called which then plays the .wav file once.
	*/

	// Init direct sound and the primary buffer
	result = InitDirectSound(hwnd);
	if (!result) {
		return false;
	}
	
	/*
	// Load the background music
	result = LoadWaveFile("Sound\\ElectroSong.wav", &mBGMusic1Buffer);
	if (!result) {
		return false;
	}
	*/

	// Load a wave audio file onto a secondary buffer
	result = LoadWaveFile("Sound\\quack.wav", &mSecondBuffer);
	if (!result) {
		return false;
	}

	/*
	// play the wave file now that is has been loaded
	result = PlayWaveFile();
	if (!result) {
		return false;
	}
	*/

	return true;
}

// the shutdown function releases the secondary buffer which held the .wav file audio data using the ShutdownWaveFile function.
// once the other sounds are released we shutdown directsound buffer and interface.
void SoundManager::Shutdown() {
	// release the 2ndary buffer
	ShutdownWaveFile(&mSecondBuffer);
	// release background music
	ShutdownWaveFile(&mBGMusic1Buffer);
	// shutdown the directsound api
	ShutdownDirectSound();
	return;
}

// initDirectSound handles getting an interface pointer to DirectSound and the default primary sound buffer.
// Note, you can query the system for all sound devices and then grab the pointer to the primary sound buffer for a specific device.
// However, this setup just grabs the primary buffer of the default sound device on the system.
bool SoundManager::InitDirectSound(HWND hwnd) {
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;
	
	// init the direct sound interface pointer for the default sound device
	result = DirectSoundCreate8(NULL, &mDirectSound, NULL);
	if (FAILED(result)) { return false; }

	// set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = mDirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result)) { return false; }

	// primary buffer desc - dwFlags set what features we want to be able to use
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// get control of the primary sound buffer on the default sound device.
	result = mDirectSound->CreateSoundBuffer(&bufferDesc, &mPrimaryBuffer, NULL);
	if (FAILED(result)) { return false; }

	// now that control of the primary buffer is set we want to change its format to our desired audio file format.
	// for some decent quality sound we'll set it to uncompressed CD audio quality.
	// setup primary sound buffer format
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// set the primary buffer to be the wave format specified
	result = mPrimaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result)) { return false; }

	return true;
}

// shutdownDirectSound handles the release of the primary buffer and DirectSound interface.
void SoundManager::ShutdownDirectSound() {
	// release primary sound buffer pointer
	if (mPrimaryBuffer) {
		mPrimaryBuffer->Release();
		mPrimaryBuffer = 0;
	}
	// release the direct sound interface pointer.
	if (mDirectSound) {
		mDirectSound->Release();
		mDirectSound = 0;
	}
	return;
}

// LoadWaveFile handles the loading in a .wav audio file and then copies the data on to a new secondary buffer.
// For different formats this function should be changed or re-written.
bool SoundManager::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer) {
	HRESULT result;
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char *bufferPtr;
	unsigned long bufferSize;

	// open the wave file in binary
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0) { return false;	}

	// read in the wave file header
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1) { return false; }

	// check that the chunk ID is the RIFF format
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') || (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F')) { return false; }

	// check that the file format is the WAVE format
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') || (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E')) { return false; }

	// check that the sub chunk ID is the fmt format
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') || (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' ')) { return false; }

	// check that the audio format is WAVE_FORMAT_PCM
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM) { return false; }

	// check that the wave file was recorded in stereo format
	if (waveFileHeader.numChannels != 2) { return false; }

	// check that the wave file was recorded at a sample rate of 44.1 KHz
	if (waveFileHeader.sampleRate != 44100) { return false; }

	// ensure that the wave file was recorded in 16 bit format
	if (waveFileHeader.bitsPerSample != 16) { return false; }

	// check for the data chunk header
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') || (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a')) { return false; }

	// now that the wave header file is verified we can setup the secondary buffer which we'll load the audio data into.
	// setup a bufferDesc for secondary buffer
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// set the buffer desc of the secondary sound buffer that the wave file will be loaded onto
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// create temp sound buffer with the specific buffer settings
	result = mDirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result)) { return false; }

	// test the buffer format against the direct sound 8 interface and create the secondary buffer
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result)) { return false; }

	// release the temporary buffer
	tempBuffer->Release();
	tempBuffer = 0;

	// move to the beginning of the wave data which starts at the end of the data chunk header
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// create a temp buffer to hold the wave file data
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData) { return false; }

	// read in the wave file data into the newly create buffer
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize) { return false; }

	// close the file once done reading
	error = fclose(filePtr);
	if (error != 0) { return false; }

	// lock the secondary buffer to write wave data into it
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result)) { return false; }

	// copy the wave data into the buffer
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);
	// unlock the secondary buffer after the data has been written to it
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result)) { return false; }

	// release the wave data since it was copied into the secondary buffer
	delete[] waveData;
	waveData = 0;

	return true;
}

// ShutdownWaveFile releases the secondary buffer
void SoundManager::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer) {
	// release the secondary sound buffer
	if (*secondaryBuffer) {
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}
	return;
}

bool SoundManager::PlayWaveFile(string name) {
	HRESULT result;
	
	// if buffer != 0 try to play it
	if (mBGMusic1Buffer != 0 && name == "bgmusic") {
		// set position at the beginning of the sound buffer
		result = mBGMusic1Buffer->SetCurrentPosition(0);
		if (FAILED(result)) { return false; }
		// set volume of the buffer to 100%
		result = mBGMusic1Buffer->SetVolume(DSBVOLUME_MAX);
		if (FAILED(result)) { return false; }
		// play the content of this buffer
		result = mBGMusic1Buffer->Play(0, 0, 0);
		if (FAILED(result)) { return false; }
	}
	if (mSecondBuffer != 0 && name == "quack") {
		result = mSecondBuffer->SetCurrentPosition(0);
		if (FAILED(result)) { return false; }
		result = mSecondBuffer->SetVolume(DSBVOLUME_MAX);
		if (FAILED(result)) { return false; }
		result = mSecondBuffer->Play(0, 0, 0);
		if (FAILED(result)) { return false; }
	}
	
	return true;
}
