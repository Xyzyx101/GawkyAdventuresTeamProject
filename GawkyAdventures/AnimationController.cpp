#include "AnimationController.h"
#include "Skeleton.h"

AnimationController::AnimationController() {}

AnimationController::~AnimationController() {}

void AnimationController::AddAnim( Anim* anim ) {
	std::string name = anim->name;
	if( name=="Test" ) {
		anims[ANIM_TEST] = anim;
	} else if( name=="Idle" ) {
		anims[ANIM_IDLE] = anim;
	} else if( name=="Walk" ) {
		anims[ANIM_WALK] = anim;
	} else if( name=="Attack" ) {
		anims[ANIM_ATTACK] = anim;
	} else if( name=="Jump" ) {
		anims[ANIM_JUMP] = anim;
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

}