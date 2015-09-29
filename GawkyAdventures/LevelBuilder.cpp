
#include "LevelBuilder.h"

#include "Effects.h"
#include "Camera.h"
#include "LevelParts.h"
#include "ModelEnum.cpp"








LevelBuilder::LevelBuilder(ID3D11Device* device, TextureMgr& texMgr)
{



	mGround = new BasicModel(device, texMgr, "Models\\Ground.obj", L"Textures\\");
	mPlatform = new BasicModel(device, texMgr, "Models\\Platform2.obj", L"Textures\\");
	mVinebox = new BasicModel(device, texMgr, "Models\\vinebox.obj", L"Textures\\");
	mSmallTree = new BasicModel(device, texMgr, "Models\\sTree.obj", L"Textures\\");
	mTreeTrunk = new BasicModel(device, texMgr, "Models\\treetrunk.obj", L"Textures\\");
	mTreeTop = new BasicModel(device, texMgr, "Models\\treetop.obj", L"Textures\\");
	mFence1 = new BasicModel(device, texMgr, "Models\\Fence.obj", L"Textures\\");
	mFence2 = new BasicModel(device, texMgr, "Models\\Fence2.obj", L"Textures\\");
	mCattail = new BasicModel(device, texMgr, "Models\\cattail.obj", L"Textures\\");
	mHouseSide = new BasicModel(device, texMgr, "Models\\houseside.obj", L"Textures\\");
	mHouseBack = new BasicModel(device, texMgr, "Models\\houseback.obj", L"Textures\\");
	mHouseRoof = new BasicModel(device, texMgr, "Models\\houseroof.obj", L"Textures\\");
	mSandBox = new BasicModel(device, texMgr, "Models\\sandbox.obj", L"Textures\\");
	mFencePart1 = new BasicModel(device, texMgr, "Models\\fencepart1.obj", L"Textures\\");
	mFencePart2 = new BasicModel(device, texMgr, "Models\\fencepart2.obj", L"Textures\\");
	mlvl2Ground = new BasicModel(device, texMgr, "Models\\lvl2Ground.obj", L"Textures\\");
	mTestBarn = new BasicModel(device, texMgr, "Models\\barn.obj", L"Textures\\");

	mBarnBack = new BasicModel(device, texMgr, "Models\\bBack.obj", L"Textures\\");
	mBarnFrontSide = new BasicModel(device, texMgr, "Models\\bFrontSide.obj", L"Textures\\");
	mBarnFrontTop = new BasicModel(device, texMgr, "Models\\bFrontTop.obj", L"Textures\\");
	mBarnRoof = new BasicModel(device, texMgr, "Models\\bRoof.obj", L"Textures\\");
	mBarnSide = new BasicModel(device, texMgr, "Models\\bSide.obj", L"Textures\\");
	mBarnFrontSide2 = new BasicModel(device, texMgr, "Models\\bFrontSide2.obj", L"Textures\\");
	mRoundBail = new BasicModel(device, texMgr, "Models\\roundbail.obj", L"Textures\\");
	mSquareBail = new BasicModel(device, texMgr, "Models\\squarebail.obj", L"Textures\\");
	mWoodPile = new BasicModel(device, texMgr, "Models\\woodpile.obj", L"Textures\\");







}


LevelBuilder::~LevelBuilder()
{
	for (UINT i = 0; i < LevelPartsclass.size(); ++i)
	{

		delete LevelPartsclass[i];


	}
}






void LevelBuilder::draw(ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech)
{



	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;


	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();



	for (UINT modelIndex = 0; modelIndex < mLevelPartsInstances.size(); ++modelIndex)
	{
		world = XMLoadFloat4x4(&mLevelPartsInstances[modelIndex].World);
		worldInvTranspose = MathHelper::InverseTranspose(world);
		worldViewProj = world*view*proj;

		Effects::BasicFX->SetWorld(world);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetWorldViewProj(worldViewProj);




		//I do not have shadoews
		//Effects::BasicFX->SetShadowTransform(world*shadowTransform);
		Effects::BasicFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

		for (UINT subset = 0; subset < mLevelPartsInstances[modelIndex].Model->SubsetCount; ++subset)
		{
			Effects::BasicFX->SetMaterial(mLevelPartsInstances[modelIndex].Model->Mat[subset]);
			Effects::BasicFX->SetDiffuseMap(mLevelPartsInstances[modelIndex].Model->DiffuseMapSRV[subset]);
			//Effects::BasicFX->SetNormalMap(mModelInstances[modelIndex].Model->NormalMapSRV[subset]);

			activeTexTech->GetPassByIndex(0)->Apply(0, dc);
			mLevelPartsInstances[modelIndex].Model->ModelMesh.Draw(dc, subset);


		}

	}

}





void LevelBuilder::addLevelParts(BasicModelInstance theLevelParts)
{

	mLevelPartsInstances.push_back(theLevelParts);

}



void LevelBuilder::createLevelParts(int model, FLOAT x, FLOAT y, FLOAT z, int collisionstype, int scale, FLOAT rotation)
{
	LevelParts* newLevelParts;

	newLevelParts = new LevelParts();

	XMMATRIX modelScale = XMMatrixScaling(scale, scale, -scale);
	XMMATRIX modelRot = XMMatrixRotationY(rotation);
	XMMATRIX modelOffset = XMMatrixTranslation(x, y, z);


	newLevelParts->setModelScale(modelScale);
	newLevelParts->setModelRot(modelRot);
	newLevelParts->setModelOffset(modelOffset);


	if (model == Ground)
	{
		anLevelParts = mGround;

	}
	else if (model == Platform)
	{

		anLevelParts = mPlatform;
	}
	else if (model == Vinebox)
	{

		anLevelParts = mVinebox;
	}
	else if (model == SmallTree)
	{

		anLevelParts = mSmallTree;
	}
	else if (model == TreeTrunk)
	{

		anLevelParts = mTreeTrunk;
	}
	else if (model == TreeTop)
	{
		anLevelParts = mTreeTop;

	}
	else if (model == Fence1)
	{
		anLevelParts = mFence1;

	}
	else if (model == Fence2)
	{
		anLevelParts = mFence2;

	}
	else if (model == Cattail)
	{
		anLevelParts = mCattail;

	}
	else if (model == HouseSide)
	{
		anLevelParts = mHouseSide;
	}
	else if (model == HouseBack)
	{
		anLevelParts = mHouseBack;

	}
	else if (model == HouseRoof)
	{

		anLevelParts = mHouseRoof;
	}
	else if (model == SandBox)
	{

		anLevelParts = mSandBox;
	}
	else if (model == FencePart1)
	{
		anLevelParts = mFencePart1;

	}
	else if (model == FencePart2)
	{
		anLevelParts = mFencePart2;

	}
	else if (model == lvl2Ground)
	{
		anLevelParts = mlvl2Ground;

	}
	else if (model == testbarn)
	{
		anLevelParts = mTestBarn;

	}
	else if (model == barnback)
	{
		anLevelParts = mBarnBack;

	}
	else if (model == barnfrontside)
	{
		anLevelParts = mBarnFrontSide;

	}
	else if (model == barnfronttop)
	{
		anLevelParts = mBarnFrontTop;

	}
	else if (model == barnroof)
	{
		anLevelParts = mBarnRoof;

	}
	else if (model == barnside)
	{
		anLevelParts = mBarnSide;

	}
	else if (model == barnfrontside2)
	{
		anLevelParts = mBarnFrontSide2;

	}




	else if (model == roundbail)
	{
		anLevelParts = mRoundBail;

	}
	else if (model == squarebail)
	{
		anLevelParts = mSquareBail;

	}
	else if (model == woodpile)
	{
		anLevelParts = mWoodPile;

	}









	newLevelParts->setModel(anLevelParts);

	newLevelParts->setRotationY(rotation);


	BasicModelInstance oneLevelParts;


	theLevelParts.Model = anLevelParts;


	XMStoreFloat4x4(&theLevelParts.World, modelScale*modelRot*modelOffset);


	newLevelParts->setBasicMInstance(theLevelParts);

	oneLevelParts = newLevelParts->getBasicMInstance();



	addLevelParts(theLevelParts);

	LevelPartsclass.push_back(newLevelParts);


	LevelCollisions.push_back(LevelPartsBox);

	newLevelParts->setWorld(theLevelParts.World);


	newLevelParts->setCollisionType(collisionstype);


	///set the scale
	newLevelParts->setScale(scale);



}



void LevelBuilder::CreateBoundingBox()
{


	//
	// Compute scene bounding box.
	//
	XMFLOAT3 minPt(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 maxPt(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);



	for (UINT i = 0; i < mLevelPartsInstances.size(); ++i)
	{
		minPt.x = 0.0f;
		minPt.y = 0.0f;
		minPt.z = 0.0f;

		maxPt.x = 0.0f;
		maxPt.y = 0.0f;
		maxPt.z = 0.0f;

		for (UINT j = 0; j < mLevelPartsInstances[i].Model->BasicVertices.size(); ++j)
		{

			XMFLOAT3 P = mLevelPartsInstances[i].Model->BasicVertices[j].Pos;




			minPt.x = MathHelper::Min(minPt.x, P.x);
			minPt.y = MathHelper::Min(minPt.y, P.y);
			minPt.z = MathHelper::Min(minPt.z, P.z);

			maxPt.x = MathHelper::Max(maxPt.x, P.x);
			maxPt.y = MathHelper::Max(maxPt.y, P.y);
			maxPt.z = MathHelper::Max(maxPt.z, P.z);


		}

		XMMATRIX temp = XMLoadFloat4x4(&mLevelPartsInstances[i].World);

		LevelPartsclass[i]->setWorld(mLevelPartsInstances[i].World);

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

		LevelCollisions[i].collisionType = LevelPartsclass[i]->getCollisionType();

		// sets the scale of the collision box 
		int scale = LevelPartsclass[i]->getScale();

		LevelCollisions[i].Extents.x = LevelCollisions[i].Extents.x * scale;
		LevelCollisions[i].Extents.y = LevelCollisions[i].Extents.y * scale;
		LevelCollisions[i].Extents.z = LevelCollisions[i].Extents.z * scale;

		if (LevelPartsclass[i]->getRotationY() != 0)
		{
			FLOAT tempX = LevelCollisions[i].Extents.x;
			FLOAT tempZ = LevelCollisions[i].Extents.z;

			LevelCollisions[i].Extents.x = tempZ;
			LevelCollisions[i].Extents.z = tempX;

		}


		//// this doesn't work, useless atm
		//LevelPartsBox[i]->setCollisionType();


		LevelPartsclass[i]->setAABB(&LevelCollisions[i]);

	}
}



std::vector<BasicModelInstance> LevelBuilder::getLevelParts()
{

	return mLevelPartsInstances;
}



std::vector <XNA::AxisAlignedBox> LevelBuilder::getLevelPartsCollisions()
{







	return LevelCollisions;
}




void LevelBuilder::update(float dt)
{



	DeltaTime = dt;

	for (int i = 0; i < mLevelPartsInstances.size(); i++)
	{

		LevelPartsclass[i]->update(DeltaTime);

		mLevelPartsInstances[i].World = LevelPartsclass[i]->GetWorld();

		XMMATRIX temp = XMLoadFloat4x4(&mLevelPartsInstances[i].World);
		XMVECTOR Scale;
		XMVECTOR Rotation;
		XMVECTOR Position;

		XMFLOAT3 tempPosition;

		XMMatrixDecompose(&Scale, &Rotation, &Position, temp);
		XMStoreFloat3(&tempPosition, Position);



		LevelCollisions[i].Center = tempPosition;


	}








}


void LevelBuilder::RemovemLevelPartsInstance(int number)
{

	mLevelPartsInstances.erase(mLevelPartsInstances.begin() + number);

	LevelCollisions.erase(LevelCollisions.begin() + number);
	delete(LevelPartsclass[number]);
}