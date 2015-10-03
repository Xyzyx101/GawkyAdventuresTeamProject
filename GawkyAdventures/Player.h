#pragma once

#include "d3dUtil.h"
#include "xnacollision.h"
#include "BasicModel.h"


class Camera;

class Enemies;
class TheObjects;


class Player
{
public:
	Player(ID3D11Device* device, TextureMgr& texMgr,
		const std::string& modelFilename,
		const std::wstring& texturePath, FLOAT x, FLOAT y, FLOAT z);
	~Player();

	void move(float dt, XMVECTOR direction, Enemies* guys,TheObjects* things);
	void update();

	void setMoveDirection(XMVECTOR mDirection);

	///setters
	void setLevelCollisions(std::vector <XNA::AxisAlignedBox> &thelevelCollisions);

	///getters	
	XMFLOAT3 getPlayerPosition(); 
	bool getOnGround();

	void drawPlayer(ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech);

private:

	void initPlayer();
	void setPlayerModel(ID3D11Device* device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath);

	XMFLOAT3 mPlayerPosition;
	XMFLOAT3 mPlayerScale;
	XMFLOAT4 mPlayerRotationQuad;
	XMFLOAT4 mPlayerRotation;

	XMFLOAT3 mOPlayerPosition;
	XMFLOAT3 mOPlayerScale;
	XMFLOAT4 mOPlayerRotationQuad;
	XMFLOAT4 mOPlayerRotation;

	float PlayerRotation;

	XMMATRIX mRotation;
	XMFLOAT4X4 playerRotation;

	int mPlayerVertexOffset;
	UINT mPlayerIndexOffset;
	UINT mPlayerIndexCount;

	//textures
	ID3D11ShaderResourceView* mPlayerMapSRV;
	Material mPlayerMat;

	XMFLOAT4X4 mPlayerTexTransform;

	XMVECTOR PlayerForward;
	XMVECTOR PlayerRight;
	XMVECTOR PlayerUp;

	XMVECTOR currCharDirection;
	XMVECTOR oldCharDirection;
	XMVECTOR charPosition;
	XMVECTOR moveDirection;

	XMVECTOR tripDirection;
	XMVECTOR tripDistance;

	// Bounding box of the Player
	XNA::AxisAlignedBox mPlayerBox;

	std::vector <XNA::AxisAlignedBox> LevelCollisions;
	
	///////////// the Players states
	bool isAlive;
	bool isImmune;
	bool isTripping;

	bool hitFeet;
	bool onGround;
	bool fellOffMap;
	
	//jumping variables
	bool isJump;
	bool isFalling;
	bool hitHead;

	int currentObject;
	FLOAT currGround;

	XMVECTOR startJumpPos;
	XMVECTOR Jump;

	BasicModel* playerModel;

	BasicModelInstance mPlayer;

};

