#include "Skeleton.h"
#include "AnimationController.h"

Skeleton::Skeleton() :
numBones( 0 ) {}

Skeleton::~Skeleton() {}

void  Skeleton::SetRootTransform( XMFLOAT4X4& transform ) {
	rootTransform = &transform;
}

void  Skeleton::SetAnimationController( AnimationController* animController ) {
	animationController = animController;
}

void Skeleton::AddBone( Bone* newBone ) {
	++numBones;
	idxBones.insert( std::pair<int, Bone*>( newBone->idx, newBone ) );
	nameBones.insert( std::pair<std::string, Bone*>( newBone->name, newBone ) );
}

int Skeleton::BoneCount() {
	return numBones;
}

Bone* Skeleton::GetBoneByName( std::string name ) {
	auto nameIt = nameBones.find( name );
	if( nameIt==nameBones.end() ) {
		return nullptr;
	}
	return nameIt->second;
}

Bone* Skeleton::GetBoneByIndex( int index ) {
	auto indexIt = idxBones.find( index );
	if( indexIt==idxBones.end() ) {
		return nullptr;
	}
	return indexIt->second;
}

XMFLOAT4X4* Skeleton::GetFinalTransforms() {
	for( auto it:nameBones ) {
		Bone* bone = it.second;
		bone->localTransform = animationController->GetBoneTransform( bone );
	}
	Bone* root = GetBoneByName( "Skeleton_Root" );
	UpdateTransforms( root );
	return finalTransformData;
}

void Skeleton::UpdateTransforms( Bone* bone ) {
	XMMATRIX localTransform = XMLoadFloat4x4( &(bone->localTransform) );
	XMMATRIX offset = XMLoadFloat4x4( &(bone->offset) );
	XMMATRIX finalTransform;
	if( bone->parentIdx = -1 ) {
		// The root bone
		XMStoreFloat4x4( &(toRoot[bone->idx]), localTransform );
		finalTransform = offset*localTransform;
	} else {
		XMMATRIX parentToRoot = XMLoadFloat4x4( &(toRoot[bone->parentIdx]) );
		XMMATRIX boneToRoot = localTransform*parentToRoot;
		XMStoreFloat4x4( &(toRoot[bone->idx]), boneToRoot );
		finalTransform = offset*boneToRoot;
	}
	XMStoreFloat4x4( &(finalTransformData[bone->idx]), finalTransform );
	if( bone->children.size()==0 ) { return; }
	for( Bone* childBone:bone->children ) {
		UpdateTransforms( childBone );
	}
}