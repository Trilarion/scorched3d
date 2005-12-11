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

#include <actions/ShotProjectile.h>
#include <sprites/MissileActionRenderer.h>
#include <tankgraph/TracerRenderer.h>
#include <common/OptionsParam.h>
#include <common/Defines.h>
#include <engine/ScorchedContext.h>
#include <engine/ViewPoints.h>
#include <weapons/AccessoryStore.h>
#include <math.h>

REGISTER_ACTION_SOURCE(ShotProjectile);

Vector ShotProjectile::lookatPosition_;
unsigned int ShotProjectile::lookatCount_ = 0;

ShotProjectile::ShotProjectile() : 
	collisionInfo_(CollisionIdShot), vPoint_(0), 
	snapTime_(0.2f),  up_(false), landedCounter_(0), data_(0),
	totalTime_(0.0)
{

}

ShotProjectile::ShotProjectile(Vector &startPosition, Vector &velocity,
							   WeaponProjectile *weapon, unsigned int playerId,
							   unsigned int flareType,
							   unsigned int data) :
	collisionInfo_(CollisionIdShot), 
	startPosition_(startPosition), velocity_(velocity), 
	weapon_(weapon), playerId_(playerId), 
	flareType_(flareType), vPoint_(0),
	snapTime_(0.2f), up_(false),
	landedCounter_(0), data_(data),
	totalTime_(0.0)
{

}

void ShotProjectile::init()
{
	if (!context_->serverMode) 
	{
		setActionRender(new MissileActionRenderer(flareType_, weapon_->getScale()));
	}

	vPoint_ = context_->viewPoints->getNewViewPoint(playerId_);
	setPhysics(startPosition_, velocity_);
	collisionInfo_.data = this;
	collisionInfo_.collisionOnSurface = !getWeapon()->getUnder();
	physicsObject_.setData(&collisionInfo_);
}

ShotProjectile::~ShotProjectile()
{
	if (vPoint_) context_->viewPoints->releaseViewPoint(vPoint_);
}

void ShotProjectile::collision(Vector &position)
{
	// Apex collisions dud if they collide with the ground
	// unless no dud is set
	if (!getWeapon()->getApexCollision() ||
		getWeapon()->getApexNoDud()) 
	{
		doCollision(position);
	}
	PhysicsParticleMeta::collision(position);
}

void ShotProjectile::simulate(float frameTime, bool &remove)
{
	totalTime_ += frameTime;
	if (vPoint_)
	{
		vPoint_->setPosition(getCurrentPosition());

		Vector velocity = -getCurrentVelocity();
		velocity[2] = 10.0f;
		vPoint_->setLookFrom(velocity);
	}

	// Apex collision
	if (getWeapon()->getApexCollision())
	{
		if (getCurrentVelocity()[2] > 0.0f) up_ = true;
		else if (up_)
		{
			doCollision(getCurrentPosition());
			remove = true;
		}
	}

	// Timed collision
	if (getWeapon()->getTimedCollision() > 0.0f)
	{
		if (totalTime_ > getWeapon()->getTimedCollision())
		{
			doCollision(getCurrentPosition());
			remove = true;
		}
	}

	// Shot path
	if (getWeapon()->getShowShotPath())
	{
		snapTime_ += frameTime;
		if (snapTime_ > 0.1f || remove)
		{
			Vector up (0.0f, 0.0f, 1.0f);
			TracerRenderer::TracerLinePoint point;
			point.position = getCurrentPosition();
			point.cross = (getCurrentVelocity() * up).Normalize();
			positions_.push_back(point);
			snapTime_ = 0.0f;
		}
	}

	PhysicsParticleMeta::simulate(frameTime, remove);
}

bool ShotProjectile::writeAction(NetBuffer &buffer)
{
	buffer.addToBuffer(startPosition_);
	buffer.addToBuffer(velocity_);
	context_->accessoryStore->writeWeapon(buffer, weapon_);
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(flareType_);
	buffer.addToBuffer(data_);
	return true;
}

bool ShotProjectile::readAction(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(startPosition_)) return false;
	if (!reader.getFromBuffer(velocity_)) return false;
	weapon_ = (WeaponProjectile *) context_->accessoryStore->readWeapon(reader); if (!weapon_) return false;
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(flareType_)) return false;
	if (!reader.getFromBuffer(data_)) return false;
	return true;
}

void ShotProjectile::doCollision(Vector &position)
{	
	if (!context_->serverMode)
	{
		if (getWeapon()->getShowShotPath())
		{
			TracerRenderer::instance()->
				addSmokeTracer(playerId_, position, positions_);
		}
		else if (getWeapon()->getShowEndPoint())
		{
			TracerRenderer::instance()->
				addTracer(playerId_, position);
		}
	}

	Vector velocity;
	getWeapon()->getCollisionAction()->fireWeapon(
		*context_,
		playerId_, position, getCurrentVelocity(), data_);
}

