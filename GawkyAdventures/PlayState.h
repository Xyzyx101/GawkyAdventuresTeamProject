#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <memory>
#include <iostream>

#include "DefaultGameStateManager.h"
#include "GameState.h"
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
#include "Game.h"
#include "GameTimer.h"

class Controller;

class PlayState : public GameState
{
public:
	PlayState(const std::shared_ptr<GameStateManager> &gameStateManager, 
			ID3D11Device* device, 
			ID3D11DeviceContext* deviceContext, 
			ID3D11RenderTargetView* mRenderTargetView,
			ID3D11DepthStencilView* mDepthStencilView, 
			IDXGISwapChain* mSwapChain,
			ID3DX11EffectTechnique* activeTexTech,
			Camera* cam);

	void Entered();
	void Exiting();
	void Update(float dt);
	void Draw();
	void Obscuring();
	void Revealed();
	void OnResize();

	float PlayState::getDeltaTime();

private:
	std::shared_ptr<GameStateManager> gameStateManager;
	Sky* sky;
	Controller* mController;
	DirectionalLight dirLights[3];
	UINT lightCount;
	Camera* cam;
	TextureMgr texMgr;
	GameTimer time;
	
	std::vector<BasicModelInstance> modelInstances;
	std::vector<XNA::AxisAlignedBox> levelCollisions;
	
	Player* playerOne;
	Enemies* enemies;
	TheObjects* objects;
	LevelBuilder* level;

	XMFLOAT3 playerPosition;
	XMFLOAT3 DeltaTime;

	XMVECTOR playerForward;
	XMVECTOR playerRight;
	XMVECTOR playerUp;
	
	float DeltaTimeF;
	int totalEnemies;
	int totalCollectibles;

	void addDeltaTime(float dt);

protected:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3DX11EffectTechnique* activeTexTech;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	IDXGISwapChain* mSwapChain;	
};

#endif //PLAYSTATE_H