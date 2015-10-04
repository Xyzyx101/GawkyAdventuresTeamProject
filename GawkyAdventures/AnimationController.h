#pragma once
#include<map>
#include<unordered_set>
#include "d3dUtil.h"

struct Bone;

enum ANIM_NAME {
	ANIM_TEST,
	ANIM_IDLE,
	ANIM_WALK,
	ANIM_JUMP,
	ANIM_ATTACK,
	_ANIM_COUNT
};

struct Anim;

class AnimationController {
public:
	AnimationController();
	~AnimationController();
	void AddAnim( Anim* anim );
	void ChangeAnim( ANIM_NAME anim );
	void Interpolate( float dt );
private:
	float							timeSinceStart;
	Anim*							anims[_ANIM_COUNT];
	ANIM_NAME						currentAnim;
};

typedef std::map<float, XMFLOAT4> keySet_t; // map< time in sec, position vector or rotQuat or scale vector >

struct Anim {
public:
	std::unordered_set<Bone*>		boneSet;
	std::string						name;
	float							totalTime;
	std::map<Bone*, keySet_t>		rotChannels;
	std::map<Bone*, keySet_t>		posChannels;
	std::map<Bone*, keySet_t>		scaleChannels;
};