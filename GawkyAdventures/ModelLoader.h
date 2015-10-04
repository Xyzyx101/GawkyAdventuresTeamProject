#pragma once
#include <vector>
#include "Vertex.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "xnacollision.h"
#include "Model.h"

class Skeleton;
class AnimationController;

class ModelLoader {
public:
	ModelLoader();
	~ModelLoader();
	bool Load( ID3D11Device* device, const std::string& filename, Vertex::VERTEX_TYPE type, Model& outModel, Skeleton* outSkeleton = nullptr, AnimationController* = nullptr );
	XNA::AxisAlignedBox GetBoundingBox();
	//bool OBJLoader::LoadOBJ( ID3D11Device* device, const std::string& filename, std::vector<Vertex::Basic32>& vertices, std::vector<USHORT>& indices, std::vector<MeshGeometry::Subset>& subsets, std::vector<M3dMaterial>& mats, bool vFlipped, bool RHSModel )
private:
	void Clear();
	void CreateIndexBuffer();
	void CreateVertexBuffer( Vertex::VERTEX_TYPE type );
	void CreateSkeleton();
	void CreateBoneHierarchy();
	void FindBoneChildren( aiNode* node, int parentIdx );
	void CreateAnimations();
	//void FindBoneChildren( aiNode* node, int parentIdx );
	inline void UpdateExtents( float x, float y, float z );
	XMMATRIX ConvertMatrix( aiMatrix4x4 inMat );
	ID3D11Device*			device;
	const aiScene*			scene;
	UINT					indexCount;
	ID3D11Buffer*			ib;
	ID3D11Buffer*			vb;
	float					minX;
	float					minY;
	float					minZ;
	float					maxX;
	float					maxY;
	float					maxZ;
	Skeleton*				skeleton;
	AnimationController*	animationController;

	template <typename VertexType>
	void SetVertices( ID3D11Device* device, UINT count, const VertexType* vertices ) {
		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = sizeof( VertexType ) * count;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices;

		HR( device->CreateBuffer( &vbd, &vinitData, &vb ) );
	}

	struct BoneWeight {
		BoneWeight::BoneWeight( int boneIndex, float weight ) : boneIndex( boneIndex ), weight( weight ) {}
		int boneIndex;
		float weight;
	};
};

