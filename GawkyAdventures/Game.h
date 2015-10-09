#ifndef GAME_H
#define GAME_H

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "Camera.h"
#include "Sky.h"
#include "xnacollision.h"
#include "d3dUtil.h"
#include "TextureMgr.h"
#include "BasicModel.h"
#include "RenderStates.h"
#include "Enemies.h"
#include "TheObjects.h"
#include "LevelBuilder.h"
#include "Player.h"
#include "ModelEnum.h"
#include "DefaultGameStateManager.h"

class GameState;

class Game : public D3DApp
{
public:
	Game(HINSTANCE hInstance);
	~Game();

	bool Init(HINSTANCE hInstance);
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	//DeltaTime getter
	float Game::getDeltaTime();

private:
	std::shared_ptr<DefaultGameStateManager> gameStateManager;
	Sky* mSky;

	//Player
	XMFLOAT3 mPlayerPosition;
	XMVECTOR PlayerForward;
	XMVECTOR PlayerRight;
	XMVECTOR PlayerUp;

	//lighting
	DirectionalLight mDirLights[3];
	UINT mLightCount;

	// Camera
	Camera* mCam;

	/// DeltaTime
	void addDeltaTime(float dt);
	XMFLOAT3 DeltaTime;
	float DeltaTimeF;

	// OBJ Model files
	TextureMgr mTexMgr;
	std::vector<BasicModelInstance> mModelInstances;
	std::vector <XNA::AxisAlignedBox> LevelCollisions;
	Enemies* theEnemies;
	TheObjects* Objects;
	LevelBuilder* Level1;
	Player* PlayerOne;

	int totEnemy;
	int totCollect;

	ID3DX11EffectTechnique* activeTexTech;
};
#endif // GAME_H
