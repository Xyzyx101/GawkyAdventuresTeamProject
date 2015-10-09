
#include "Player.h"
#include "Effects.h"
#include "Camera.h"
#include "TheObjects.h"
#include "Enemies.h"
#include "ModelEnum.h"
#include "Model.h"
#include "ModelLoader.h"

Player::Player( ID3D11Device* device, TextureMgr& texMgr, const std::string& modelFilename, const std::wstring& texturePath, FLOAT x, FLOAT y, FLOAT z ) :
mOPlayerPosition( 0.0f, 2.0f, 0.0f ),
mOPlayerScale( 0.03f, 0.03f, 0.03f ),
mPlayerRotAngle(0.f),
verticalVelocity( 0.f ),
speed( 30.f ),
killEnemy( false ) {
	currCharDirection = XMVectorSet( 0.0f, 3.0f, 0.0f, 0.0f );
	oldCharDirection = XMVectorSet( 0.0f, 3.0f, 0.0f, 0.0f );
	charPosition = XMVectorSet( 0.0f, 3.0f, 0.0f, 0.0f );
	PlayerForward = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	
	mPlayerScale = mOPlayerScale;
	mPlayerPosition = mOPlayerPosition;
	mPlayerRotAngle = mOplayerRotAngle;
	
	XMMATRIX S = XMMatrixScalingFromVector( XMLoadFloat3( &mPlayerScale ) );
	XMMATRIX T = XMMatrixTranslationFromVector( XMLoadFloat3( &mPlayerPosition ) );
	XMMATRIX R = XMMatrixRotationY( mPlayerRotAngle );
	XMMATRIX world = S*R*T;
	XMStoreFloat4x4( &mPlayer.World, world );
}

Player::~Player() {}

bool Player::init( ID3D11Device* device, ModelLoader* loader, TextureMgr& texMgr, const std::wstring& texturePath ) {
	if( !loader->Load( device, "./Models/gawky2_0014.fbx", Vertex::POS_NORMAL_TEX_TAN_SKINNED, mModel, &skeleton, &animController ) ) {
		return false;
	}

	skeleton.SetAnimationController( &animController );
	animController.ChangeAnim( ANIM_NAME::JUMP );

	// New AssImp changes
	mDiffuseSRV = texMgr.CreateTexture( texturePath+L"Gawky2_diffuse_color.png" );
	mMaterial.Ambient = XMFLOAT4( 0.2f, 0.2f, 0.2f, 1.f );
	mMaterial.Diffuse = XMFLOAT4( 0.8f, 0.8f, 0.8f, 1.f );
	mMaterial.Specular = XMFLOAT4( 0.5f, 0.5f, 0.0f, 4.f ); // w = spec pow
	mMaterial.Reflect = XMFLOAT4( 0.f, 0.f, 0.f, 0.f );

	XNA::AxisAlignedBox aabb = loader->GetBoundingBox();
	mPlayerBox.Center = XMFLOAT3( aabb.Center.x*mPlayerScale.x, aabb.Extents.y*mPlayerScale.y, aabb.Extents.z*mPlayerScale.z );

	mPlayerBox.Extents = XMFLOAT3( aabb.Extents.x*mPlayerScale.x, aabb.Extents.y*mPlayerScale.y, aabb.Extents.z*mPlayerScale.z );
	mPlayerBox.collisionType = ctPlayer;

	initFSM();

	return true;
}

void Player::setLevelCollisions( std::vector <XNA::AxisAlignedBox> &thelevelCollisions ) {
	LevelCollisions = thelevelCollisions;
}

void Player::update( float dt, XMVECTOR direction, Enemies* guys, TheObjects* things ) {
	
	XMStoreFloat3( &desiredDirection, direction );
	fsm->Update( dt );

	// Gravity
	mPlayerPosition.y += verticalVelocity * dt;
	verticalVelocity -= 30.f * dt;

	updateCollisions( guys, things );

	if( collisions[ctCollect] ) {
		things->RemovemObjectInstance( hitThing );
	}
	if( collisions[ctEnemy]&&killEnemy ) {
		guys->RemovemObjectInstance( hitEnemy );
	}
	killEnemy = false;
	
	XMMATRIX S = XMMatrixScalingFromVector( XMLoadFloat3( &mPlayerScale ) );
	XMMATRIX T = XMMatrixTranslationFromVector( XMLoadFloat3( &mPlayerPosition ) );
	XMMATRIX R = XMMatrixRotationY( mPlayerRotAngle );
	XMMATRIX world = S*R*T;
	XMStoreFloat4x4( &mPlayer.World, world );

	animController.Interpolate( dt );
}

void Player::drawPlayer( ID3D11DeviceContext* dc, Camera& camera, ID3DX11EffectTechnique* activeTexTech ) {
	XMMATRIX world;
	XMMATRIX worldInvTranspose;
	XMMATRIX worldViewProj;

	XMMATRIX view = camera.View();
	XMMATRIX proj = camera.Proj();
	XMMATRIX viewProj = camera.ViewProj();

	world = XMLoadFloat4x4( &mPlayer.World );
	worldInvTranspose = MathHelper::InverseTranspose( world );
	worldViewProj = world*view*proj;
	Effects::GawkyFX->SetWorld( world );
	Effects::GawkyFX->SetWorldInvTranspose( worldInvTranspose );
	Effects::GawkyFX->SetViewProj( viewProj );
	Effects::GawkyFX->SetWorldViewProj( worldViewProj );

	Effects::GawkyFX->SetTexTransform( XMMatrixScaling( 1.0f, 1.0f, 1.0f ) );

	skeleton.SetRootTransform( mPlayer.World );

	// Assimp Draw
	Effects::GawkyFX->SetMaterial( mMaterial );
	Effects::GawkyFX->SetDiffuseMap( mDiffuseSRV );

	Effects::GawkyFX->SetBoneTransforms( skeleton.GetBoneTransforms(), skeleton.BoneCount() );

	UINT offset = 0;
	UINT stride = mModel.VertexStride();
	ID3D11Buffer* vBuffers[1] = { mModel.VB() };
	ID3D11Buffer* iBuffer = mModel.IB();
	activeTexTech->GetPassByName( "P0" )->Apply( 0, dc );
	dc->IASetVertexBuffers( 0, 1, &vBuffers[0], &stride, &offset );
	dc->IASetIndexBuffer( iBuffer, mModel.IndexFormat(), 0 );
	dc->DrawIndexed( mModel.IndexCount(), 0, 0 );
	activeTexTech->GetPassByName( "P1" )->Apply( 0, dc );
	dc->IASetVertexBuffers( 0, 1, &vBuffers[0], &stride, &offset );
	dc->IASetIndexBuffer( iBuffer, mModel.IndexFormat(), 0 );
	dc->DrawIndexed( mModel.IndexCount(), 0, 0 );
}

void Player::updateCollisions( Enemies* guys, TheObjects* things ) {

	// Reset Collisions
	for( bool& c:collisions ) { c = false; }

	// Reset Object and Enemy Collisions
	int collisionThing = 0;
	int collisionEnemy = 0;

	mPlayerBox.Center = XMFLOAT3( mPlayerPosition.x, mPlayerPosition.y+mPlayerBox.Extents.y, mPlayerPosition.z );
	// Gawky Bounds
	float gawkyRight = mPlayerBox.Center.x+mPlayerBox.Extents.x;
	float gawkyLeft = mPlayerBox.Center.x-mPlayerBox.Extents.x;
	float gawkyForward = mPlayerBox.Center.z+mPlayerBox.Extents.z;
	float gawkyBack = mPlayerBox.Center.z-mPlayerBox.Extents.z;
	float gawkyTop = mPlayerBox.Center.y+mPlayerBox.Extents.y;
	float gawkyBottom = mPlayerBox.Center.y-mPlayerBox.Extents.y;

	for( auto collisionObject:LevelCollisions ) {

		// This is in a while loop for edge cases where I hit an object on an angle and need to move in
		//	two or three axis to avoid collision.
		while( IntersectAxisAlignedBoxAxisAlignedBox( &mPlayerBox, &collisionObject ) ) {

			// Set collision flags
			// Collision with the ctLevel objects is special and handled below 
			if( collisionObject.collisionType!=ctLevel ) {
				if( collisionObject.collisionType==ctStumble||collisionObject.collisionType==ctCollect ) {
					hitThing = collisionThing;
				}
				if( collisionObject.collisionType==ctEnemy ) {
					hitEnemy = collisionEnemy;
				}
				collisions[(int)collisionObject.collisionType] = true;
				break;
			}

			////////  Terrain Response ////////////
			if( collisionObject.collisionType==ctLevel ) {
				// Object Bounds
				float objectRight = collisionObject.Center.x+collisionObject.Extents.x;
				float objectLeft = collisionObject.Center.x-collisionObject.Extents.x;
				float objectForward = collisionObject.Center.z+collisionObject.Extents.z;
				float objectBack = collisionObject.Center.z-collisionObject.Extents.z;
				float objectTop = collisionObject.Center.y+collisionObject.Extents.y;
				float objectBottom = collisionObject.Center.y-collisionObject.Extents.y;

				// Pos and Neg is positive and negative
				float xPos = objectRight-gawkyLeft;
				float xNeg = objectLeft-gawkyRight;
				float yPos = objectTop-gawkyBottom;
				float yNeg = objectBottom-gawkyTop;
				float zPos = objectForward-gawkyBack;
				float zNeg = objectBack-gawkyForward;

				// find overlap
				float xResponse, yResponse, zResponse;
				if( abs( xNeg )<xPos ) {
					xResponse = xNeg;
				} else {
					xResponse = xPos;
				}
				if( abs( yNeg )<yPos ) {
					yResponse = yNeg;
				} else {
					yResponse = yPos;
				}
				if( abs( zNeg )<zPos ) {
					zResponse = zNeg;
				} else {
					zResponse = zPos;
				}

				// The term below -- copysign(epsilon,yResponse) -- just adds a small amount in the diretion that the response is already moving.
				// We don't want to just move tell they are touching we want to move tell they are just touching plus a little bit.
				float epsilon = 0.01;

				// move player in smallest direction to undo the overlap
				if( abs( yResponse )<abs( xResponse )&&abs( yResponse )<abs( zResponse ) ) {
					mPlayerPosition = XMFLOAT3( mPlayerPosition.x, mPlayerPosition.y+yResponse+copysign( epsilon, yResponse ), mPlayerPosition.z );
					if( yResponse>0 ) {
						// I only set the collision flag with ctLevel objects if I moved up so that I can use it to tell if I am standing on the ground.
						collisions[(int)collisionObject.collisionType] = true;
						verticalVelocity = 0.f;
					}
				} else if( abs( xResponse )<abs( zResponse ) ) {
					mPlayerPosition = XMFLOAT3( mPlayerPosition.x+xResponse+copysign( epsilon, xResponse ), mPlayerPosition.y, mPlayerPosition.z );
				} else {
					mPlayerPosition = XMFLOAT3( mPlayerPosition.x, mPlayerPosition.y, mPlayerPosition.z+zResponse+copysign( epsilon, zResponse ) );
				}

				//  Recalculate Gawky Bounds
				mPlayerBox.Center = XMFLOAT3( mPlayerPosition.x, mPlayerPosition.y+mPlayerBox.Extents.y, mPlayerPosition.z );
				gawkyRight = mPlayerBox.Center.x+mPlayerBox.Extents.x;
				gawkyLeft = mPlayerBox.Center.x-mPlayerBox.Extents.x;
				gawkyForward = mPlayerBox.Center.z+mPlayerBox.Extents.z;
				gawkyBack = mPlayerBox.Center.z-mPlayerBox.Extents.z;
				gawkyTop = mPlayerBox.Center.y+mPlayerBox.Extents.y;
				gawkyBottom = mPlayerBox.Center.y-mPlayerBox.Extents.y;
			}
		}

		if( collisionObject.collisionType==ctStumble||collisionObject.collisionType==ctCollect ) {
			collisionThing++;
		}
		if( collisionObject.collisionType==ctEnemy ) {
			collisionEnemy++;
		}
	}
}

void Player::initFSM() {
	fsm = new FSM<Player>( this );
	FSM<Player>::StateData idleStateData;
	idleStateData.Before = &Player::Idle_Before;
	idleStateData.Update = &Player::Idle_Update;
	fsm->AddState( FSM_STATE::STATE_IDLE, idleStateData );

	FSM<Player>::StateData walkStateData;
	walkStateData.Before = &Player::Walk_Before;
	walkStateData.Update = &Player::Walk_Update;
	fsm->AddState( FSM_STATE::STATE_WALK, walkStateData );

	FSM<Player>::StateData jumpStateData;
	jumpStateData.Before = &Player::Jump_Before;
	jumpStateData.Update = &Player::Jump_Update;
	fsm->AddState( FSM_STATE::STATE_JUMP, jumpStateData );

	FSM<Player>::StateData fallStateData;
	fallStateData.Before = &Player::Fall_Before;
	fallStateData.Update = &Player::Fall_Update;
	fsm->AddState( FSM_STATE::STATE_FALL, fallStateData );

	FSM<Player>::StateData tripStateData;
	tripStateData.Before = &Player::Trip_Before;
	tripStateData.Update = &Player::Trip_Update;
	fsm->AddState( FSM_STATE::STATE_TRIP, tripStateData );

	FSM<Player>::StateData dieStateData;
	dieStateData.Before = &Player::Die_Before;
	fsm->AddState( FSM_STATE::STATE_DIE, dieStateData );

	fsm->ChangeState( FSM_STATE::STATE_IDLE );
}

void Player::Idle_Before( float dt ) {
	animController.ChangeAnim( ANIM_NAME::IDLE );
}
void Player::Idle_Update( float dt ) {
	XMVECTOR direction = XMLoadFloat3( &desiredDirection );
	float lengthSq;
	XMStoreFloat( &lengthSq, XMVector3LengthSq( direction ) );
	if( lengthSq>0.1f ) {
		fsm->ChangeState( FSM_STATE::STATE_WALK );
	}
	if( desiredDirection.y>0.5 ) {
		fsm->ChangeState( FSM_STATE::STATE_JUMP );
	}
	if( collisions[ctEnemy]||collisions[ctUnkillable] ) {
		fsm->ChangeState( FSM_STATE::STATE_DIE );
	}
}

void Player::Walk_Before( float dt ) {
	animController.ChangeAnim( ANIM_NAME::WALK );
}
void Player::Walk_Update( float dt ) {

	XMVECTOR direction = XMLoadFloat3( &desiredDirection );
	XMMATRIX worldMatrix = XMLoadFloat4x4( &mPlayer.World );

	float lengthSq;
	XMStoreFloat( &lengthSq, XMVector3LengthSq( direction ) );
	if( lengthSq<0.1f ) {
		fsm->ChangeState( FSM_STATE::STATE_IDLE );
		return;
	}
	if( desiredDirection.y>0.5 ) {
		fsm->ChangeState( FSM_STATE::STATE_JUMP );
	}
	if( verticalVelocity<-1.f ) {
		fsm->ChangeState( FSM_STATE::STATE_FALL );
	}
	if( collisions[ctStumble] ) {
		fsm->ChangeState( FSM_STATE::STATE_TRIP );
	}
	if( collisions[ctEnemy]||collisions[ctUnkillable] ) {
		fsm->ChangeState( FSM_STATE::STATE_DIE );
	}

	// Normalize our destinated direction vector
	direction = XMVector3Normalize( direction );
	direction = XMVectorSetY( direction, 0.0f );

	/////character spinning make it more smooth
	if( XMVectorGetX( XMVector3Dot( direction, oldCharDirection ) )==-1 ) {
		oldCharDirection += XMVectorSet( 0.01f, 0.0f, 0.0f, 0.0f );
	}

	///////get characters position in world space
	charPosition = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	charPosition = XMVector3TransformCoord( charPosition, worldMatrix );

	///// rotate the character
	float destDirLength = 10.0f * dt;

	currCharDirection = (oldCharDirection)+(direction * destDirLength);
	currCharDirection = XMVector3Normalize( currCharDirection );

	oldCharDirection = XMVector3Normalize( currCharDirection );

	// get the angle 
	float charDirAngle = XMVectorGetX( XMVector3AngleBetweenNormals( XMVector3Normalize( currCharDirection ), XMVector3Normalize( PlayerForward ) ) );

	if( XMVectorGetY( XMVector3Cross( currCharDirection, PlayerForward ) )>0.0f ) {
		charDirAngle = -charDirAngle;
	}

	direction = direction * speed * dt;
	charPosition = charPosition+direction;
	mPlayerRotAngle = charDirAngle-XM_PI;

	XMStoreFloat3( &mPlayerPosition, charPosition );
}

void Player::Jump_Before( float dt ) {
	animController.ChangeAnim( ANIM_NAME::JUMP );
	// 0.01f in the y is required because I don't want the jump controls to control the direction but normalizing (0,0,0) will break;
	XMVECTOR jumpDir = XMLoadFloat3( &XMFLOAT3( desiredDirection.x, 0.01f, desiredDirection.z ) );
	jumpDir = XMVector3NormalizeEst( jumpDir );
	jumpDir = jumpDir*speed;
	XMStoreFloat3( &jumpDirection, jumpDir );
	jumpDirection.y = 0.f;
	verticalVelocity = 25.f;
}
void Player::Jump_Update( float dt ) {
	XMVECTOR pos = XMLoadFloat3( &mPlayerPosition );
	XMVECTOR dir = XMLoadFloat3( &jumpDirection );
	pos += dir * dt;
	XMStoreFloat3( &mPlayerPosition, pos );
	if( verticalVelocity<0.f ) {
		fsm->ChangeState( FSM_STATE::STATE_FALL );
	}
	if( collisions[ctEnemy]||collisions[ctUnkillable] ) {
		fsm->ChangeState( FSM_STATE::STATE_DIE );
	}
}

void Player::Fall_Before( float dt ) {
	animController.ChangeAnim( ANIM_NAME::FALL );
}
void Player::Fall_Update( float dt ) {
	XMVECTOR pos = XMLoadFloat3( &mPlayerPosition );
	XMVECTOR dir = XMLoadFloat3( &XMFLOAT3( desiredDirection.x, 0.01f, desiredDirection.z ) );
	dir = XMVector3Normalize( dir );
	dir *= speed * 0.5f;
	dir = XMVectorSetY( dir, 0.f );
	pos += dir * dt;
	XMStoreFloat3( &mPlayerPosition, pos );
	if( collisions[ctLevel] ) {
		fsm->ChangeState( FSM_STATE::STATE_IDLE );
	}
	if( collisions[ctEnemy]||collisions[ctUnkillable] ) {
		fsm->ChangeState( FSM_STATE::STATE_DIE );
	}
}

void Player::Trip_Before( float dt ) {
	animController.ChangeAnim( ANIM_NAME::TRIP );
	// 0.01f in the y is required because I don't want the jump controls to control the direction but normalizing (0,0,0) will break;
	XMVECTOR tripDir = XMLoadFloat3( &XMFLOAT3( desiredDirection.x, 0.01f, desiredDirection.z ) );
	tripDir = XMVector3NormalizeEst( tripDir );
	tripDir = tripDir*speed*0.6;
	XMStoreFloat3( &tripDirection, tripDir );
	tripDirection.y = 0.f;
	tripTimer = 3.f;
}
void Player::Trip_Update( float dt ) {
	XMVECTOR pos = XMLoadFloat3( &mPlayerPosition );
	XMFLOAT3 finalTripDirection = XMFLOAT3( tripDirection.x*(tripTimer/2.5f), 0.f, tripDirection.z*(tripTimer/2.5f) );
	XMVECTOR dir = XMLoadFloat3( &finalTripDirection );
	pos += dir * dt;
	XMStoreFloat3( &mPlayerPosition, pos );
	tripTimer -= dt;
	if( tripTimer<0.f ) {
		fsm->ChangeState( FSM_STATE::STATE_IDLE );
	}
	if( collisions[ctEnemy] ) {
		killEnemy = true;
	}
	if( collisions[ctUnkillable] ) {
		fsm->ChangeState( FSM_STATE::STATE_DIE );
	}
}

void Player::Die_Before( float dt ) {
	mPlayerPosition = mOPlayerPosition;
	mPlayerScale = mOPlayerScale;
	mPlayerRotAngle = 0.f;
}
void Player::Die_Update( float dt ) {
	fsm->ChangeState( FSM_STATE::STATE_IDLE );
}

////getters
XMFLOAT3 Player::getPlayerPosition() {
	return mPlayerPosition;
}

////setters
void Player::setMoveDirection( XMVECTOR mDirection ) {
	moveDirection = mDirection;
}

void Player::setJump(bool newset) {
	isJump = newset;
}
