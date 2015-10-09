#include "d3dUtil.h"
#include "d3dApp.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include "Camera.h"
#include "Vertex.h"
#ifndef SHADOW_H
#define SHADOW_H

#include "d3dUtil.h"

class Shadow
{
public:
	Shadow();
	~Shadow();
	void GetPlayerPos(XMFLOAT3 playerPos);
	void CreateShadow(XMFLOAT3 playerPos, ID3D11DeviceContext* dc,ID3D11Device* dv);
private:
	XMFLOAT3 playerPosition;
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;
	XMFLOAT4X4 mSphereWorld;
	UINT mSphereIndexCount;
	UINT mSphereIndexOffset;
	int mSphereVertexOffset;
	Camera mCam;
	ID3D11Buffer* mShapesVB;
};
#endif // SHADOW_H