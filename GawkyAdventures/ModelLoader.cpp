#include "ModelLoader.h"
#include <map>
#include "assimp\DefaultLogger.hpp"
#include "Skeleton.h"
#include "AnimationController.h"

ModelLoader::ModelLoader() :
device( nullptr ),
scene( nullptr ),
ib( nullptr ),
indexCount( 0 ),
vb( nullptr ) {
	Assimp::DefaultLogger::create( "assimp.log", Assimp::Logger::VERBOSE );
}

ModelLoader::~ModelLoader() {
	Assimp::DefaultLogger::kill();
}

bool ModelLoader::Load( ID3D11Device* device, const std::string& filename, Vertex::VERTEX_TYPE type, Model& outModel, Skeleton* outSkeleton, AnimationController* outAnimationController ) {
	this->device = device;
	// reset extents
	minX = minY = minZ = FLT_MAX;
	maxX = maxY = maxZ = FLT_MIN;
	Assimp::Importer importer;
	Assimp::DefaultLogger::get()->info( "***********    Importing: "+filename+"    ***********" );
	scene = importer.ReadFile( filename,
		aiProcess_CalcTangentSpace|
		aiProcess_ImproveCacheLocality|
		//aiProcess_MakeLeftHanded|
		aiProcess_FlipWindingOrder|
		aiProcess_Triangulate|
		aiProcess_JoinIdenticalVertices|
		aiProcess_SortByPType|
		aiProcess_FlipUVs|
		aiProcess_ValidateDataStructure|
		aiProcess_FindInvalidData
		);
	if( !scene ) {
		Assimp::DefaultLogger::get()->error( importer.GetErrorString() );
		return false;
	}
	if( !scene->HasMeshes() ) {
		Assimp::DefaultLogger::get()->error( "File contains no mesh" );
		return false;
	}
	CreateIndexBuffer();
	CreateVertexBuffer( type );

	if( scene->HasAnimations() ) {
		this->skeleton = outSkeleton;
		this->animationController = outAnimationController;
		// if the model has animations then the caller must pass in a skeleton and a animation controller
		assert( outSkeleton!=nullptr );
		assert( outAnimationController!=nullptr );
		CreateSkeleton();
		CreateBoneHierarchy();
		CreateAnimations();
	}

	outModel.SetIB( ib, indexCount );
	outModel.SetVB( vb );
	switch( type ) {
	case Vertex::BASIC_32:
	{
		outModel.SetVertexStride( sizeof( Vertex::Basic32 ) );
		break;
	}
	case Vertex::POS_NORMAL_TEX_TAN:
	{
		outModel.SetVertexStride( sizeof( Vertex::PosNormalTexTan ) );
		break;
	}
	case Vertex::POS_NORMAL_TEX_TAN_SKINNED:
	{
		outModel.SetVertexStride( sizeof( Vertex::PosNormalTexTanSkinned ) );
		break;
	}
	default:
		assert( false ); // this should never happen
	}
	return true;
}

void ModelLoader::CreateIndexBuffer() {
	aiMesh* mesh = scene->mMeshes[0];
	aiMesh* otherMesh = scene->mMeshes[1];
	UINT count = mesh->mNumFaces;
	const aiFace* faces = mesh->mFaces;
	indexCount = mesh->mNumFaces*3;
	std::vector<USHORT> indexData( indexCount );
	for( UINT faceIndex = 0, dataIndex = 0; faceIndex<count; ++faceIndex, dataIndex += 3 ) {
		assert( faces[faceIndex].mNumIndices==3 ); //mesh should be triangulated
		indexData[dataIndex] = (USHORT)faces[faceIndex].mIndices[0];
		indexData[dataIndex+1] = (USHORT)faces[faceIndex].mIndices[1];
		indexData[dataIndex+2] = (USHORT)faces[faceIndex].mIndices[2];
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof( USHORT ) * indexData.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indexData.data();
	HR( device->CreateBuffer( &ibd, &initData, &ib ) );
}

void ModelLoader::CreateVertexBuffer( Vertex::VERTEX_TYPE type ) {
	aiMesh* mesh = scene->mMeshes[0];
	UINT count = mesh->mNumVertices;
	aiVector3D* vertices = mesh->mVertices;
	/*  The switch case looks like duplicated code.  It is not.	 vertData is a different type in each and SetVertices() is a template. */
	switch( type ) {
	case Vertex::BASIC_32:
	{
		std::vector<Vertex::Basic32> vertData( count );
		aiVector3D* normals = mesh->mNormals;
		aiVector3D* texCoords = mesh->mTextureCoords[0];
		for( UINT i = 0; i<count; ++i ) {
			UpdateExtents( vertices[i].x, vertices[i].y, vertices[i].z );
			vertData[i].Pos = XMFLOAT3( vertices[i].x, vertices[i].y, vertices[i].z );
			vertData[i].Normal = XMFLOAT3( normals[i].x, normals[i].y, normals[i].z );
			vertData[i].Tex = XMFLOAT2( texCoords[i].x, texCoords[i].y );
		}
		SetVertices( device, count, vertData.data() );
		break;
	}
	case Vertex::POS_NORMAL_TEX_TAN:
	{
		aiVector3D* normals = mesh->mNormals;
		aiVector3D* texCoords = mesh->mTextureCoords[0];
		aiVector3D* tangents = mesh->mTangents;
		std::vector<Vertex::PosNormalTexTan> vertData( count );
		for( UINT i = 0; i<count; ++i ) {
			UpdateExtents( vertices[i].x, vertices[i].y, vertices[i].z );
			vertData[i].Pos = XMFLOAT3( vertices[i].x, vertices[i].y, vertices[i].z );
			vertData[i].Normal = XMFLOAT3( normals[i].x, normals[i].y, normals[i].z );
			vertData[i].Tex = XMFLOAT2( texCoords[i].x, texCoords[i].y );
			vertData[i].TangentU = XMFLOAT4( tangents[i].x, tangents[i].y, tangents[i].z, 0.f );
		}
		SetVertices( device, count, vertData.data() );
		break;
	}
	case Vertex::POS_NORMAL_TEX_TAN_SKINNED:
	{
		aiVector3D* normals = mesh->mNormals;
		aiVector3D* texCoords = mesh->mTextureCoords[0];
		aiVector3D* tangents = mesh->mTangents;
		std::vector<Vertex::PosNormalTexTanSkinned> vertData( count );
		for( UINT i = 0; i<count; ++i ) {
			UpdateExtents( vertices[i].x, vertices[i].y, vertices[i].z );
			vertData[i].Pos = XMFLOAT3( vertices[i].x, vertices[i].y, vertices[i].z );
			vertData[i].Normal = XMFLOAT3( normals[i].x, normals[i].y, normals[i].z );
			vertData[i].Tex = XMFLOAT2( texCoords[i].x, texCoords[i].y );
			vertData[i].TangentU = XMFLOAT4( tangents[i].x, tangents[i].y, tangents[i].z, 0.f );
		}

		// Bone Data
		std::multimap<int, BoneWeight> vertexBoneWeight;
		for( unsigned int boneIndex = 0; boneIndex<mesh->mNumBones; ++boneIndex ) {
			auto bone = mesh->mBones[boneIndex];
			for( int i = 0; i<bone->mNumWeights; ++i ) {
				auto boneWeight = BoneWeight( boneIndex, bone->mWeights[i].mWeight );
				vertexBoneWeight.insert( std::pair<int, BoneWeight>( bone->mWeights[i].mVertexId, boneWeight ) );
			}
		}
		for( UINT i = 0; i<count; ++i ) {
			BYTE boneIndices[4] = { 0, 0, 0, 0 };
			float weights[3] = { 0, 0, 0 };
			int j = 0;
			auto itlow = vertexBoneWeight.lower_bound( i );
			auto itup = vertexBoneWeight.upper_bound( i );
			assert( itlow!=itup ); // every vertex should have some influence
			for( auto it = itlow; it!=itup; ++it ) {
				if( j>=3 ) {
					break;
				}
				boneIndices[j] = it->second.boneIndex;
				weights[j] = it->second.weight;
				++j;
			}
			vertData[i].BoneIndicies[0] = boneIndices[0];
			vertData[i].BoneIndicies[1] = boneIndices[1];
			vertData[i].BoneIndicies[2] = boneIndices[2];
			//vertData[i].BoneIndicies[3] = boneIndices[3];
			vertData[i].Weights = XMFLOAT3( weights );
		}

		SetVertices( device, count, vertData.data() );
		break;
	}
	}
}

void ModelLoader::CreateSkeleton() {
	aiMesh* mesh = scene->mMeshes[0];
	aiBone** bones = mesh->mBones;
	UINT numBones = mesh->mNumBones;
	Bone* newBone;
	for( int i = 0; i<numBones; ++i ) {
		newBone = new Bone();
		aiBone* bone = bones[i];
		newBone->idx = i;
		newBone->name = bone->mName.data;
		XMMATRIX mat = ConvertMatrix( bone->mOffsetMatrix );
		XMStoreFloat4x4( &(newBone->offset), mat );
		skeleton->AddBone( newBone );
	}
}

void ModelLoader::CreateBoneHierarchy() {
	aiNode* root = scene->mRootNode->FindNode( "Skeleton_Root" );
	Bone* bone = skeleton->GetBoneByName( root->mName.data );
	if( bone==nullptr ) {
		bone = new Bone();
		bone->idx = skeleton->BoneCount();
		bone->name = root->mName.data;
		XMMATRIX mat = XMMatrixIdentity();
		XMStoreFloat4x4( &(bone->offset), mat );
		XMStoreFloat4x4( &(bone->localTransform), mat );
		skeleton->AddBone( bone );
	}
	assert( root!=nullptr ); // The root bone must be called Skeleton_Root
	FindBoneChildren( root, -1 );
}

void ModelLoader::FindBoneChildren( aiNode* node, int parentIdx ) {
	Bone* bone = skeleton->GetBoneByName( node->mName.data );
	bone->parentIdx = parentIdx;
	XMMATRIX transformMat;
	transformMat = ConvertMatrix( node->mTransformation );
	XMStoreFloat4x4( &(bone->localTransform), transformMat);
	if( node->mNumChildren==0 ) { return; }
	for( int i = 0; i<node->mNumChildren; ++i ) {
		aiNode* childNode = node->mChildren[i];
		std::string childName = childNode->mName.data;
		Bone* childBone = skeleton->GetBoneByName( childName );
		if( childBone==nullptr ) {
			// Bones with no skin influence will be missing from the skeleton
			childBone = new Bone();
			childBone->idx = skeleton->BoneCount();
			childBone->name = childName;
			XMMATRIX transform = ConvertMatrix( childNode->mTransformation );
			XMMATRIX parentOffset = XMLoadFloat4x4( &(bone->offset) );
			XMMATRIX childOffset = XMMatrixMultiply( transform, parentOffset );
			XMStoreFloat4x4( &(childBone->offset), childOffset );
			skeleton->AddBone( childBone );
		}
		bone->children.push_back( childBone );
		FindBoneChildren( childNode, bone->idx );
	}
}

void ModelLoader::CreateAnimations() {
	for( int i = 0; i<scene->mNumAnimations; ++i ) {
		aiAnimation* aiAnim = scene->mAnimations[i];
		Anim* anim = new Anim();
		anim->name = aiAnim->mName.C_Str();
		float framesPerSec = (float)aiAnim->mTicksPerSecond;
		anim->totalTime = (float)(aiAnim->mDuration/aiAnim->mTicksPerSecond);
		float timePerFrame = 1/framesPerSec;
		for( int j = 0; j<aiAnim->mNumChannels; ++j ) {
			aiNodeAnim* aiNodeAnim = aiAnim->mChannels[j];
			Bone* bone = skeleton->GetBoneByName( aiNodeAnim->mNodeName.C_Str() );
			anim->boneSet.insert( bone );

			keySet_t rotKeySet;
			for( int k = 0; k<aiNodeAnim->mNumRotationKeys; ++k ) {
				aiQuatKey quatKey = aiNodeAnim->mRotationKeys[k];
				rotKeySet[(float)quatKey.mTime * timePerFrame] = XMFLOAT4( quatKey.mValue.x, quatKey.mValue.y, quatKey.mValue.z, quatKey.mValue.w );
			}
			anim->rotChannels[bone] = rotKeySet;

			keySet_t posKeySet;
			for( int m = 0; m<aiNodeAnim->mNumPositionKeys; ++m ) {
				aiVectorKey posKey = aiNodeAnim->mPositionKeys[m];
				posKeySet[(float)posKey.mTime * timePerFrame] = XMFLOAT4( posKey.mValue.x, posKey.mValue.y, posKey.mValue.z, 1.0f );
			}
			anim->posChannels[bone] = posKeySet;

			keySet_t scaleKeySet;
			for( int n = 0; n<aiNodeAnim->mNumScalingKeys; ++n ) {
				aiVectorKey scaleKey = aiNodeAnim->mScalingKeys[n];
				scaleKeySet[(float)scaleKey.mTime* timePerFrame] = XMFLOAT4( scaleKey.mValue.x, scaleKey.mValue.y, scaleKey.mValue.z, 1.0f );
			}
			anim->scaleChannels[bone] = scaleKeySet;
		}
		animationController->AddAnim( anim );
	}
}

void ModelLoader::UpdateExtents( float x, float y, float z ) {
	if( x<minX ) {
		minX = x;
	} else if( x>maxX ) {
		maxX = x;
	}
	if( y<minY ) {
		minY = y;
	} else if( y>maxY ) {
		maxY = y;
	}
	if( z<minZ ) {
		minZ = z;
	} else if( z>maxZ ) {
		maxZ = z;
	}
}

XNA::AxisAlignedBox ModelLoader::GetBoundingBox() {
	XNA::AxisAlignedBox aabb;
	XMFLOAT3 center( minX+(maxX-minX) * 0.5f, minY+(maxY-minY) * 0.5f, minZ+(maxZ-minZ)*0.5f );
	XMFLOAT3 extents( (maxX-minX)*0.5f, (maxY-minY)*0.5f, (maxZ-minZ)*0.5f );
	aabb.Center = center;
	aabb.Extents = extents;
	return aabb;
}

XMMATRIX ModelLoader::ConvertMatrix( aiMatrix4x4 inMat ) {
	return XMLoadFloat4x4( &XMFLOAT4X4(
		inMat.a1, inMat.b1, inMat.c1, inMat.d1,
		inMat.a2, inMat.b2, inMat.c2, inMat.d2,
		inMat.a3, inMat.b3, inMat.c3, inMat.d3,
		inMat.a4, inMat.b4, inMat.c4, inMat.d4 ) );
}

XMMATRIX ModelLoader::ConvertFBXtoDXMatrix( aiMatrix4x4 inMat ) {
	// aiMatrix is transposed
	XMMATRIX xmInMat = XMLoadFloat4x4( &XMFLOAT4X4(
		inMat.a1, inMat.b1, inMat.c1, inMat.d1,
		inMat.a2, inMat.b2, inMat.c2, inMat.d2,
		inMat.a3, inMat.b3, inMat.c3, inMat.d3,
		inMat.a4, inMat.b4, inMat.c4, inMat.d4 ) );
	XMVECTOR translate, scale, rotQuat;
	XMMatrixDecompose( &scale, &rotQuat, &translate, xmInMat );
	scale = scale*100;
	return nullptr;
}