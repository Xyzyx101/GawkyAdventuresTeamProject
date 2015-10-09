#include <memory>
#include <iostream>
#include "PlayState.h"
#include "d3dUtil.h"
#include "LevelBuilder.h"
#include "Game.h"
#include "Sky.h"
#include "Effects.h"
#include "BasicModel.h"
#include "Player.h"
#include "LoadM3d.h"
#include "LoadOBJ.h"
#include "Camera.h"
#include "ModelEnum.h"
#include "Controller.h"
#include "ModelLoader.h"

using namespace std;

PlayState::PlayState(const std::shared_ptr<GameStateManager> &gameStateManager, 
					ID3D11Device* device, 
					ID3D11DeviceContext* deviceContext,
					ID3D11RenderTargetView* mRenderTargetView, 
					ID3D11DepthStencilView* mDepthStencilView, 
					IDXGISwapChain* mSwapChain,
					ID3DX11EffectTechnique* activeTexTech,
					Camera* cam) :
					gameStateManager(gameStateManager),
					device(device),
					deviceContext(deviceContext),
					mRenderTargetView(mRenderTargetView),
					mDepthStencilView(mDepthStencilView),
					mSwapChain(mSwapChain),
					cam(cam),
					sky(0),
					lightCount(3),
					playerPosition(0.0f, 2.0f, 0.0f),
					totalEnemies(0),
					totalCollectibles(0)
{};

void PlayState::Entered()
{
	Effects::InitAll(device);
	InputLayouts::InitAll(device);
	texMgr.Init(device);
	
	playerForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	playerRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	playerUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	cam->getPlayerPos(playerPosition);
	cam->playerInfo(playerForward, playerRight, playerUp);
	cam->SetPosition(0.0f, 2.0f, -40.0f);

	dirLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLights[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLights[0].Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLights[0].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	dirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLights[1].Specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	dirLights[1].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	dirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLights[2].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLights[2].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	dirLights[2].Direction = XMFLOAT3(-0.5f, -1.9f, -1.57735f);
	
	sky = new Sky(device, L"Textures//thisskybox.dds", 10000.0f);
	playerOne = new Player( device, texMgr, "Models\\gawky.obj", L"Textures\\", 0.0f, 10.0f, 0.0f );
	ModelLoader* modelLoader = new ModelLoader();
	if( !playerOne->init( device, modelLoader, texMgr, L"Textures\\" ) ) {
		wchar_t msgbuf[256];
		wsprintf( msgbuf, L"\nPlayer Init Failed!!!\n\n" );
		OutputDebugString( msgbuf );
	}
	enemies = new Enemies(device, texMgr);
	objects = new TheObjects(device, texMgr);
	level = new LevelBuilder(device, texMgr);

	objects->createObject(branch, 60.0f, 0.25f, 55.0f, ctStumble, 1);
	objects->createObject(branch, 0.0f, 0.25f, 20.0f, ctStumble, 1);

	objects->createObject(orange, 80.0f, 30.0f, 20.0f, ctCollect, 1);
	objects->createObject(orange, 20.0f, 4.0f, 20.0f, ctCollect, 1);
	objects->createObject(orange, -80.0f, 10.0f, -60.0f, ctCollect, 1);

	//Objects->createObject(gatetwo, -95.0f, 8.5f, 0.0f, ctStumble, 7);

	enemies->createEnemy(simpleEnemy, -85.0f, 9.0f, 78.0f, -65.f, 9.0f, 78.0f, NULL, 0, 0, 0, 0, 0, 3, 15, ctEnemy);
	enemies->createEnemy(simpleEnemy, 55.0f, 3.0f, 80.0f, 55.0f, 3.0f, 60.0f, NULL, 0, 0, 0, 0, 0, 3, 15, ctEnemy);
	enemies->createEnemy(simpleEnemy, 0.0f, 3.0f, 45.0f, 0.0f, 3.0f, 25.0f, NULL, 0, 0, 0, 0, 0, 3, 15, ctEnemy);

	level->createLevelParts(Ground, 0, -1.8, 0, 0, 7, 0);

	///left side 3 platforms
	level->createLevelParts(Platform, -76, 2.1, 26.6, 0, 7, 0);
	level->createLevelParts(Platform, -76, 9.1, 50.82, 0, 7, 0);
	level->createLevelParts(Platform, -76, 2.1, 74.2, 0, 7, 0);

	///rightside 3 platforms
	level->createLevelParts(Platform, 73, 2.1, 75.6, 0, 7, 0);
	level->createLevelParts(Platform, 73, 9.1, 50.82, 0, 7, 0);
	level->createLevelParts(Platform, 73, 14.0, 15.82, 0, 7, 0);

	///the tree's
	level->createLevelParts(SmallTree, -56, 15.4, 86.8, 0, 7, 0);
	level->createLevelParts(SmallTree, -56, 15.4, 72.8, 0, 7, 0);
	level->createLevelParts(SmallTree, -56, 15.4, 58.8, 0, 7, 0);

	/// large tree
	level->createLevelParts(TreeTrunk, 0, 14, 57.6, 0, 7, 0);
	level->createLevelParts(TreeTop, 0, 61.6, 57.6, 0, 7, 0);

	// the Fence
	level->createLevelParts(Fence1, 87, 5, 0, 0, 7, 0);
	level->createLevelParts(FencePart2, -95, 6, 48, 0, 7, 0);
	level->createLevelParts(FencePart2, -95, 6, -48, 0, 7, 0);


	level->createLevelParts(Fence2, -5, 6, 91, 0, 7, 0);
	level->createLevelParts(Fence2, -5, 6, -91, 0, 7, 0);
	//cattails
	level->createLevelParts(Cattail, 84, 5.6, -47.6, 0, 7, 0);
	level->createLevelParts(Cattail, 77, 5.6, -47.6, 0, 7, 0);
	level->createLevelParts(Cattail, 68.25, 5.6, -47.6, 0, 7, 0);
	level->createLevelParts(Cattail, 59.5, 5.6, -47.6, 0, 7, 0);
	level->createLevelParts(Cattail, 50.75, 5.6, -47.6, 0, 7, 0);
	/// the House
	level->createLevelParts(HouseSide, 43.4, 14, -70.0, 0, 7, 0);
	level->createLevelParts(HouseSide, 7, 14, -70.0, 0, 7, 0);
	level->createLevelParts(HouseBack, 24.5, 14, -82.6, 0, 7, 0);
	level->createLevelParts(HouseRoof, 24.5, 34.0, -70.5, 0, 6, 0);

	/// build the sandbox


	level->createLevelParts(SandBox, -60.9, 1.4, -68.0, 0, 7, 0);


	////2nd section of level
	////offset everything by -250 and -15

	//the barn
	int x2o = -230;
	int y2o = 0;
	int z2o = 0;

	level->createLevelParts(lvl2Ground, 0 + x2o, -5 + y2o, 0.0 + z2o, ctLevel, 14, 0);
	level->createLevelParts(barnback, -81.56 + x2o, 22.8 + y2o, 136.8 + z2o, ctLevel, 23, 0);
	level->createLevelParts(barnside, -124.34 + x2o, 23.58 + y2o, 100.92 + z2o, ctLevel, 25, 0);
	level->createLevelParts(barnside, -36.34 + x2o, 23.58 + y2o, 100.92 + z2o, ctLevel, 25, 0);
	level->createLevelParts(barnfrontside, -44.74 + x2o, 23.74 + y2o, 64.96 + z2o, ctLevel, 25, 0);
	level->createLevelParts(barnfrontside2, -117.21 + x2o, 22.8 + y2o, 64.96 + z2o, ctLevel, 25, 0);
	level->createLevelParts(barnfronttop, -80.47 + x2o, 64.38 + y2o, 64.96 + z2o, ctLevel, 25, 0);
	level->createLevelParts(barnroof, -81.56 + x2o, 75.84 + y2o, 102.79 + z2o, ctLevel, 25, 0);
	/// 1.57 = 90 degrees
	level->createLevelParts(Fence1, -139 + x2o, 7 + y2o, 0 + z2o, ctLevel, 11, 0);

	level->createLevelParts(Fence1, 0 + x2o, 7 + y2o, 139 + z2o, ctLevel, 11, 1.57);
	level->createLevelParts(Fence1, 0 + x2o, 7 + y2o, -139 + z2o, ctLevel, 11, 1.57);

	//bails


	//bottom Row

	level->createLevelParts(squarebail, -114 + x2o, 0 + y2o, 129 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -114 + x2o, 11.6 + y2o, 129 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -114 + x2o, 0 + y2o, 120.2 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -114 + x2o, 0 + y2o, 111.48 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -114 + x2o, 0 + y2o, 102.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -100 + x2o, 0 + y2o, 128.98 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -86.68 + x2o, 0 + y2o, 128.98 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -86.68 + x2o, 5.5 + y2o, 128.98 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -86.68 + x2o, 11.6 + y2o, 128.98 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -103 + x2o, 0 + y2o, 118 + z2o, ctLevel, 7, 1.57);
	level->createLevelParts(squarebail, -93.9 + x2o, 0 + y2o, 118 + z2o, ctLevel, 7, 1.57);
	level->createLevelParts(squarebail, -84.98 + x2o, 0 + y2o, 118 + z2o, ctLevel, 7, 1.57);
	level->createLevelParts(squarebail, -103 + x2o, 0 + y2o, 104.9 + z2o, ctLevel, 7, 1.57);
	level->createLevelParts(squarebail, -93.9 + x2o, 0 + y2o, 104.9 + z2o, ctLevel, 7, 1.57);
	level->createLevelParts(squarebail, -85 + x2o, 0 + y2o, 104.9 + z2o, ctLevel, 7, 1.57);
	level->createLevelParts(squarebail, -114 + x2o, 5.5 + y2o, 120.2 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -114 + x2o, 11.6 + y2o, 120.2 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -114 + x2o, 5.5 + y2o, 111.48 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -100.6 + x2o, 5.5 + y2o, 128.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -100.6 + x2o, 11.6 + y2o, 128.9 + z2o, ctLevel, 7, 0);

	//
	level->createLevelParts(squarebail, -72.7 + x2o, 0 + y2o, 128.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 5.5 + y2o, 128.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 11.6 + y2o, 128.9 + z2o, ctLevel, 7, 0);


	level->createLevelParts(squarebail, -72.7 + x2o, 0 + y2o, 119.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 5.5 + y2o, 119.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 11.6 + y2o, 119.9 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -72.7 + x2o, 0 + y2o, 111.2 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 5.5 + y2o, 111.2 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 11.6 + y2o, 111.2 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -72.7 + x2o, 0 + y2o, 102.4 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 5.5 + y2o, 102.4 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 11.6 + y2o, 102.4 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -72.7 + x2o, 0 + y2o, 94 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 5.5 + y2o, 94 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -72.7 + x2o, 11.6 + y2o, 94 + z2o, ctLevel, 7, 0);


	//

	level->createLevelParts(squarebail, -59 + x2o, 11.6 + y2o, 128.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -59 + x2o, 11.6 + y2o, 119.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -59 + x2o, 11.6 + y2o, 111.2 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -59 + x2o, 11.6 + y2o, 102.4 + z2o, ctLevel, 7, 0);

	level->createLevelParts(squarebail, -59 + x2o, 0 + y2o, 94 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -59 + x2o, 5.5 + y2o, 94 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -59 + x2o, 11.6 + y2o, 94 + z2o, ctLevel, 7, 0);


	//
	level->createLevelParts(squarebail, -45.3 + x2o, 11.6 + y2o, 128.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -45.3 + x2o, 11.6 + y2o, 119.9 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -45.3 + x2o, 11.6 + y2o, 111.2 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, -45.3 + x2o, 11.6 + y2o, 102.4 + z2o, ctLevel, 7, 0);


	//
	level->createLevelParts(squarebail, 40 + x2o, 0 + y2o, -43 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 40 + x2o, 5.5 + y2o, -43 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 40 + x2o, 11.6 + y2o, -43 + z2o, ctLevel, 7, 0);
	//
	level->createLevelParts(squarebail, 54 + x2o, 0 + y2o, -43 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 54 + x2o, 5.5 + y2o, -43 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 54 + x2o, 11.6 + y2o, -43 + z2o, ctLevel, 7, 0);
	//
	level->createLevelParts(squarebail, 68.5 + x2o, 0 + y2o, -43 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 68.5 + x2o, 5.5 + y2o, -43 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 68.5 + x2o, 11.6 + y2o, -43 + z2o, ctLevel, 7, 0);
	//
	level->createLevelParts(squarebail, 54 + x2o, 0 + y2o, -52 + z2o, ctLevel, 7, 0);
	level->createLevelParts(squarebail, 54 + x2o, 5.5 + y2o, -52 + z2o, ctLevel, 7, 0);
	//
	level->createLevelParts(squarebail, 40 + x2o, 0 + y2o, -52 + z2o, ctLevel, 7, 0);


	level->createLevelParts(squarebail, -45.3 + x2o, 0 + y2o, 94 + z2o, ctNothing, 7, 0);
	level->createLevelParts(squarebail, -45.3 + x2o, 5.5 + y2o, 94 + z2o, ctNothing, 7, 0);
	level->createLevelParts(squarebail, -45.3 + x2o, 11.6 + y2o, 94 + z2o, ctLevel, 7, 0);

	level->createLevelParts(roundbail, 41 + x2o, 9 + y2o, 52 + z2o, ctLevel, 14, 0);
	level->createLevelParts(roundbail, 67 + x2o, 9 + y2o, 52 + z2o, ctLevel, 14, 0);

	level->createLevelParts(woodpile, -130 + x2o, 4 + y2o, -127 + z2o, ctLevel, 1, 1.57);




	objects->createObject(branch, 20 + x2o, -3 + y2o, -70 + z2o, ctStumble, 1);

	objects->createObject(orange, 54 + x2o, 4 + y2o, 32 + z2o, ctCollect, 1);
	objects->createObject(orange, 54 + x2o, 4 + y2o, -7 + z2o, ctCollect, 1);

	objects->createObject(orange, -59 + x2o, 4 + y2o, 111 + z2o, ctCollect, 1);
	objects->createObject(orange, -45 + x2o, 4 + y2o, 111 + z2o, ctCollect, 1);
	//
	objects->createObject(orange, 54 + x2o, 35 + y2o, 53 + z2o, ctCollect, 1);
	objects->createObject(orange, 54 + x2o, 35 + y2o, 13 + z2o, ctCollect, 1);
	objects->createObject(orange, 54 + x2o, 35 + y2o, -27 + z2o, ctCollect, 1);

	enemies->createEnemy(simpleEnemy, 31 + x2o, 2 + y2o, -6 + z2o, 76 + x2o, 2 + y2o, -6 + z2o, NULL, 0, 0, NULL, 0, 0, 3, 15, ctEnemy);
	enemies->createEnemy(simpleEnemy, 76 + x2o, 2 + y2o, 32 + z2o, 31 + x2o, 2 + y2o, 32 + z2o, NULL, 0, 0, NULL, 0, 0, 3, 15, ctEnemy);

	enemies->createEnemy(simpleEnemy, 27 + x2o, 2 + y2o, -62 + z2o, 27 + x2o, 2 + y2o, -42 + z2o, NULL, 0, 0, NULL, 0, 0, 3, 15, ctEnemy);
	enemies->createEnemy(simpleEnemy, 47 + x2o, 2 + y2o, -62 + z2o, 27 + x2o, 2 + y2o, -62 + z2o, NULL, 0, 0, NULL, 0, 0, 3, 15, ctEnemy);

	///unkillable enemies must be placed at the end
	enemies->createEnemy(tractor, 4.0f + x2o, 13 + y2o, 88.0f + z2o, 4 + x2o, 13 + y2o, -96 + z2o, 103 + x2o, 13 + y2o, -96 + z2o, 103 + x2o, 13 + y2o, 88 + z2o, 1, 30, ctUnkillable);
	enemies->createEnemy(tractor, 103 + x2o, 13 + y2o, -96 + z2o, 103 + x2o, 13 + y2o, 88 + z2o, 4.0f + x2o, 13 + y2o, 88.0f + z2o, 4 + x2o, 13 + y2o, -96 + z2o, 1, 30, ctUnkillable);

	enemies->CreateBoundingBox();
	objects->CreateBoundingBox();
	level->CreateBoundingBox();

	std::vector <XNA::AxisAlignedBox> temp;
	temp = level->getLevelPartsCollisions();
		for (UINT i = 0; i < temp.size(); i++)
		{
			levelCollisions.push_back(temp[i]);
		}
	
	temp = objects->getObjectCollisions();
		for (UINT i = 0; i < temp.size(); i++)
		{
			levelCollisions.push_back(temp[i]);
			totalCollectibles++;
		}
		temp = enemies->getEnemyCollisions();
		for (UINT i = 0; i < temp.size(); i++)
		{
			levelCollisions.push_back(temp[i]);
			totalEnemies++;
		}
		mController = new Controller( playerOne, cam );
		// init controller
		mController->InitControllerInput( nullptr );
}

void PlayState::Exiting()
{
	cout << "Playing state is exiting" << endl;
}

void PlayState::Update(float dt)
{
	addDeltaTime(dt);
	
	enemies->update(dt);
	
	int levelColsize = levelCollisions.size();
	int tempOtherObject;

	//////updates the enemy collisions as they move
	std::vector <XNA::AxisAlignedBox> temp;
	//original value = 3
	temp = enemies->getEnemyCollisions();
	
	std::vector <XNA::AxisAlignedBox> tempObject;
	//original value = 5
	tempObject = objects->getObjectCollisions();
	
	std::vector <XNA::AxisAlignedBox> tempLevel;
	//original value = 26
	tempLevel = level->getLevelPartsCollisions();
	tempOtherObject = tempObject.size() + temp.size() + tempLevel.size();
	
	int tempSize = temp.size();
	int sizeDifference = 0;
	
	sizeDifference = levelCollisions.size() - tempOtherObject;
	
	
	if (sizeDifference > 0)
	{
			int something = 0;
			levelCollisions.pop_back();
	
			int j = 0;
			for (UINT i = tempLevel.size(); i < (tempLevel.size() + tempObject.size()); i++, j++)
			{
			levelCollisions[i] = tempObject[j];
		}
	
		if (temp.size() >= 0)
		{
			j = 0;
			for (UINT i = (tempLevel.size() + tempObject.size()); i < (tempLevel.size() + tempObject.size() + temp.size()); i++, j++)
			{
				levelCollisions[i] = temp[j];
				levelCollisions[i].Center = temp[j].Center;
			}
		}
	}
	else
	{
		int	j = 0;
		for (UINT i = tempObject.size() + tempLevel.size(); i < tempOtherObject; i++, j++)
		{
			levelCollisions[i] = temp[j];
		}
	}
	
	playerOne->setLevelCollisions(levelCollisions);
	
	XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR playerPos = XMLoadFloat3(&playerPosition);
	XMVECTOR camRight = XMLoadFloat3(&cam->GetRight());
	XMVECTOR camForward = XMLoadFloat3(&cam->GetLook());
	XMVECTOR camUp = XMLoadFloat3(&cam->GetUp());
	XMVECTOR multiply = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);
	
	camUp = XMVectorAdd(camUp, multiply);
	
	bool jumpChar = false;
	bool moveChar = false;
	
	
	if (GetAsyncKeyState('W') & 0x8000)
	{
		desiredCharDir += (camForward);
		moveChar = true;
	}
	
	if (GetAsyncKeyState('S') & 0x8000)
	{
		desiredCharDir += -(camForward);
		moveChar = true;
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		desiredCharDir += (camRight);
		moveChar = true;
	}
	
	if (GetAsyncKeyState('D') & 0x8000)
	{
		desiredCharDir += -(camRight);
		moveChar = true;
	}
	
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		float dy = 1.5 * dt;
		cam->RotateY(-dy);
	}
	
	if (GetAsyncKeyState('E') & 0x8000)
	{
		float dy = 1.5 * dt;
		cam->RotateY(dy);
	}
	
	
	if (GetAsyncKeyState('R') & 0x8000)
	{
		float dy = 0.25 * dt;
		cam->Pitch(dy);
	}
	
	if (GetAsyncKeyState('F') & 0x8000)
	{
		float dy = 0.25 * dt;
		cam->Pitch(-dy);
	}
			if (GetAsyncKeyState( VK_SPACE ))
	{
		desiredCharDir += camUp;
		moveChar = true;
	}
	
	mController->CheckControllerState( nullptr );
	// work-around for getting directionchange made by controller (if any)
	desiredCharDir += mController->GetCharDirection();

	XMVECTOR addGravity = XMVectorSet( 0.0f, -30.f * DeltaTimeF, 0.0f, 0.0f );
	XMFLOAT3 tGrav;
	XMStoreFloat3( &tGrav, addGravity );
		
	////send player information to the camera
	
	cam->getPlayerPos(playerOne->getPlayerPosition());
	cam->getDeltaTime(dt);
	
	cam->moveCam();
	
	playerOne->update( dt, desiredCharDir, enemies, objects);
	mController->ResetSettings();

}

void PlayState::Draw()
{
	deviceContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	deviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	deviceContext->IASetInputLayout(InputLayouts::Basic32);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cam->UpdateViewMatrix();
	// Set per frame constants.
	ID3DX11EffectTechnique* activeTexTech = Effects::BasicFX->Light3TexTech;
	Effects::BasicFX->SetDirLights(dirLights);
	Effects::BasicFX->SetEyePosW(cam->GetPosition());
	Effects::BasicFX->SetCubeMap(sky->CubeMapSRV());
	

	enemies->draw(deviceContext, *cam, activeTexTech);
	objects->draw(deviceContext, *cam, activeTexTech);
	level->draw(deviceContext, *cam, activeTexTech);
	playerOne->drawPlayer(deviceContext, *cam, activeTexTech);
	sky->Draw(deviceContext, *cam);

	// restore default states, as the SkyFX changes them in the effect file.
	deviceContext->RSSetState(0);
	deviceContext->OMSetDepthStencilState(0, 0);
	HR(mSwapChain->Present(0, 0));
}
void PlayState::Obscuring()
{

}

void PlayState::Revealed()
{

}

void PlayState::addDeltaTime(float dt)
{
	DeltaTimeF = dt;
}

float PlayState::getDeltaTime()
{
	return DeltaTimeF;
}
