
#include "Enemy.h"
#include "Effects.h"


#include "Camera.h"





Enemy::Enemy() : mEnemyPosition(0.0f, 0.0f, 0.0f), mEnemyScale(3.0f, 3.0f, 3.0f), mEnemyRotation(0.0f, 0.0f, 0.0f, 1.0f),
mEnemyRotationQuad(0.0f, 0.0f, 0.0f, 0.0f), mEnemyPositionOne(0.0f, 0.0f, 0.0f), mEnemyPositionTwo(0.0f, 0.0f, 0.0f), travelToPoint(2), timesThrough(0)
{



	///initialize player
	XMVECTOR S = XMLoadFloat3(&mEnemyScale);
	XMVECTOR P = XMLoadFloat3(&mEnemyPosition);
	XMVECTOR Q = XMLoadFloat4(&mEnemyRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mEnemyRotation);
	XMStoreFloat4x4(&mEnemyWorld, XMMatrixAffineTransformation(S, rot, Q, P));


	currCharDirection = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	oldCharDirection = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	charPosition = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	EnemyForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	EnemyRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);


	direction = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	lastPoint = mEnemyPositionOne;

}


Enemy::~Enemy()
{
}

//setters

void Enemy::setcollisiontype(int collisiontype)
{

	mcollisiontype = collisiontype;
}

void Enemy::setSpeed(FLOAT theSpeed)
{
	speed = theSpeed;
}


void Enemy::setScale(FLOAT scale)
{

	theScale = scale;
}


void Enemy::setModelScale(XMMATRIX& Scale)

{

	modelScale = Scale;

}

void Enemy::setModelRot(XMMATRIX& Rot)
{
	modelRot = Rot;

}
void Enemy::setModelOffset(XMMATRIX& Offset)
{


	modelOffset = Offset;

}

void Enemy::setModel(BasicModel* Enemy)
{

	anEnemy = Enemy;

}

void Enemy::setBasicMInstance(BasicModelInstance model)
{

	theEnemy = model;
}

void Enemy::setAABB(XNA::AxisAlignedBox* tempEnemyBox)
{

	EnemyBox = tempEnemyBox;
}


void Enemy::setWorld(XMFLOAT4X4 tempEnemyWorld)
{


	mEnemyWorld = tempEnemyWorld;


	XMMATRIX tempWorld = XMLoadFloat4x4(&mEnemyWorld);


	XMVECTOR Scale;
	XMVECTOR Position;
	XMVECTOR Rotation;


	XMMatrixDecompose(&Scale, &Rotation, &Position, tempWorld);

	XMStoreFloat3(&mEnemyPosition, Position);

	XMStoreFloat3(&mEnemyScale, Scale);

	XMStoreFloat4(&mEnemyRotation, Rotation);





	XMVECTOR S = XMLoadFloat3(&mEnemyScale);
	XMVECTOR P = XMLoadFloat3(&mEnemyPosition);
	XMVECTOR Q = XMLoadFloat4(&mEnemyRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mEnemyRotation);





	XMStoreFloat4x4(&mEnemyStartingWorld, XMMatrixAffineTransformation(S, rot, Q, P));











}




//getters

int Enemy::getcollisiontype()
{

	return mcollisiontype;
}


FLOAT Enemy::getScale()
{

	return theScale;
}
XMMATRIX Enemy::getModelScale()
{

	return modelScale;
}
XMMATRIX Enemy::getModelRot()
{


	return modelRot;
}
XMMATRIX Enemy::getModelOffset()
{


	return modelOffset;
}

BasicModel* Enemy::getModel()
{

	return anEnemy;
}

BasicModelInstance Enemy::getBasicMInstance()
{


	return theEnemy;
}

XNA::AxisAlignedBox* Enemy::getEnemyAABB()
{


	return EnemyBox;
}

XMFLOAT4X4 Enemy::GetWorld()
{


	return mEnemyWorld;
}


XMFLOAT3 Enemy::getEnemyPosition()
{


	return mEnemyPosition;
}

void Enemy::SetPositionOne(FLOAT x, FLOAT y, FLOAT z)
{

	XMVECTOR temp = XMVectorSet(x, y, z, 0.0f);
	XMStoreFloat3(&mEnemyPositionOne, temp);

}


void Enemy::SetPositionTwo(FLOAT x, FLOAT y, FLOAT z)
{

	XMVECTOR temp = XMVectorSet(x, y, z, 0.0f);
	XMStoreFloat3(&mEnemyPositionTwo, temp);


}

void Enemy::SetPositionThree(FLOAT x, FLOAT y, FLOAT z)
{

	XMVECTOR temp = XMVectorSet(x, y, z, 0.0f);
	XMStoreFloat3(&mEnemyPositionThree, temp);


}

void Enemy::SetPositionFour(FLOAT x, FLOAT y, FLOAT z)
{

	XMVECTOR temp = XMVectorSet(x, y, z, 0.0f);
	XMStoreFloat3(&mEnemyPositionFour, temp);


}









void Enemy::update(FLOAT dt)
{

	move(dt);

	XMVECTOR S = XMLoadFloat3(&mEnemyScale);
	XMVECTOR P = XMLoadFloat3(&mEnemyPosition);
	XMVECTOR Q = XMLoadFloat4(&mEnemyRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mEnemyRotation);
	XNA::AxisAlignedBox tempEnemyBox;

	tempEnemyBox.Center = mEnemyPosition;

	EnemyBox = &tempEnemyBox;


	XMStoreFloat4x4(&mEnemyWorld, XMMatrixAffineTransformation(S, rot, Q, P));

}



void Enemy::move(FLOAT dt)
{


	bool dontUpdate = false;

	XMMATRIX startingworldMatrix = XMLoadFloat4x4(&mEnemyStartingWorld);
	XMVECTOR S;
	XMVECTOR P;
	XMVECTOR Q;

	XMMatrixDecompose(&S, &Q, &P, startingworldMatrix);


	XMVECTOR enemyPosition = XMLoadFloat3(&mEnemyPosition);



	FLOAT newPos = XMVectorGetZ(enemyPosition);
	FLOAT oldPos = XMVectorGetZ(P);

	direction = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);



	FLOAT diffX;
	FLOAT diffY;
	FLOAT diffZ;

	diffX = 0;
	diffY = 0;
	diffZ = 0;





	if (travelToPoint == 2)
	{


		diffX = mEnemyPosition.x - mEnemyPositionTwo.x;
		diffY = mEnemyPosition.y - mEnemyPositionTwo.y;
		diffZ = mEnemyPosition.z - mEnemyPositionTwo.z;
		mEnemyPositionThree.x;

	
	
	

		int nothing = 0;
		if (diffX < 0.0f)
		{

			diffX *= -1;
		}
		if (diffY < 0.0f)
		{

			diffY *= -1;
		}
		if (diffZ < 0.0f)
		{

			diffZ *= -1;
		}

		//////////////////////////////////
		if (diffX < 0.01)
		{

			mEnemyPosition.x = mEnemyPositionTwo.x;
		}
		if (diffY < 0.01)
		{

			mEnemyPosition.y = mEnemyPositionTwo.y;
		}
		if (diffZ < 0.01)
		{

			mEnemyPosition.z = mEnemyPositionTwo.z;
		}
		////////////////////////////////


		if (mEnemyPosition.x > mEnemyPositionTwo.x)
		{

			direction += XMVectorSet(-1.0f, 0.0f, 0, 0.0f);


		}
		else if (mEnemyPosition.x < mEnemyPositionTwo.x)
		{

			direction += XMVectorSet(1.0f, 0.0f, 0, 0.0f);

		}



		if (mEnemyPosition.z > mEnemyPositionTwo.z)
		{

			direction += XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);


		}
		else if (mEnemyPosition.z < mEnemyPositionTwo.z)
		{

			direction += XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		}


		if (mEnemyPosition.y > mEnemyPositionTwo.y)
		{

			direction += XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);


		}
		else if (mEnemyPosition.y < mEnemyPositionTwo.y)
		{

			direction += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		}
		if (diffX < 0.05f && diffY < 0.05f && diffZ < 0.05f)
		{

		
			if (timesThrough == 0)
			{
				if (mEnemyPositionThree.x == 0)
				{

					dontUpdate = true;
					travelToPoint = 1;

					lastPoint = mEnemyPositionTwo;

				}
				else if (mEnemyPositionThree.x != 0)
				{
					dontUpdate = true;
					travelToPoint = 3;

					lastPoint = mEnemyPositionTwo;

				}
			}
			else
			{
				dontUpdate = true;

				travelToPoint = 1;
				lastPoint = mEnemyPositionTwo;
			}

			int something = 1;
		}

	}



	/////////////////////////////////////////
	else if (travelToPoint == 1)
	{

		timesThrough = 0;

		diffX = mEnemyPosition.x - mEnemyPositionOne.x;
		diffY = mEnemyPosition.y - mEnemyPositionOne.y;
		diffZ = mEnemyPosition.z - mEnemyPositionOne.z;



		if (diffX < 0.0f)
		{

			diffX *= -1;
		}
		if (diffY < 0.0f)
		{

			diffY *= -1;
		}
		if (diffZ < 0.0f)
		{

			diffZ *= -1;
		}


		//////////////////////////////////
		if (diffX < 0.01)
		{

			mEnemyPosition.x = mEnemyPositionOne.x;
		}
		if (diffY < 0.01)
		{

			mEnemyPosition.y = mEnemyPositionOne.y;
		}
		if (diffZ < 0.01)
		{

			mEnemyPosition.z = mEnemyPositionOne.z;
		}
		////////////////////////////////



		if (mEnemyPosition.x > mEnemyPositionOne.x)
		{

			direction += XMVectorSet(-1.0f, 0.0f, 0, 0.0f);


		}
		if (mEnemyPosition.x < mEnemyPositionOne.x)
		{

			direction += XMVectorSet(1.0f, 0.0f,0, 0.0f);

		}



		if (mEnemyPosition.z > mEnemyPositionOne.z && diffZ > 0.01)
		{

			direction += XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);


		}
		if (mEnemyPosition.z < mEnemyPositionOne.z && diffZ > 0.01)
		{

			direction += XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		}


		if (mEnemyPosition.y > mEnemyPositionOne.y)
		{

			direction += XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);


		}
		if (mEnemyPosition.y < mEnemyPositionOne.y)
		{

			direction += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		}
		if (diffX < 0.05f && diffY < 0.05f && diffZ < 0.05f)
		{
			
			travelToPoint = 2;

			lastPoint = mEnemyPositionOne;

		}
	}



	////////////////////////////////////////////////////////////////////////////////
	else if (travelToPoint == 3 && timesThrough == 0)
	{


		diffX = mEnemyPosition.x - mEnemyPositionThree.x;
		diffY = mEnemyPosition.y - mEnemyPositionThree.y;
		diffZ = mEnemyPosition.z - mEnemyPositionThree.z;


		int nothing;
		if (diffX < 0.0f)
		{

			diffX *= -1;
		}
		if (diffY < 0.0f)
		{

			diffY *= -1;
		}
		if (diffZ < 0.0f)
		{

			diffZ *= -1;
		}


		//////////////////////////////////
		if (diffX < 0.01)
		{

			mEnemyPosition.x = mEnemyPositionThree.x;
		}
		if (diffY < 0.01)
		{

			mEnemyPosition.y = mEnemyPositionThree.y;
		}
		if (diffZ < 0.01)
		{

			mEnemyPosition.z = mEnemyPositionThree.z;
		}
		////////////////////////////////



		if (mEnemyPosition.x > mEnemyPositionThree.x)
		{

			direction += XMVectorSet(-1.0f, 0.0f, 0, 0.0f);


		}
		if (mEnemyPosition.x < mEnemyPositionThree.x)
		{

			direction += XMVectorSet(1.0f, 0.0f, 0, 0.0f);

		}



		if (mEnemyPosition.z > mEnemyPositionThree.z)
		{

			direction += XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);


		}
		if (mEnemyPosition.z < mEnemyPositionThree.z)
		{

			direction += XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		}


		if (mEnemyPosition.y > mEnemyPositionThree.y)
		{

			direction += XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);


		}
		if (mEnemyPosition.y < mEnemyPositionThree.y)
		{

			direction += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		}
		if (diffX < 0.05f && diffY < 0.05f && diffZ < 0.05f)
		{

			if (mEnemyPositionFour.x == NULL)
			{
					


				travelToPoint = 2;
				timesThrough = 1;
			}
			else
			{
				
				travelToPoint = 4;
		
			}
		}
	}


	else if (travelToPoint == 4)
	{


		diffX = mEnemyPosition.x - mEnemyPositionFour.x;
		diffY = mEnemyPosition.y - mEnemyPositionFour.y;
		diffZ = mEnemyPosition.z - mEnemyPositionFour.z;

		if (diffX < 0.0f)
		{

			diffX *= -1;
		}
		if (diffY < 0.0f)
		{

			diffY *= -1;
		}
		if (diffZ < 0.0f)
		{

			diffZ *= -1;
		}

		//////////////////////////////////
		if (diffX < 0.001)
		{

			mEnemyPosition.x = mEnemyPositionFour.x;
		}
		if (diffY < 0.001)
		{

			mEnemyPosition.y = mEnemyPositionFour.y;
		}
		if (diffZ < 0.001)
		{

			mEnemyPosition.z = mEnemyPositionFour.z;
		}
		////////////////////////////////



		if (mEnemyPosition.x > mEnemyPositionFour.x)
		{

			direction += XMVectorSet(-1.0f, 0.0f, 0, 0.0f);


		}
		if (mEnemyPosition.x < mEnemyPositionFour.x)
		{

			direction += XMVectorSet(1.0f, 0.0f,0, 0.0f);

		}



		if (mEnemyPosition.z > mEnemyPositionFour.z && diffZ > 0.01)
		{

			direction += XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);


		}
		if (mEnemyPosition.z < mEnemyPositionFour.z && diffZ > 0.01)
		{

			direction += XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		}


		if (mEnemyPosition.y > mEnemyPositionFour.y)
		{

			direction += XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);


		}
		if (mEnemyPosition.y < mEnemyPositionFour.y)
		{

			direction += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		}
		if (diffX < 0.05f && diffY < 0.05f && diffZ < 0.05f)
		{

			
			travelToPoint = 1;

			lastPoint = mEnemyPositionFour;
		}
	}












	XMMATRIX worldMatrix = XMLoadFloat4x4(&mEnemyWorld);
	XMVECTOR r = XMLoadFloat3(&mEnemyPosition);



	// Normalize our destinated direction vector
	direction = XMVector3Normalize(direction);





	/////character spinning make it more smooth

	if (XMVectorGetX(XMVector3Dot(direction, oldCharDirection)) == -1)
	{
		oldCharDirection += XMVectorSet(1.11f, 1.0f, 0.0f, 0.0f);
	}


	///////get characters position in world space
	charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	charPosition = XMVector3TransformCoord(charPosition, worldMatrix);



	///// rotate the character
	float destDirLength = 10.0f * dt;



	currCharDirection = (oldCharDirection)+(direction * destDirLength);
	currCharDirection = XMVector3Normalize(currCharDirection);


	// get the angle 
	float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals(XMVector3Normalize(currCharDirection), XMVector3Normalize(EnemyForward)));



	if (XMVectorGetY(XMVector3Cross(currCharDirection, EnemyForward)) > 0.0f)
	{
		charDirAngle = -charDirAngle;
	}



	float Speed = speed * dt;



	direction = direction * Speed;



	charPosition = charPosition + direction;



	XMMATRIX rotationMatrix;





	XMMATRIX Translation = XMMatrixTranslation(XMVectorGetX(charPosition), XMVectorGetY(charPosition), XMVectorGetZ(charPosition));

	rotationMatrix = XMMatrixRotationY(charDirAngle - 3.14159265f);		// Subtract PI from angle so the character doesn't run backwards



	worldMatrix = rotationMatrix * Translation;


	XMMatrixDecompose(&S, &Q, &P, worldMatrix);


	XMStoreFloat3(&mEnemyPosition, P);

	XMStoreFloat4(&mEnemyRotationQuad, Q);




	XMStoreFloat4x4(&mEnemyWorld, worldMatrix);



	oldCharDirection = currCharDirection;



}
