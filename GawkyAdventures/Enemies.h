#ifndef Enemies_H
#define Enemies_H

#include "BasicModel.h"
#include "xnacollision.h"
#include "GameTimer.h"
#include "SoundSystem.h"

class BasicModel;
class Camera;
class Enemy;
class GameTimer;

class Enemies
{
public:
	Enemies(ID3D11Device* device, TextureMgr& texMgr);
	~Enemies();

	void createEnemy(int model, FLOAT x1, FLOAT y1, FLOAT z1, FLOAT x2, FLOAT y2, FLOAT z2, FLOAT x3, FLOAT y3, FLOAT z3, FLOAT x4, FLOAT y4, FLOAT z4, FLOAT scale, int speed, int collisionstype);
	void addEnemy(BasicModelInstance theEnemy);
	void RemovemObjectInstance(int number);

	void draw(ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech);
	void CreateBoundingBox();
	void update(float dt);
	std::vector<BasicModelInstance> getEnemy();
	std::vector <XNA::AxisAlignedBox> getEnemyCollisions();
	
private:

	std::vector<BasicModelInstance> mEnemyInstances;
	std::vector<Enemy*> enemyclass;
	std::vector <XNA::AxisAlignedBox> LevelCollisions;

	BasicModel* anEnemy;
	BasicModelInstance theEnemy;
	XNA::AxisAlignedBox EnemyBox;
	GameTimer timer;
	FLOAT DeltaTime;
	Enemy* newEnemy;
	BasicModel* mSimpleEnemy;
	BasicModel* mTractor;
};

#endif