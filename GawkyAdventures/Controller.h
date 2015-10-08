#pragma once
#include <wrl.h>
#include <XInput.h>
#include <stdio.h>
#include <math.h>
#include "Player.h"

class Controller
{
public:
	Controller(Player*);
	~Controller();
	void InitControllerInput(HWND);
	void CheckControllerState(HWND);					
	const UINT64 XINPUT_ENUM_TIMEOUT_MS = 2000;	// 2 second check timeout

private:
	Player*					mPlayer;				// attach to move this player
	bool					mIsControllerConnected;	// is a controller connected?
	XINPUT_STATE			mControllerState;		// state of controller
	XINPUT_CAPABILITIES		mControllerCap;			// holding controller capabilities
	UINT64					mLastEnumTime;			// last time controller check was made
	bool					mIsXDown;
};

