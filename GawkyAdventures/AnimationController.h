#pragma once
#include<map>
#include<unordered_set>
#include "d3dUtil.h"
#include "Skeleton.h"

//struct Bone;

enum ANIM_NAME {
	TEST,
	IDLE,
	WALK,
	JUMP,
	FALL,
	TRIP,
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
	XMFLOAT4X4 GetBoneTransform( Bone* bone );
private:
	float							timeSinceStart;
	Anim*							anims[_ANIM_COUNT];
	ANIM_NAME						currentAnim;
	std::map<Bone*, XMFLOAT4X4>		boneTransforms;
};

struct KeySet {
	std::vector<float>		keyTime;	// time in sec
	std::vector<XMFLOAT4>	value;		// position vector or rotQuat or scale vecto
};

struct Anim {
public:
	std::unordered_set<Bone*>						boneSet;
	std::string										name;
	float											totalTime;
	std::map<Bone*, KeySet>		rotChannels;
	std::map<Bone*, KeySet>		posChannels;
	std::map<Bone*, KeySet>		scaleChannels;
};