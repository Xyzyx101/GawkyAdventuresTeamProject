
#include "Player.h"
#include "Effects.h"
#include "Camera.h"
#include "TheObjects.h"
#include "Enemies.h"
#include "ModelEnum.h"
#include "Model.h"
#include "ModelLoader.h"


Player::Player(ID3D11Device* device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath, FLOAT x, FLOAT y, FLOAT z) : 
	mPlayerPosition(0.0f, 2.0f, 0.0f),
	mPlayerRotationQuad(0.0f, 0.0f, 0.0f, 0.0f), 
	mPlayerScale(0.03f, 0.03f, -0.03f),
	isJump(false), isFalling(false), 
	hitHead(false), 
	hitFeet(false), 
	currGround(3.0f), 
	fellOffMap(false), 
	mOPlayerPosition(0.0f, 2.0f, 0.0f), 
	mOPlayerRotationQuad(0.0f, 0.0f, 0.0f, 0.0f),
	mOPlayerScale(3.0f, 3.0f, -3.0f), 
	mOPlayerRotation(0.0f, 0.0f, 0.0f, 1.0f), 
	isAlive(true), 
	isImmune(false), 
	isTripping(false)
{
	currCharDirection = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	oldCharDirection = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	charPosition = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	PlayerForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	PlayerRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	Jump = XMVectorSet(0.0f, 5.0f, 0.0f, 0.0f);

	XMVECTOR S = XMLoadFloat3(&mPlayerScale);
	XMVECTOR P = XMLoadFloat3(&mPlayerPosition);
	XMVECTOR Q = XMLoadFloat4(&mPlayerRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mPlayerRotation);
	
	////set the player model
	
	//playerModel = new BasicModel(device, texMgr, modelFilename, texturePath);

	//mPlayer.Model = playerModel;
	XMStoreFloat4x4(&mPlayer.World, XMMatrixAffineTransformation(S, rot, Q, P));

	//
	// make the players bounding box
	//
	/*XMFLOAT3 minPt(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 maxPt(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
		minPt.x = 0.0f;
		minPt.y = 0.0f;
		minPt.z = 0.0f;
		maxPt.x = 0.0f;
		maxPt.y = 0.0f;
		maxPt.z = 0.0f;

		for (UINT j = 0; j < mPlayer.Model->BasicVertices.size(); ++j)
		{
			XMFLOAT3 P = mPlayer.Model->BasicVertices[j].Pos;
			minPt.x = MathHelper::Min(minPt.x, P.x);
			minPt.y = MathHelper::Min(minPt.y, P.y);
			minPt.z = MathHelper::Min(minPt.z, P.z);

			maxPt.x = MathHelper::Max(maxPt.x, P.x);
			maxPt.y = MathHelper::Max(maxPt.y, P.y);
			maxPt.z = MathHelper::Max(maxPt.z, P.z);
		}

		XMMATRIX temp = XMLoadFloat4x4(&mPlayer.World);
		XMVECTOR Scale;
		XMVECTOR Position;
		XMVECTOR Rotation;
		XMMatrixDecompose(&Scale, &Rotation, &Position, temp);

		XMFLOAT3 tempPos;
		XMStoreFloat3(&tempPos, Position);
		mPlayerBox.Center = tempPos;
		mPlayerBox.Extents = XMFLOAT3(0.5f*(maxPt.x - minPt.x),
			0.5f*(maxPt.y - minPt.y),
			0.5f*(maxPt.z - minPt.z));
		mPlayerBox.Extents.x = mPlayerBox.Extents.x * 3;
		mPlayerBox.Extents.y = mPlayerBox.Extents.y * 3;
		mPlayerBox.Extents.z = mPlayerBox.Extents.z * 3;
		mPlayerBox.collisionType = 99;*/
}

Player::~Player()
{}

bool Player::init( ID3D11Device* device, ModelLoader* loader, TextureMgr& texMgr, const std::wstring& texturePath ) {
	if( !loader->Load( device, "./Models/gawky2.fbx", Vertex::BASIC_32, mModel, &skeleton, &animController ) ) {
		return false;
	}
	
	skeleton.SetAnimationController( &animController );

	// New AssImp changes
	mDiffuseSRV = texMgr.CreateTexture( texturePath+L"Gawky2_diffuse_color.png" );
	mMaterial.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.f );
	mMaterial.Diffuse = XMFLOAT4( 0.8f, 0.8f, 0.8f, 1.f );
	mMaterial.Specular = XMFLOAT4( 0.5f, 0.5f, 0.5f, 32.f ); // w = spec pow
	mMaterial.Reflect = XMFLOAT4( 0.f, 0.f, 0.f, 0.f );

	XNA::AxisAlignedBox aabb = loader->GetBoundingBox();
	mPlayerBox.Center = aabb.Center;

	// TODO : note this scale is scaling everything uniformly by the x.  I don't understand the implications of a negative z scale so I just 
	// forced uniform scaling for now.
	mPlayerBox.Extents = XMFLOAT3( aabb.Extents.x*mPlayerScale.x, aabb.Extents.y*mPlayerScale.x, aabb.Extents.z*mPlayerScale.x );
	mPlayerBox.collisionType = ctPlayer;

	return true;
}

void Player::setLevelCollisions(std::vector <XNA::AxisAlignedBox> &thelevelCollisions)
{
	LevelCollisions = thelevelCollisions;
}


void Player::update()
{
	XMVECTOR S = XMLoadFloat3(&mPlayerScale);
	XMVECTOR P = XMLoadFloat3(&mPlayerPosition);
	XMVECTOR Q = XMLoadFloat4(&mPlayerRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mPlayerRotation);

	XMVECTOR OS = XMLoadFloat3(&mOPlayerScale);
	XMVECTOR OP = XMLoadFloat3(&mOPlayerPosition);
	XMVECTOR OQ = XMLoadFloat4(&mOPlayerRotationQuad);
	XMVECTOR Orot = XMLoadFloat4(&mOPlayerRotation);

	mPlayerBox.Center = mPlayerPosition;

	if (hitFeet == true)
	{
		isJump = false;
		isFalling = false;
	}

	if (isFalling == true || isJump == true)
	{
		currGround = -100.0f;
	}
	if (fellOffMap == true)
	{
		currGround = -500.0f;
	}

	if (mPlayerPosition.y < currGround)
	{
		P = XMVectorSetY(P, currGround);
		hitHead = false;
		onGround = true;
	}

	if (isAlive == true)
	{
		XMStoreFloat3(&mPlayerBox.Center, P);
		XMStoreFloat3(&mPlayerPosition, P);
		XMStoreFloat4x4(&mPlayer.World, XMMatrixAffineTransformation(S, rot, Q, P));
	}
	else if (isAlive == false)
	{
		mPlayerBox.Center = mOPlayerPosition;
		mPlayerRotation = mOPlayerRotation;
		mPlayerPosition = mOPlayerPosition;
		mPlayerRotationQuad = mOPlayerRotationQuad;

		XMStoreFloat4x4(&mPlayer.World, XMMatrixAffineTransformation(OS, Orot, OQ, OP));

		isAlive = true;
	}
}


void Player::drawPlayer(ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech)
{
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();

	world = XMLoadFloat4x4(&mPlayer.World);
	worldInvTranspose = MathHelper::InverseTranspose(world);
	worldViewProj = world*view*proj;
	Effects::GawkyFX->SetWorld(world);
	Effects::GawkyFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::GawkyFX->SetWorldViewProj(worldViewProj);
	
	Effects::GawkyFX->SetTexTransform(XMMatrixScaling(1.0f, 1.0f, 1.0f));

	//Old Draw
	/*for (UINT subset = 0; subset < mPlayer.Model->SubsetCount; ++subset)
	{
		Effects::BasicFX->SetMaterial(mPlayer.Model->Mat[subset]);
		Effects::BasicFX->SetDiffuseMap(mPlayer.Model->DiffuseMapSRV[subset]);
		
		activeTexTech->GetPassByIndex(0)->Apply(0, dc);
		mPlayer.Model->ModelMesh.Draw(dc, subset);
		
	}*/
	
	skeleton.SetRootTransform( mPlayer.World );

	// Assimp Draw
	Effects::GawkyFX->SetMaterial( mMaterial );
	Effects::GawkyFX->SetDiffuseMap( mDiffuseSRV );
	activeTexTech->GetPassByIndex( 0 )->Apply( 0, dc );
	UINT offset = 0;
	UINT stride = mModel.VertexStride();
	ID3D11Buffer* vBuffers[1] = { mModel.VB() };
	dc->IASetVertexBuffers( 0, 1, &vBuffers[0], &stride, &offset );
	ID3D11Buffer* iBuffer =  mModel.IB();
	dc->IASetIndexBuffer( iBuffer, mModel.IndexFormat(), 0 );
	dc->DrawIndexed( mModel.IndexCount(), 0, 0 );
}

void Player::move(float dt, XMVECTOR direction, Enemies* guys, TheObjects* things)
{

	int collobject = 0;
	int collEnemy = 0;

	XMVECTOR S;
	XMVECTOR P;
	XMVECTOR Q;

	XMVECTOR PS;
	XMVECTOR PP;
	XMVECTOR PQ;

	XMMATRIX worldMatrix = XMLoadFloat4x4(&mPlayer.World);
	XMVECTOR r = XMLoadFloat3(&mPlayerPosition);
	FLOAT tempY = XMVectorGetY(direction);

	/// if the character is tripping, push him in the direction he was going until he goes 10 spaces
	if (isTripping == true && isImmune == false)
	{
		startJumpPos = r;
		isImmune = true;
		tripDirection = direction;
		tripDistance = direction;

	}
	else if (isTripping == true && isImmune == true)
	{
		direction = tripDirection;
		tripDistance = XMVectorAdd(tripDistance, tripDirection);

		//XMVector4Normalize(tripDistance);
		FLOAT tempZ = XMVectorGetZ(tripDistance);
		FLOAT tempX = XMVectorGetX(tripDistance);

		if (tempZ < 0)
		{
			tempZ = tempZ * -1;
		}

		if (tempX < 0)
		{
			tempX = tempX * -1;
		}

		FLOAT totalTripDistance = tempZ + tempX;
		if (totalTripDistance > 30.0f || totalTripDistance < -30.0f)
		{
			isImmune = false;
			isTripping = false;
		}
	}

	///if the player is on the ground, they are allowed to jump,  if not they cannot, no flying allowed
	if (onGround == true && tempY > 0.5f)
	{
		//Don't want to normalize the jump
		onGround = false;
		startJumpPos = r;
		isJump = true;
		hitFeet = false;
	}

	if (onGround == false && isJump == true)
	{
		FLOAT startPos = XMVectorGetY(startJumpPos);
		FLOAT currPos = XMVectorGetY(r);
		tempY = dt * 30.0f;
		if (currPos > startPos + 10.0f)
		{
			isJump = false;
			isFalling = true;
		}
		else if (hitHead == true)
		{
			isFalling = true;
			isJump = false;
		}
	}

	// Normalize our destinated direction vector
	direction = XMVector3Normalize(direction);
	direction = XMVectorSetY(direction, 0.0f);

	///// the direction the player is going to move
	moveDirection = direction;
	/////character spinning make it more smooth
	if (XMVectorGetX(XMVector3Dot(direction, oldCharDirection)) == -1)
	{
		oldCharDirection += XMVectorSet(0.01f, 0.0f, 0.0f, 0.0f);
	}

	///////get characters position in world space
	charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	charPosition = XMVector3TransformCoord(charPosition, worldMatrix);

	///// rotate the character
	float destDirLength = 10.0f * dt;

	currCharDirection = (oldCharDirection)+(direction * destDirLength);
	currCharDirection = XMVector3Normalize(currCharDirection);

	// get the angle 
	float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(currCharDirection), XMVector3Normalize(PlayerForward)));

	if (XMVectorGetY(XMVector3Cross(currCharDirection, PlayerForward)) > 0.0f)
	{
		charDirAngle = -charDirAngle;
	}

	float speed = 30.0f * dt;
	XMVECTOR tY = XMVectorSet(0.0f, tempY, 0.0f, 0.0f);

	direction = XMVectorSetY(direction, tempY);
	direction = direction * speed;
	charPosition = charPosition + direction;

	charPosition += tY;

	XMMATRIX rotationMatrix;
	XMMATRIX previousWorld = worldMatrix;

	XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(charPosition), XMVectorGetY(charPosition), XMVectorGetZ(charPosition));
	rotationMatrix = XMMatrixRotationY(charDirAngle - 3.14159265f);		// Subtract PI from angle so the character doesn't run backwards

	worldMatrix =  rotationMatrix * Translation;
	
	XMMatrixDecompose(&PS, &PQ, &PP, previousWorld);
	XMMatrixDecompose(&S, &Q, &P, worldMatrix);
	
	FLOAT prevY = XMVectorGetY(PP);
	FLOAT currY = XMVectorGetY(P);

	/////////////////////////////////////////////////////////////  v collision system

	for (UINT i = 0; i < LevelCollisions.size(); ++i)
	{
		XMStoreFloat3(&mPlayerBox.Center, charPosition);

		// collisions with things in the world
		XMVECTOR diff;

		mPlayerBox;//1

		LevelCollisions[i].Center;
		LevelCollisions[i].Extents;

		int it = 0;

		FLOAT tRight = 0;
		FLOAT tUp = 0;
		FLOAT tForward = 0;
		bool toRight = false;
		bool Above = false;
		bool inFront = false;

		///values just to check values

		float LX = LevelCollisions[i].Center.x;
		float LY = LevelCollisions[i].Center.y;
		float LZ = LevelCollisions[i].Center.z;

		float LXE = LevelCollisions[i].Extents.x;
		float LYE = LevelCollisions[i].Extents.y;
		float LZE = LevelCollisions[i].Extents.z;

		/////////////fooling around with some collision idea's

		//Player is to the left 
		if (mPlayerBox.Center.x <= LevelCollisions[i].Center.x)
		{
			tRight = (LevelCollisions[i].Center.x - LevelCollisions[i].Extents.x) - (mPlayerBox.Center.x + mPlayerBox.Extents.x);
		}
		//player is to the right of the object
		if (mPlayerBox.Center.x >= LevelCollisions[i].Center.x)
		{
			tRight = (mPlayerBox.Center.x - mPlayerBox.Extents.x) - (LevelCollisions[i].Center.x + LevelCollisions[i].Extents.x);
			toRight = true;
		}

		///player is behind object
		if (mPlayerBox.Center.z <= LevelCollisions[i].Center.z)
		{
			tForward = (LevelCollisions[i].Center.z - LevelCollisions[i].Extents.z) - (mPlayerBox.Center.z + mPlayerBox.Extents.z);
		}
		//player is infront of object
		if (mPlayerBox.Center.z >= LevelCollisions[i].Center.z)
		{
			tForward = (mPlayerBox.Center.z - mPlayerBox.Extents.z) - (LevelCollisions[i].Center.z + LevelCollisions[i].Extents.z);
			inFront = true;
		}

		///player is below the object
		if (mPlayerBox.Center.y <= LevelCollisions[i].Center.y)
		{
			tUp = ((LevelCollisions[i].Center.y - LevelCollisions[i].Extents.y) - (mPlayerBox.Center.y + mPlayerBox.Extents.y)); //+ LevelCollisions[i].Extents.y);
		}
		//player is above the object
		if (mPlayerBox.Center.y >= LevelCollisions[i].Center.y)
		{
			tUp = ((mPlayerBox.Center.y - mPlayerBox.Extents.y) - (LevelCollisions[i].Center.y + LevelCollisions[i].Extents.y)); //- LevelCollisions[i].Extents.y);
			Above = true;
		}

		FLOAT oldPos = XMVectorGetY(oldCharDirection);
		FLOAT curPos = XMVectorGetY(currCharDirection);

		FLOAT insideX = LevelCollisions[i].Center.x;
		FLOAT insideZ = LevelCollisions[i].Center.z;

		FLOAT insideExtentX = LevelCollisions[i].Extents.x;
		FLOAT insideExtentZ = LevelCollisions[i].Extents.z;

		insideX -+ LevelCollisions[i].Extents.x;
		insideZ -+ LevelCollisions[i].Extents.z;

		//// if the player hits an object that trips him
		if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f && LevelCollisions[i].collisionType == ctStumble)
		{
			tripDirection = direction;
			isTripping = true;
			break;
		}
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f && LevelCollisions[i].collisionType == 3)
		{
			things->RemovemObjectInstance(collobject);
			break;
		}
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f && LevelCollisions[i].collisionType == ctEnemy && isImmune == true)
		{
			guys->RemovemObjectInstance(collEnemy);
			break;
		}

		// if player hit their head
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f && isJump == true && LevelCollisions[i].collisionType == 0)
		{
			int t = 3;
			// these tell me where the player is verse the object it collided with
			inFront;
			Above;
			toRight;
			hitHead = true;
			XMStoreFloat3(&mPlayerPosition, PP);
			break;
		}

		///if player lands on an object for the first time
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f && Above == true && hitFeet == false && LevelCollisions[i].collisionType == 0
			&& (mPlayerBox.Center.y - mPlayerBox.Extents.y) < (LevelCollisions[i].Center.y + LevelCollisions[i].Extents.y) && prevY > currY &&
			prevY > (LevelCollisions[i].Center.y + LevelCollisions[i].Extents.y) && onGround == false )
		{
			int t = 3;
			hitFeet = true;	

			insideX;
			insideZ;

			currentObject = i;		
			currGround = XMVectorGetY(PP);
			currGround += 0.1f;
			onGround = true;
			XMStoreFloat3(&mPlayerPosition, PP);
			break;

		}
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f && Above == true && LevelCollisions[i].collisionType == 0 && (mPlayerBox.Center.y - mPlayerBox.Extents.y) < (LevelCollisions[i].Center.y + LevelCollisions[i].Extents.y))
		{
			int t = 3;

			isTripping = false;
			
			inFront;
			Above;
			toRight;

			P;
			PP;
		
			FLOAT originalPosX = XMVectorGetX(PP);
			FLOAT originalPosZ = XMVectorGetZ(PP);
			FLOAT newY = XMVectorGetY(P);

			XMVectorSetZ(P, originalPosZ);
			XMVectorSetZ(P, originalPosX);

			P = XMVectorSet(originalPosX, newY, originalPosZ, 0.0);

			XMStoreFloat3(&mPlayerPosition, P);
			break;
		}

		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f  && LevelCollisions[i].collisionType == 0)
		{
			int t = 3;

			inFront;
			Above;
			toRight;
			isTripping = false;

			P;
			PP;

			oldCharDirection = currCharDirection;
	
			FLOAT originalPosX = XMVectorGetX(PP);
			FLOAT originalPosZ = XMVectorGetZ(PP);
			FLOAT newY = XMVectorGetY(P);

			XMVectorSetZ(P, originalPosZ);
			XMVectorSetZ(P, originalPosX);

			P = XMVectorSet(originalPosX, newY, originalPosZ, 0.0);
			XMStoreFloat3(&mPlayerPosition, P);

			break;
		}
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f  && LevelCollisions[i].collisionType == 1 && isImmune == false)
		{
			isAlive = false;
			break;
		}
		else if (tRight <= 0.0f && tUp <= 0.0f && tForward <= 0.0f  && LevelCollisions[i].collisionType == ctUnkillable)
		{
			isAlive = false;
			break;
		}
		else if (isAlive == true)
		{
			// Set the characters old direction
			XMStoreFloat3(&mPlayerPosition, P);
			XMStoreFloat4(&mPlayerRotationQuad, Q);
		}
		oldCharDirection = currCharDirection;
	
		if (LevelCollisions[i].collisionType == 2 || LevelCollisions[i].collisionType == 3)
		{
			collobject++;
		}

		if (LevelCollisions[i].collisionType == 1 )
		{
			collEnemy++;
		}
		
	}
	////////////////////////////////////////////////////////////// ^ collisions

	//	for (UINT i = 0; i < 2; ++i)
	//	{


	//	if (i == 1)
	//	{
	///		currentObject = 0;

	//	}

	FLOAT tRight = 0;
	FLOAT tUp = 0;
	FLOAT tForward = 0;
	bool toRight = false;
	bool Above = false;
	bool inFront = false;

	//Player is to the left 
	if (mPlayerBox.Center.x <= LevelCollisions[currentObject].Center.x)
	{
		tRight = (LevelCollisions[currentObject].Center.x - LevelCollisions[currentObject].Extents.x) - (mPlayerBox.Center.x + mPlayerBox.Extents.x);
	}
	//player is to the right of the object
	if (mPlayerBox.Center.x >= LevelCollisions[currentObject].Center.x)
	{
		tRight = (mPlayerBox.Center.x - mPlayerBox.Extents.x) - (LevelCollisions[currentObject].Center.x + LevelCollisions[currentObject].Extents.x);
		toRight = true;
	}

	///player is behind object
	if (mPlayerBox.Center.z <= LevelCollisions[currentObject].Center.z)
	{
		tForward = (LevelCollisions[currentObject].Center.z - LevelCollisions[currentObject].Extents.z) - (mPlayerBox.Center.z + mPlayerBox.Extents.z);
	}
	//player is infront of object
	if (mPlayerBox.Center.z >= LevelCollisions[currentObject].Center.z)
	{
		tForward = (mPlayerBox.Center.z - mPlayerBox.Extents.z) - (LevelCollisions[currentObject].Center.z + LevelCollisions[currentObject].Extents.z);
		inFront = true;
	}

	if (tRight > 0.0f || tForward > 0.0f)
	{
		isFalling = true;
		hitFeet = false;
		onGround = false;
		if (currentObject == 0)
		{
			fellOffMap = true;
		}
	}
}

////getters
XMFLOAT3 Player::getPlayerPosition()
{
	return mPlayerPosition;
}

bool Player::getOnGround()
{
	return onGround;
}

////setters
void Player::setMoveDirection(XMVECTOR mDirection)
{
	moveDirection = mDirection;
}
