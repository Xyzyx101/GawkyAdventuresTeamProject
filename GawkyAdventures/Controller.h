#pragma once
#include <wrl.h>
#include <XInput.h>
#include <stdio.h>
#include <math.h>
#include "Player.h"
#include "Camera.h"
class Controller
{
public:
	Controller(Player*, Camera*);
	~Controller();
	void InitControllerInput(HWND);
	void CheckControllerState(HWND);
	XMVECTOR GetCharDirection();
	const UINT64 XINPUT_ENUM_TIMEOUT_MS = 2000;	// 2 second check timeout
	void ResetSettings();
	void Vibrate(int lVal = 0, int rVal = 0);
private:
	Player*					mPlayer;				// hold player
	Camera*					mCamOne;				// hold camera 
	XMVECTOR				mCharDirection;			//
	bool					mIsControllerConnected;	// is a controller connected?
	XINPUT_STATE			mControllerState;		// state of controller
	XINPUT_CAPABILITIES		mControllerCap;			// holding controller capabilities
	UINT64					mLastEnumTime;			// last time controller check was made
	int						mControllerNum = 0;
};

