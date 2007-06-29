////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2003
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with Scorched3D; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

#include <actions/ShotBounce.h>
#include <actions/CameraPositionAction.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/WeaponRoller.h>
#include <weapons/AccessoryStore.h>
#ifndef S3D_SERVER
	#include <GLEXT/GLState.h>
#endif
#include <3dsparse/ModelStore.h>
#include <3dsparse/Model.h>
#include <graph/ModelRenderer.h>
#include <graph/ModelRendererStore.h>
#include <graph/ModelRendererSimulator.h>
#include <string.h>

ShotBounce::ShotBounce(WeaponRoller *weapon, 
		Vector &startPosition, Vector &velocity,
		WeaponFireContext &weaponContext) : 
	startPosition_(startPosition),
	velocity_(velocity), weapon_(weapon), weaponContext_(weaponContext),
	totalTime_(0.0f), 
	vPoint_(0), model_(0)
{
}

void ShotBounce::init()
{
	PhysicsParticleInfo info(ParticleTypeBounce, weaponContext_.getPlayerId(), this);
	setPhysics(info, startPosition_, velocity_, 
		1.0f, 5.0f, weapon_->getWindFactor(*context_), false, weapon_->getRoll());

	Vector lookatPos;
	vPoint_ = context_->viewPoints->getNewViewPoint(weaponContext_.getPlayerId());
	context_->viewPoints->getValues(lookatPos, lookFrom_);

	// Point the action camera at this event
	const float ShowTime = 5.0f;
#ifndef S3D_SERVER
        if (!context_->serverMode)
	{
		CameraPositionAction *pos = new CameraPositionAction(
			startPosition_, ShowTime,
			5);
		context_->actionController->addAction(pos);
	}
#endif

}

ShotBounce::~ShotBounce()
{
	delete model_;
	if (vPoint_) context_->viewPoints->releaseViewPoint(vPoint_);
}

void ShotBounce::collision(PhysicsParticleObject &position, 
	ScorchedCollisionId collisionId)
{
	if (!collision_)
	{
		doCollision();
	}
	PhysicsParticleReferenced::collision(position, collisionId);
}

void ShotBounce::simulate(float frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (totalTime_ > weapon_->getTime(*context_))
	{
		doCollision();
		remove = true;
	}

	PhysicsParticleReferenced::simulate(frameTime, remove);
}

void ShotBounce::draw()
{
#ifndef S3D_SERVER
	if (!context_->serverMode) 
	{
		static float rotMatrix[16];
		getRotationQuat().getOpenGLRotationMatrix(rotMatrix);

		if (!model_)
		{
			ModelID &id = ((WeaponRoller *) weapon_)->getRollerModelID();
			bool useTexture = (strcmp(id.getSkinName(), "none") != 0);
			model_ = new ModelRendererSimulator(
				ModelRendererStore::instance()->loadModel(id));
		}

		if (vPoint_)
		{
			vPoint_->setPosition(getCurrentPosition());
			vPoint_->setLookFrom(lookFrom_);
		}

		GLState state(GLState::TEXTURE_OFF);
		glPushMatrix();
			glTranslatef(
				getCurrentPosition()[0], 
				getCurrentPosition()[1], 
				getCurrentPosition()[2] -
				model_->getRenderer()->getModel()->getMin()[2] * 0.08f);

			glMultMatrixf(rotMatrix);
			glScalef(0.08f, 0.08f, 0.08f);
			model_->draw();
		glPopMatrix();
	}
#endif // #ifndef S3D_SERVER
}

void ShotBounce::doCollision()
{
	WeaponRoller *proj = (WeaponRoller *) weapon_;
	proj->getCollisionAction()->fireWeapon(
		*context_, weaponContext_, getCurrentPosition(), getCurrentVelocity());
}
