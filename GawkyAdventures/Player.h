#pragma once

#include <array>
#include "d3dUtil.h"
#include "xnacollision.h"
#include "BasicModel.h"
#include "Model.h"
#include "Skeleton.h"
#include "AnimationController.h"
#include "ModelEnum.h"
#include "FSM.h"

class Camera;
class Enemies;
class TheObjects;
class ModelLoader;

class Player
{
public:
	Player(ID3D11Device* device, TextureMgr& texMgr,
		const std::string& modelFilename,
		const std::wstring& texturePath, FLOAT x, FLOAT y, FLOAT z);
	~Player();
	bool init( ID3D11Device* device, ModelLoader* loader, TextureMgr& texMgr, const std::wstring& texturePath );
	void update( float dt, XMVECTOR direction, Enemies* guys, TheObjects* things );
	void updateCollisions( Enemies* guys, TheObjects* things );
	void setMoveDirection(XMVECTOR mDirection);
	void setLevelCollisions(std::vector <XNA::AxisAlignedBox> &thelevelCollisions);
	void drawPlayer(ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech);

	XMFLOAT3 getPlayerPosition();

private:
	// FSM functions
	FSM<Player>* fsm;
	void initFSM();
	void Idle_Before( float dt );
	void Idle_Update( float dt );
	void Walk_Before( float dt );
	void Walk_Update( float dt );
	void Jump_Before( float dt );
	void Jump_Update( float dt );
	void Fall_Before( float dt );
	void Fall_Update( float dt );
	void Trip_Before( float dt );
	void Trip_Update( float dt );
	void Die_Before( float dt );
	void Die_Update( float dt );
	
	Model mModel;
	Material mMaterial;
	Skeleton skeleton;
	AnimationController animController;
	ID3D11ShaderResourceView* mDiffuseSRV;

	void initPlayer();
	void setPlayerModel(ID3D11Device* device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath);

	XMFLOAT3 mPlayerPosition;
	XMFLOAT3 mPlayerScale;
	float mPlayerRotAngle;

	XMFLOAT3 mOPlayerPosition;
	XMFLOAT3 mOPlayerScale;
	float mOplayerRotAngle;
	
	int mPlayerVertexOffset;
	UINT mPlayerIndexOffset;
	UINT mPlayerIndexCount;

	//textures
	ID3D11ShaderResourceView* mPlayerMapSRV;
	Material mPlayerMat;
	XMFLOAT4X4 mPlayerTexTransform;

	// Movement
	XMVECTOR PlayerForward;
	XMVECTOR currCharDirection;
	XMVECTOR oldCharDirection;
	XMVECTOR charPosition;
	XMVECTOR moveDirection;

	// Bounding box of the Player
	XNA::AxisAlignedBox mPlayerBox;
	std::vector <XNA::AxisAlignedBox> LevelCollisions;
	std::array<bool, (int)Collisiontype::_COLLISION_TYPE_COUNT> collisions;
	
	BasicModel* playerModel;
	BasicModelInstance mPlayer;

	XMFLOAT3 desiredDirection;
	float verticalVelocity;
	XMFLOAT3 jumpDirection;
	XMFLOAT3 tripDirection;
	float tripTimer;
	float speed;
	int hitThing;
	int hitEnemy;
	bool killEnemy;
};
