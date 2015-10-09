#include "Controller.h"
#include "SoundSystem.h"

Controller::Controller(Player* playerOne, Camera* camOne) :
mIsControllerConnected(false),
mPlayer(playerOne),
mCamOne(camOne)
{
	mLastEnumTime = ::GetTickCount64();
	mCharDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
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

	// USE RECOG. AND ACTION

	// LEFT THUMB STICK
	float LX = mControllerState.Gamepad.sThumbLX;
	float LY = mControllerState.Gamepad.sThumbLY;
	float magnitudeL = sqrt(LX*LX + LY*LY);	// determine how far the controller is pressed
	// determine direction pushed
	float normalizedLX = LX / magnitudeL;
	float normalizedLY = LY / magnitudeL;
	float normalizedMagnitudeL = 0;
	// check if controller is outside a circular dead zone
	if (magnitudeL > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {		// IF LEFT STICK IS MOVED
		
		// clip the magnitude at its expected max value
		if (magnitudeL > 32767) { magnitudeL = 32767; }
		// adjust magnitude relative to the end of the dead zone
		magnitudeL -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
		// normalize the magnitude with respect to its expected range 0.0 -> 1.0
		normalizedMagnitudeL = magnitudeL / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
		if (magnitudeL > 5000 && magnitudeL < 8000) {
			SoundSystem::Play(SOUND::PICKUP);
		}

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
	if (magnitudeR > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {		// IF RIGHT STICK IS MOVED
		
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

	XMVECTOR jumpUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	switch (mControllerState.Gamepad.wButtons) {
	case XINPUT_GAMEPAD_A :					// A-Button
		mCharDirection = mCharDirection + jumpUp;
		break;
	case XINPUT_GAMEPAD_B :					// B-Button
		SoundSystem::Play(SOUND::QUACK);
		break;
	case XINPUT_GAMEPAD_X :					// X-Button
		SoundSystem::Play(SOUND::PICKUP);
		break;
	case XINPUT_GAMEPAD_Y :					// Y-Button
		SoundSystem::Play(SOUND::MOBDEATH);
		break;
	case XINPUT_GAMEPAD_LEFT_THUMB :		// when left stick is pressed in
		Vibrate(65535, 0);
		break;
	case XINPUT_GAMEPAD_RIGHT_THUMB:		// when right stick is pressed in
		Vibrate(0, 65535);
		break;
	case XINPUT_GAMEPAD_START :				// Start-Button
		Vibrate(0, 0);
		break;
	case XINPUT_GAMEPAD_BACK :				// Back-Button (select)
		Vibrate(65535, 65535);
		break;
	case XINPUT_GAMEPAD_LEFT_SHOULDER :		// Left-Upper Trigger
		break;
	case XINPUT_GAMEPAD_RIGHT_SHOULDER :		// Right-Upper Trigger

		break;
	default:
		break;
	}
}

XMVECTOR Controller::GetCharDirection() {
	return mCharDirection;
}

void Controller::ResetSettings() {
	mCharDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
}

void Controller::Vibrate(int leftVal, int rightVal) {
	XINPUT_VIBRATION Vibration;
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));
	Vibration.wLeftMotorSpeed = leftVal;
	Vibration.wRightMotorSpeed = rightVal;
	XInputSetState(mControllerNum, &Vibration);
}
