#include "Object.h"
#include "Effects.h"
#include "Camera.h"

Object::Object() : mObjectPosition(0.0f, 0.0f, 0.0f), mObjectScale(1.0f, 1.0f, 1.0f), mObjectRotation(0.0f, 0.0f, 0.0f, 1.0f), mObjectRotationQuad(0.0f, 0.0f, 0.0f, 0.0f)
{
	///initialize player
	XMVECTOR S = XMLoadFloat3(&mObjectScale);
	XMVECTOR P = XMLoadFloat3(&mObjectPosition);
	XMVECTOR Q = XMLoadFloat4(&mObjectRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mObjectRotation);
	XMStoreFloat4x4(&mObjectWorld, XMMatrixAffineTransformation(S, rot, Q, P));
	currCharDirection = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	oldCharDirection = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	charPosition = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);
	ObjectForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	ObjectRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	direction = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
}


Object::~Object()
{
}

//setters
void Object::setModelScale(XMMATRIX& Scale)

{
	modelScale = Scale;
}

void Object::setModelRot(XMMATRIX& Rot)
{
	modelRot = Rot;
}
void Object::setModelOffset(XMMATRIX& Offset)
{
	modelOffset = Offset;
}

void Object::setModel(BasicModel* Object)
{
	anObject = Object;
}

void Object::setBasicMInstance(BasicModelInstance model)
{
	theObject = model;
}

void Object::setAABB(XNA::AxisAlignedBox* tempObjectBox)
{
	ObjectBox = tempObjectBox;
}

void Object::setCollisionType(int theCollisionType)
{
	collisionType = theCollisionType;
}

void Object::setWorld(XMFLOAT4X4 tempObjectWorld)
{
	mObjectWorld = tempObjectWorld;

	XMMATRIX tempWorld = XMLoadFloat4x4(&mObjectWorld);
	XMVECTOR Scale;
	XMVECTOR Position;
	XMVECTOR Rotation;

	XMMatrixDecompose(&Scale, &Rotation, &Position, tempWorld);
	XMStoreFloat3(&mObjectPosition, Position);
	XMStoreFloat3(&mObjectScale, Scale);
	XMStoreFloat4(&mObjectRotation, Rotation);

	XMVECTOR S = XMLoadFloat3(&mObjectScale);
	XMVECTOR P = XMLoadFloat3(&mObjectPosition);
	XMVECTOR Q = XMLoadFloat4(&mObjectRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mObjectRotation);

	XMStoreFloat4x4(&mObjectStartingWorld, XMMatrixAffineTransformation(S, rot, Q, P));

}

void Object::setScale(int scale)
{
	mScale = scale;
}
//getters
XMMATRIX Object::getModelScale()
{
	return modelScale;
}
XMMATRIX Object::getModelRot()
{
	return modelRot;
}
XMMATRIX Object::getModelOffset()
{
	return modelOffset;
}

BasicModel* Object::getModel()
{
	return anObject;
}

BasicModelInstance Object::getBasicMInstance()
{
	return theObject;
}

XNA::AxisAlignedBox* Object::getObjectAABB()
{
	return ObjectBox;
}

XMFLOAT4X4 Object::GetWorld()
{
	return mObjectWorld;
}

XMFLOAT3 Object::getObjectPosition()
{
	return mObjectPosition;
}

int Object::getCollisionType()
{
	return collisionType;
}

int Object::getScale()
{
	return mScale;
}

void Object::update(FLOAT dt)
{
	//move(dt);
	XMVECTOR S = XMLoadFloat3(&mObjectScale);
	XMVECTOR P = XMLoadFloat3(&mObjectPosition);
	XMVECTOR Q = XMLoadFloat4(&mObjectRotationQuad);
	XMVECTOR rot = XMLoadFloat4(&mObjectRotation);
	XNA::AxisAlignedBox tempObjectBox;
	tempObjectBox.Center = mObjectPosition;
	ObjectBox = &tempObjectBox;
	XMStoreFloat4x4(&mObjectWorld, XMMatrixAffineTransformation(S, rot, Q, P));
}

