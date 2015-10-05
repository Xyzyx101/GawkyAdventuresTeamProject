
#include "Enemies.h"

#include "Effects.h"
#include "Camera.h"
#include "Enemy.h"
#include "ModelEnum.h"

Enemies::Enemies(ID3D11Device* device, TextureMgr& texMgr)
{
	mSimpleEnemy = new BasicModel(device, texMgr, "Models\\simpleenemy.obj", L"Textures\\");
	mTractor = new BasicModel(device, texMgr, "Models\\tractor.obj", L"Textures\\");
}

Enemies::~Enemies()
{
	for (UINT i = 0; i < enemyclass.size(); ++i)
	{
		delete enemyclass[i];
	}
}

void Enemies::draw(ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();

	for (UINT modelIndex = 0; modelIndex < mEnemyInstances.size(); ++modelIndex)
	{
		world = XMLoadFloat4x4(&mEnemyInstances[modelIndex].World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::GawkyFX->SetWorld(world);
		Effects::GawkyFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::GawkyFX->SetWorldViewProj(worldViewProj);




		//I do not have shadows
		//Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::GawkyFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (UINT subset = 0; subset < mEnemyInstances[modelIndex].Model->SubsetCount; ++subset)
		{
			Effects::GawkyFX->SetMaterial(mEnemyInstances[modelIndex].Model->Mat[subset]);
			Effects::GawkyFX->SetDiffuseMap(mEnemyInstances[modelIndex].Model->DiffuseMapSRV[subset]);
			//Effects::BasicFX->SetNormalMap(mModelInstances[modelIndex].Model->NormalMapSRV[subset]);

			activeTexTech->GetPassByIndex(0)->Apply(0, dc);
			mEnemyInstances[modelIndex].Model->ModelMesh.Draw(dc, subset);




		}

	}




}





void Enemies::addEnemy(BasicModelInstance theEnemy)
{



	mEnemyInstances.push_back(theEnemy);


}



void Enemies::createEnemy(int model, FLOAT x1, FLOAT y1, FLOAT z1, FLOAT x2, FLOAT y2, FLOAT z2, FLOAT x3, FLOAT y3, FLOAT z3, FLOAT x4, FLOAT y4, FLOAT z4, FLOAT scale, int speed, int collisionstype)
{
	Enemy* newEnemy;

	newEnemy = new Enemy();

	XMMATRIX modelScale = XMMatrixScaling(scale, scale, -scale);
	XMMATRIX modelRot = XMMatrixRotationY(0);
	XMMATRIX modelOffset = XMMatrixTranslation(x1, y1, z1);


	newEnemy->setModelScale(modelScale);
	newEnemy->setModelRot(modelRot);
	newEnemy->setModelOffset(modelOffset);

	newEnemy->SetPositionOne(x1, y1, z1);
	newEnemy->SetPositionTwo(x2, y2, z2);
	newEnemy->SetPositionThree(x3, y3, z3);
	newEnemy->SetPositionFour(x4, y4, z4);



	if (model == simpleEnemy)
	{
		anEnemy = mSimpleEnemy;
	}
	else if (model == tractor)
	{
		anEnemy = mTractor;
	}


	

	newEnemy->setModel(anEnemy);
	newEnemy->setScale(scale);
	newEnemy->setSpeed(speed);
	newEnemy->setcollisiontype(collisionstype);


	BasicModelInstance oneEnemy;


	theEnemy.Model = anEnemy;


	XMStoreFloat4x4(&theEnemy.World, modelScale*modelRot*modelOffset);


	newEnemy->setBasicMInstance(theEnemy);

	oneEnemy = newEnemy->getBasicMInstance();



	addEnemy(theEnemy);

	enemyclass.push_back(newEnemy);


	LevelCollisions.push_back(EnemyBox);

	newEnemy->setWorld(theEnemy.World);





}



void Enemies::CreateBoundingBox()
{


	//
	// Compute scene bounding box.
	//
	XMFLOAT3 minPt(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 maxPt(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);



	for (UINT i = 0; i < mEnemyInstances.size(); ++i)
	{
		minPt.x = 0.0f;
		minPt.y = 0.0f;
		minPt.z = 0.0f;

		maxPt.x = 0.0f;
		maxPt.y = 0.0f;
		maxPt.z = 0.0f;

		for (UINT j = 0; j < mEnemyInstances[i].Model->BasicVertices.size(); ++j)
		{

			XMFLOAT3 P = mEnemyInstances[i].Model->BasicVertices[j].Pos;


			//////multiply all these by 7

			minPt.x = MathHelper::Min(minPt.x, P.x);
			minPt.y = MathHelper::Min(minPt.y, P.y);
			minPt.z = MathHelper::Min(minPt.z, P.z);

			maxPt.x = MathHelper::Max(maxPt.x, P.x);
			maxPt.y = MathHelper::Max(maxPt.y, P.y);
			maxPt.z = MathHelper::Max(maxPt.z, P.z);


		}

		XMMATRIX temp = XMLoadFloat4x4(&mEnemyInstances[i].World);

		enemyclass[i]->setWorld(mEnemyInstances[i].World);

		XMVECTOR Scale;
		XMVECTOR Position;
		XMVECTOR Rotation;


		XMMatrixDecompose(&Scale, &Rotation, &Position, temp);

		XMFLOAT3 tempPos;
		XMStoreFloat3(&tempPos, Position);

		LevelCollisions[i].Center = tempPos;


		LevelCollisions[i].Extents = XMFLOAT3(0.5f*(maxPt.x - minPt.x),
			0.5f*(maxPt.y - minPt.y),
			0.5f*(maxPt.z - minPt.z));

		LevelCollisions[i].collisionType = enemyclass[i]->getcollisiontype();


		FLOAT scale = enemyclass[i]->getScale();

		LevelCollisions[i].Extents.x = LevelCollisions[i].Extents.x * scale;
		LevelCollisions[i].Extents.y = LevelCollisions[i].Extents.y * scale;
		LevelCollisions[i].Extents.z = LevelCollisions[i].Extents.z * scale;

		EnemyBox.collisionType = 1;


		enemyclass[i]->setAABB(&LevelCollisions[i]);

	}
}



std::vector<BasicModelInstance> Enemies::getEnemy()
{

	return mEnemyInstances;
}



std::vector <XNA::AxisAlignedBox> Enemies::getEnemyCollisions()
{







	return LevelCollisions;
}




void Enemies::update(float dt)
{



	DeltaTime = dt;

	for (int i = 0; i < mEnemyInstances.size(); i++)
	{

		enemyclass[i]->update(DeltaTime);

		mEnemyInstances[i].World = enemyclass[i]->GetWorld();

		XMMATRIX temp = XMLoadFloat4x4(&mEnemyInstances[i].World);
		XMVECTOR Scale;
		XMVECTOR Rotation;
		XMVECTOR Position;

		XMFLOAT3 tempPosition;

		XMMatrixDecompose(&Scale, &Rotation, &Position, temp);
		XMStoreFloat3(&tempPosition, Position);



		LevelCollisions[i].Center = tempPosition;


	}

}


void Enemies::RemovemObjectInstance(int number)
{

	mEnemyInstances.erase(mEnemyInstances.begin() + number);

	LevelCollisions.erase(LevelCollisions.begin() + number);
	delete(enemyclass[number]);
	enemyclass.erase(enemyclass.begin() + number);
}