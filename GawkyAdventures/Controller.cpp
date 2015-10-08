#include "Controller.h"
#include "SoundSystem.h"

Controller::Controller(Player* playerOne) :
mIsControllerConnected(false),
mPlayer(playerOne)
{
	mLastEnumTime = ::GetTickCount64();
}


Controller::~Controller()
{
}

void Controller::InitControllerInput(HWND hWnd) {
	
	if (!mIsControllerConnected) {
		// set LastEnum time as now
		mLastEnumTime = ::GetTickCount64();

		// check for controller by trying to get the capabilities
		HRESULT cResult = XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &mControllerCap); 
		if (cResult != S_OK) { 
			MessageBox(hWnd, L"No controller found. USB XBOX Controller recommended!", L"Notice", MB_OK);
			return;		// nothing there
		}
		// otherwise, device is connected
		//MessageBox(hWnd, L"Controller found and ready.", L"Notice", MB_OK);
		mIsControllerConnected = true;
	}
}

void Controller::CheckControllerState(HWND hWnd) {
	if (!mIsControllerConnected) { return; }
	UINT64 currentTime = ::GetTickCount64();
	if (currentTime - mLastEnumTime < XINPUT_ENUM_TIMEOUT_MS) {
		return;		// havent hit delay(timeout) time yet, get out
	}
	// controller is connected, now check its state
	HRESULT sResult = XInputGetState(0, &mControllerState);
	if (sResult != S_OK) {
		// device is no longer connected
		mIsControllerConnected = false;
		MessageBox(hWnd, L"Controller disconnected...", L"Notice", MB_OK);
		mLastEnumTime = ::GetTickCount64();
		return;
	}

	// LEFT THUMB STICK
	float LX = mControllerState.Gamepad.sThumbLX;
	float LY = mControllerState.Gamepad.sThumbLY;
	float magnitudeL = sqrt(LX*LX + LY*LY);	// determine how far the controller is pressed
	// determine direction pushed
	float normalizedLX = LX / magnitudeL;
	float normalizedLY = LY / magnitudeL;
	float normalizedMagnitudeL = 0;
	// check if controller is outside a circular dead zone
	if (magnitudeL > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
		// clip the magnitude at its expected max value
		if (magnitudeL > 32767) { magnitudeL = 32767; }
		// adjust magnitude relative to the end of the dead zone
		magnitudeL -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		// normalize the magnitude with respect to its expected range 0.0 -> 1.0
		normalizedMagnitudeL = magnitudeL / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}
	else {
		magnitudeL = 0.0;
		normalizedMagnitudeL = 0.0;
	}

	// RIGHT THUMB STICK 
	float RX = mControllerState.Gamepad.sThumbRX;
	float RY = mControllerState.Gamepad.sThumbRY;
	float magnitudeR = sqrt(RX*RX + RY*RY);	// determine how far the controller is pressed
	// determine direction pushed
	float normalizedRX = RX / magnitudeR;
	float normalizedRY = RY / magnitudeR;
	float normalizedMagnitudeR = 0;
	// check if controller is outside a circular dead zone
	if (magnitudeR > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
		// clip the magnitude at its expected max value
		if (magnitudeR > 32767) { magnitudeR = 32767; }
		// adjust magnitude relative to the end of the dead zone
		magnitudeR -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		// normalize the magnitude with respect to its expected range 0.0 -> 1.0
		normalizedMagnitudeR = magnitudeR / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}
	else {
		magnitudeR = 0.0;
		normalizedMagnitudeR = 0.0;
	}
	
	// updates to run when controller does certain things
	switch (mControllerState.Gamepad.wButtons) {
	case XINPUT_GAMEPAD_A :		
		SoundSystem::Play(SOUND::SAYQUACK);
		break;
	case XINPUT_GAMEPAD_B :
		SoundSystem::Play(SOUND::QUACK);
		break;
	case XINPUT_GAMEPAD_X :
		SoundSystem::Play(SOUND::PICKUP);
		break;
	case XINPUT_GAMEPAD_Y :
		SoundSystem::Play(SOUND::MOBDEATH);
		break;
	default:
		break;
	}

}
