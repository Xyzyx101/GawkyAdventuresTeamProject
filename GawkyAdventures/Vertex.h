//***************************************************************************************
// Vertex.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines vertex structures and input layouts.
//***************************************************************************************

#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

namespace Vertex
{
	enum VERTEX_TYPE {
		BASIC_32,
		POS_NORMAL_TEX_TAN,
	};
	
	// Basic 32-byte vertex structure.
	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};


	// Basic 32-byte vertex structure.
	// Is this used anywhere ????
	/*struct Basicmodel
	{
		Basicmodel(){}
		Basicmodel(float x, float y, float z,
			float u, float v,
			float nx, float ny, float nz)
			: Pos(x, y, z), Tex(u, v), Normal(nx, ny, nz){}



		XMFLOAT3 Pos;	
		XMFLOAT2 Tex;
		XMFLOAT3 Normal;
	};*/

	struct PosNormalTexTan
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
		XMFLOAT4 TangentU;
	};






}

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Pos[1];
	static const D3D11_INPUT_ELEMENT_DESC Basic32[3];
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC InstancedBasic32[8];

	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];

};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Pos;
	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* InstancedBasic32;

	static ID3D11InputLayout* PosNormalTexTan;
};

#endif // VERTEX_H
