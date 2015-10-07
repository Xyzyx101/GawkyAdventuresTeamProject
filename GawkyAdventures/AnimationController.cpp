#include "AnimationController.h"
#include "Skeleton.h"

AnimationController::AnimationController() {}

AnimationController::~AnimationController() {}

void AnimationController::AddAnim( Anim* anim ) {
	std::string name = anim->name;
	if( name=="Test48" ) {
		anims[ANIM_NAME::TEST] = anim;
	} else if( name=="Idle" ) {
		anims[ANIM_NAME::IDLE] = anim;
	} else if( name=="Walk" ) {
		anims[ANIM_NAME::WALK] = anim;
	} else if( name=="Jump" ) {
		anims[ANIM_NAME::JUMP] = anim;
	} else if( name=="Fall" ) {
		anims[ANIM_NAME::FALL] = anim;
	} else if( name=="Trip" ) {
		anims[ANIM_NAME::TRIP] = anim;
	} else {
		fprintf( stderr, "Bad Animation name : %s", name );
	}
}

void AnimationController::ChangeAnim( ANIM_NAME newAnim ) {
	if( currentAnim==newAnim ) {
		return;
	}
	timeSinceStart = 0.f;
	currentAnim = newAnim;
}

void AnimationController::Interpolate( float dt ) {
	timeSinceStart += dt;
	Anim* anim = anims[currentAnim];
	float animCurrentTime = fmod( timeSinceStart, anim->totalTime );
	for( Bone* bone:anim->boneSet ) {
		XMMATRIX rotMat, scaleMat, translateMat;

		// Interpolate Rotation
		auto rotSetIt = anim->rotChannels.find( bone );
		if( rotSetIt==anim->rotChannels.end() ) {
			rotMat = XMMatrixIdentity();
		} else {
			if( rotSetIt->second.keyTime.size()==1 ) {
				XMVECTOR quat = XMLoadFloat4( &(rotSetIt->second.value[0]) );
				rotMat = XMMatrixRotationQuaternion( quat );
			} else {
				int lowIdx = 1, highIdx = 1;
				KeySet rotKeySet = rotSetIt->second;
				auto itLow = rotKeySet.keyTime.cbegin()+1;
				while( *itLow<animCurrentTime ) {
					++itLow; ++lowIdx;
					if( itLow==rotKeySet.keyTime.cend() ) {
						break;
					}
				}
				if( lowIdx==rotKeySet.keyTime.size() ) {
					--lowIdx;
					rotMat = XMMatrixRotationQuaternion( XMLoadFloat4( &rotKeySet.value[lowIdx] ) );
				} else {
					auto itHigh = itLow; highIdx = lowIdx;
					--itLow; --lowIdx;
					while( *itHigh<animCurrentTime ) {
						++itHigh; ++highIdx;
						if( itLow==rotKeySet.keyTime.cend() ) {
							break;
						}
					}
					float factor;
					if( itLow==itHigh ) {
						factor = 0.f;
					} else {
						factor = (animCurrentTime-*itLow)/(*itHigh-*itLow);
					}
					XMVECTOR low = XMLoadFloat4( &rotKeySet.value[lowIdx] );
					XMVECTOR high = XMLoadFloat4( &rotKeySet.value[highIdx] );
					XMVECTOR interp = XMQuaternionSlerp( low, high, factor );
					XMVECTOR normalized = XMQuaternionNormalize( interp );
					rotMat = XMMatrixRotationQuaternion( interp );
				}
			}
		}

		// Interpolate Scale
		auto scaleSetIt = anim->scaleChannels.find( bone );
		if( scaleSetIt==anim->scaleChannels.end() ) {
			scaleMat = XMMatrixIdentity();
		} else {
			if( scaleSetIt->second.keyTime.size()==1 ) {
				XMVECTOR scale = XMLoadFloat4( &(scaleSetIt->second.value[0]) );
				scaleMat = XMMatrixScalingFromVector( scale );
			} else {
				int lowIdx = 1, highIdx = 1;
				KeySet scaleKeySet = scaleSetIt->second;
				auto itLow = scaleKeySet.keyTime.cbegin()+1;
				while( *itLow<animCurrentTime ) {
					++itLow; ++lowIdx;
					if( itLow==scaleKeySet.keyTime.cend() ) {
						break;
					}
				}
				if( lowIdx==scaleKeySet.keyTime.size() ) {
					--lowIdx;
					XMFLOAT4 lowVec = scaleKeySet.value[lowIdx];
					scaleMat = XMMatrixScaling( lowVec.x, lowVec.y, lowVec.z );
				} else {
					auto itHigh = itLow; highIdx = lowIdx;
					--itLow; --lowIdx;
					while( *itHigh<animCurrentTime ) {
						++itHigh; ++highIdx;
						if( itLow==scaleKeySet.keyTime.cend() ) {
							break;
						}
					}
					float factor;
					if( itLow==itHigh ) {
						factor = 0.f;
					} else {
						factor = (animCurrentTime-*itLow)/(*itHigh-*itLow);
					}
					XMFLOAT4 lowVec = scaleKeySet.value[lowIdx];
					XMFLOAT4 highVec = scaleKeySet.value[highIdx];
					scaleMat = XMMatrixScaling(
						lowVec.x+factor*(highVec.x-lowVec.x),
						lowVec.y+factor*(highVec.y-lowVec.y),
						lowVec.z+factor*(highVec.z-lowVec.z) );
				}
			}
		}

		// Interpolate Position
		auto posSetIt = anim->posChannels.find( bone );
		if( posSetIt==anim->posChannels.end() ) {
			translateMat = XMMatrixIdentity();
		} else {
			if( posSetIt->second.keyTime.size()==1 ) {
				XMVECTOR pos = XMLoadFloat4( &(posSetIt->second.value[0]) );
				translateMat = XMMatrixTranslationFromVector( pos );
			} else {
				int lowIdx = 1, highIdx = 1;
				KeySet posKeySet = posSetIt->second;
				auto itLow = posKeySet.keyTime.cbegin()+1;
				while( *itLow<animCurrentTime ) {
					++itLow; ++lowIdx;
					if( itLow==posKeySet.keyTime.cend() ) {
						break;
					}
				}
				if( lowIdx==posKeySet.keyTime.size() ) {
					--lowIdx;
					XMFLOAT4 lowVec = posKeySet.value[lowIdx];
					translateMat = XMMatrixTranslation( lowVec.x, lowVec.y, lowVec.z );
				} else {
					auto itHigh = itLow; highIdx = lowIdx;
					--itLow; --lowIdx;
					while( *itHigh<animCurrentTime ) {
						++itHigh; ++highIdx;
						if( itLow==posKeySet.keyTime.cend() ) {
							break;
						}
					}
					float factor;
					if( itLow==itHigh ) {
						factor = 0.f;
					} else {
						factor = (animCurrentTime-*itLow)/(*itHigh-*itLow);
					}
					XMFLOAT4 lowVec = posKeySet.value[lowIdx];
					XMFLOAT4 highVec = posKeySet.value[highIdx];
					translateMat = XMMatrixTranslation(
						lowVec.x+factor*(highVec.x-lowVec.x),
						lowVec.y+factor*(highVec.y-lowVec.y),
						lowVec.z+factor*(highVec.z-lowVec.z) );
				}
			}
		}
		XMMATRIX finalMat = scaleMat * rotMat * translateMat;
		XMFLOAT4X4 transform;
		XMStoreFloat4x4( &transform, finalMat );
		boneTransforms[bone] = transform;
	}
}

XMFLOAT4X4 AnimationController::GetBoneTransform( Bone* bone ) {
	auto it = boneTransforms.find( bone );
	if( it==boneTransforms.end() ) {
		return XMFLOAT4X4(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f );
	}
	return it->second;
}