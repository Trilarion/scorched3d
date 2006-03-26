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

#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <actions/ShotProjectile.h>
#include <actions/ShotBounce.h>
#include <actions/WallHit.h>
#include <actions/ShieldHit.h>
#include <actions/TankDamage.h>
#include <landscape/LandscapeMaps.h>
#include <engine/ScorchedCollisionHandler.h>
#include <engine/PhysicsParticle.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <weapons/ShieldMag.h>
#include <weapons/ShieldReflective.h>
#include <weapons/Accessory.h>
#include <tank/TankContainer.h>

ScorchedCollisionHandler::ScorchedCollisionHandler(ScorchedContext *context) :
	context_(context)
{

}

ScorchedCollisionHandler::~ScorchedCollisionHandler()
{
}

void ScorchedCollisionHandler::collision(dGeomID o1, dGeomID o2, 
		dContactGeom *contacts, int noContacts)
{
	// Check what we have collided with
	ScorchedCollisionInfo *info1 = 
		(ScorchedCollisionInfo *) dGeomGetData(o1);
	ScorchedCollisionInfo *info2 = 
		(ScorchedCollisionInfo *) dGeomGetData(o2);
	DIALOG_ASSERT(info1 && info2);

	if (info1->id == CollisionIdSprite || info2->id == CollisionIdSprite)
	{
		// The sprite has hit something
		groundCollision(o1, o2, contacts, noContacts, false);
	}
	else
	if (info1->id == CollisionIdFallingTank || info2->id == CollisionIdFallingTank)
	{
		// The falling tank has hit something
		groundCollision(o1, o2, contacts, noContacts, false);
	}
	else
	if (info1->id == CollisionIdBounce || info2->id == CollisionIdBounce)
	{
		// We have a bouncy particle collision
		bounceCollision(o1, o2, contacts, noContacts);
	}
	else
	if (info1->id == CollisionIdShot || info2->id == CollisionIdShot)
	{
		// We have a particle collision
		shotCollision(o1, o2, contacts, noContacts);
	}
	else 
	if (info1->id == CollisionIdTarget && info2->id == CollisionIdTarget)
	{
		// Target and target collision
		targetCollision(info1, info2, contacts, noContacts);
	}
}

void ScorchedCollisionHandler::targetCollision(
	ScorchedCollisionInfo *info1, ScorchedCollisionInfo *info2, 
	dContactGeom *contacts, int noContacts)
{
	unsigned int playerId1 = (unsigned int) info1->data;
	unsigned int playerId2 = (unsigned int) info2->data;
	Target *target1 = context_->targetContainer->getTargetById(playerId1);
	Target *target2 = context_->targetContainer->getTargetById(playerId2);
	if (!target1 || !target2) return;

	//Dummy weapon, should'nt be used anywhere
	Weapon *weapon = (Weapon *) 
		context_->accessoryStore->findByAccessoryType(
			AccessoryPart::AccessoryWeapon)->getAction();
	if (target1->isTarget() && !target2->isTarget())
	{
		context_->actionController->addAction(
			new TankDamage(weapon, playerId1, playerId2, target1->getLife().getLife(),
				false, false, false, 0));
	}
	else if (!target1->isTarget() && target2->isTarget())
	{
		context_->actionController->addAction(
			new TankDamage(weapon, playerId2, playerId1, target2->getLife().getLife(),
				false, false, false, 0));
	}
}

void ScorchedCollisionHandler::bounceCollision(dGeomID o1, dGeomID o2, 
		dContactGeom *contacts, int noContacts)
{
	const dReal *bouncePosition = dGeomGetPosition(o1);
	ScorchedCollisionInfo *bounceInfo = 
		(ScorchedCollisionInfo *) dGeomGetData(o1);
	ScorchedCollisionInfo *otherInfo = 
		(ScorchedCollisionInfo *) dGeomGetData(o2);
	if (otherInfo->id == CollisionIdBounce)
	{
		bouncePosition = dGeomGetPosition(o2);
		bounceInfo = (ScorchedCollisionInfo *) dGeomGetData(o2);
		otherInfo = (ScorchedCollisionInfo *) dGeomGetData(o1);
	}

	ShotBounce *particle = (ShotBounce *) bounceInfo->data;
	unsigned int id = (unsigned int) otherInfo->data;

	// only collide with the ground, walls or landscape,
	// or iteself
	switch (otherInfo->id)
	{
	case CollisionIdGround:
	case CollisionIdLandscape:
	case CollisionIdRoof:
		collisionBounce(o1, o2, contacts, noContacts, 10000.0, 1.0f);
		break;
	case CollisionIdBounce:
		//collisionBounce(o1, o2, contacts, noContacts);
		break;
	case CollisionIdWallRight:
	case CollisionIdWallTop:
	case CollisionIdWallBottom:
	case CollisionIdWallLeft:
	case CollisionIdTarget:
		{
			Vector bouncePositionV(
				(float) bouncePosition[0],
				(float) bouncePosition[1],
				(float) bouncePosition[2]);
			particle->collision(bouncePositionV);
		}
		break;
	case CollisionIdShield:
		{
			Vector bouncePositionV(
				(float) bouncePosition[0],
				(float) bouncePosition[1],
				(float) bouncePosition[2]);

			float deflectPower = 1.0f;
			ParticleAction action = collisionShield(
				particle->getPlayerId(),
				id, 
				bouncePositionV, 
				particle,
				particle->getWeapon()->getShieldHurtFactor(),
				deflectPower);

			// Unless there is no shield, we bounce off all shields
			if (action != ParticleActionNone)
			{
				collisionBounce(o1, o2, contacts, noContacts, deflectPower);
			}
		}
		break;
	default:
		break;
	}
}

void ScorchedCollisionHandler::groundCollision(dGeomID o1, dGeomID o2, 
		dContactGeom *contacts, int noContacts, bool metaAction)
{
	const dReal *tankPosition = dGeomGetPosition(o1);
	ScorchedCollisionInfo *tankInfo = 
		(ScorchedCollisionInfo *) dGeomGetData(o1);
	ScorchedCollisionInfo *otherInfo = 
		(ScorchedCollisionInfo *) dGeomGetData(o2);
	if (otherInfo->id == CollisionIdFallingTank)
	{
		tankPosition = dGeomGetPosition(o2);
		tankInfo = (ScorchedCollisionInfo *) dGeomGetData(o2);
		otherInfo = (ScorchedCollisionInfo *) dGeomGetData(o1);
	}

	// only collide with the ground, walls or landscape
	if (otherInfo->id == CollisionIdWallLeft ||
		otherInfo->id == CollisionIdWallRight ||
		otherInfo->id == CollisionIdWallTop ||
		otherInfo->id == CollisionIdWallBottom)
	{
		// Always bounce off the side walls,
		// This will stop tanks hanging off the side
		collisionBounce(o1, o2, contacts, noContacts, 1.0);
	}
	else if (otherInfo->id == CollisionIdGround ||
		otherInfo->id == CollisionIdLandscape)
	{
		Vector tankPositionV(
			(float) tankPosition[0],
			(float) tankPosition[1],
			(float) tankPosition[2]);

		if (metaAction)
		{
			PhysicsParticleMeta *particle = (PhysicsParticleMeta *) tankInfo->data;
			particle->collision(tankPositionV);
		}
		else
		{
			PhysicsParticle *particle = (PhysicsParticle *) tankInfo->data;
			particle->collision(tankPositionV);
		}
	}
}

void ScorchedCollisionHandler::shotCollision(dGeomID o1, dGeomID o2, 
		dContactGeom *contacts, int noContacts)
{
	float deflectPower = 1.0f;
	ScorchedCollisionType collisionType = CollisionNone;
	const dReal *particlePosition = dGeomGetPosition(o1);
	ScorchedCollisionInfo *particleInfo = 
		(ScorchedCollisionInfo *) dGeomGetData(o1);
	ScorchedCollisionInfo *otherInfo = 
		(ScorchedCollisionInfo *) dGeomGetData(o2);

	DIALOG_ASSERT(particleInfo && otherInfo);

	if (otherInfo->id == CollisionIdShot)
	{
		particlePosition = dGeomGetPosition(o2);
		particleInfo = (ScorchedCollisionInfo *) dGeomGetData(o2);
		otherInfo = (ScorchedCollisionInfo *) dGeomGetData(o1);
	}

	unsigned int id = (unsigned int) otherInfo->data;
	ShotProjectile *shot = (ShotProjectile *) particleInfo->data;
	shot->incLandedCounter();
	Vector particlePositionV(
		(float) particlePosition[0],
		(float) particlePosition[1],
		(float) particlePosition[2]);

	ParticleAction action = ParticleActionNone;
	switch (otherInfo->id)
	{
	case CollisionIdShot:
		// Two shots collide
		action = ParticleActionNone;
		break;
	case CollisionIdShield:
		// May have collided with shield
		{
			// Only collide with other peoples shields
			action = collisionShield(
				shot->getPlayerId(),
				id, 
				particlePositionV, 
				shot,
				shot->getWeapon()->getShieldHurtFactor(),
				deflectPower);
			if (action != ParticleActionNone)
			{
				collisionType = CollisionShield;
			}
		}
		break;
	case CollisionIdTarget:
		// A shot collides with a tank
		{
			// Only collide with other tanks
			if (shot->getPlayerId() != id)
			{
				// Make sure tank we are colliding with is alive
				Target *target = context_->targetContainer->getTargetById(id);
				if (target &&
					target->getAlive())
				{
					action = ParticleActionFinished;
					collisionType = CollisionFinished;
				}
			}
			else
			{
				action = ParticleActionNone;
			}
		}
		break;
	case CollisionIdWallLeft:
	case CollisionIdWallRight:
	case CollisionIdWallTop:
	case CollisionIdWallBottom:
		// A shot collides with a wall
		{
			OptionsTransient::WallSide wallSide = (OptionsTransient::WallSide)
				(otherInfo->id - CollisionIdWallLeft);

			// Choose what to do depending on the wall
			switch(context_->optionsTransient->getWallType())
			{
			case OptionsTransient::wallBouncy:
				action = ParticleActionBounce;

				context_->actionController->addAction(
					new WallHit(particlePositionV, wallSide));
				collisionType = CollisionWall;
				break;
			case OptionsTransient::wallWrapAround:
				{
					int mapWidth = context_->landscapeMaps->getGroundMaps().getMapWidth();
					int mapHeight = context_->landscapeMaps->getGroundMaps().getMapHeight();
					Vector currentPosition = shot->getCurrentPosition();

					context_->actionController->addAction(
						new WallHit(particlePositionV, wallSide));
					collisionType = CollisionWall;
					action = ParticleActionNone; // Not strictly true as
					// we are about to move the particle
					switch (otherInfo->id)
					{
					case CollisionIdWallLeft:
						currentPosition[0] = float(mapWidth - 10);
						break;
					case CollisionIdWallRight:
						currentPosition[0] = 10.0f;
						break;
					case CollisionIdWallTop:
						currentPosition[1] = float(mapHeight - 10);
						break;
					case CollisionIdWallBottom:
						currentPosition[1] = 10.0f;
						break;
					default:
						break;
					}
					shot->setCurrentPosition(currentPosition);
					action = ParticalActionWarp;
				}
				break;
			case OptionsTransient::wallNone:
				action = ParticleActionNone;
				break;
			default:
				context_->actionController->addAction(
					new WallHit(particlePositionV, wallSide));
				collisionType = CollisionWall;
				action = ParticleActionFinished;
				break;
			}
		}
		break;
	case CollisionIdGround:
	case CollisionIdLandscape:
	case CollisionIdRoof:
		// A shot collides with the landscape
		action = ParticleActionFinished;
		collisionType = CollisionFinished;
		break;
	default:
		action = ParticleActionNone;
		break;
	};

	// Tell all TankAIs about this collision (if any)
	if (collisionType != CollisionNone)
	{
		std::map<unsigned int, Tank *> tanks = 
			context_->tankContainer->getAllTanks();
		std::map<unsigned int, Tank *>::iterator itor;
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *tank = (*itor).second;
			TankAI *ai = tank->getTankAI();
			if (ai)
			{		
				if (tank->getState().getState() == TankState::sNormal &&
					tank->getState().getSpectator() == false)
				{
					ai->shotLanded(collisionType, otherInfo, 
						shot->getWeapon(), shot->getPlayerId(), 
						shot->getCurrentPosition(),
						shot->getLandedCounter());
				}
			}
		}
	}

	// Perform the actual action that has been worked
	// out by the collision handler
	switch (action)
	{
	case ParticleActionFinished:
		shot->collision(particlePositionV);
		break;
	case ParticleActionBounce:
		collisionBounce(o1, o2, contacts, noContacts, deflectPower);
		break;
	default:
		break;
	}
}

ScorchedCollisionHandler::ParticleAction ScorchedCollisionHandler::collisionShield(
	unsigned int shotId,
	unsigned int hitId,
	Vector &collisionPos,
	PhysicsParticleMeta *shot,
	float hitPercentage,
	float &deflectPower)
{
	// Check the target still exists
	Target *hitTarget = context_->targetContainer->getTargetById(hitId);
	if (!hitTarget) return ParticleActionNone;

	// Check target (if tank) is still alive
	if (!hitTarget->getAlive()) return ParticleActionNone;
	
	// Check tank has a shield
	Accessory *accessory = hitTarget->getShield().getCurrentShield();
	if (!accessory) return ParticleActionNone;
	Shield *shield = (Shield *) accessory->getAction();

	// Did this tank fire the shot
	if (shotId == hitId)
	{
		return ParticleActionNone;
	}

	// Is this tank in the shield
	// This should always be a tank as it is the one firing
	Tank *shotTank = context_->tankContainer->getTankById(shotId);
	if (shotTank)
	{
		float distance = (shotTank->getPosition().getTankPosition() -
			hitTarget->getTargetPosition()).Magnitude();
		float shieldSize = shield->getActualRadius();
		if (distance < shieldSize)
		{
			return ParticleActionNone;
		}
	}

    // Check for half shields
	if (shield->getHalfShield())
	{
		Vector normal = (collisionPos - 
			hitTarget->getTargetPosition()).Normalize();
		Vector up(0.0f, 0.0f, 1.0f);
		if (!(normal.dotP(up) > 0.7f))
		{
			return ParticleActionNone;
		}
	}

	// Check for other shield types
	switch (shield->getShieldType())
	{
	case Shield::ShieldTypeNormal:
		if (hitPercentage > 0.0f) 
		{
			context_->actionController->addAction(
				new ShieldHit(hitTarget->getPlayerId(),
				collisionPos,
				hitPercentage));
		}
		return ParticleActionFinished;
	case Shield::ShieldTypeReflective:
		deflectPower = ((ShieldReflective *)shield)->getDeflectFactor();
		if (hitPercentage > 0.0f)
		{
			context_->actionController->addAction(
				new ShieldHit(hitTarget->getPlayerId(),
				collisionPos,
				hitPercentage));
		}
		return ParticleActionBounce;
	case Shield::ShieldTypeMag:
		{
			ShieldMag *magShield = (ShieldMag *) shield;
			Vector force(0.0f, 0.0f, magShield->getDeflectPower());
			shot->applyForce(force);

			if (hitPercentage > 0.0f)
			{
				hitTarget->getShield().setShieldPower(
					hitTarget->getShield().getShieldPower() -
					shield->getHitRemovePower() * hitPercentage);
			}
		}
		return ParticleActionNone;
	}
	return ParticleActionNone;
}

void ScorchedCollisionHandler::collisionBounce(dGeomID o1, dGeomID o2, 
		dContactGeom *contacts, int noContacts, double bounceVel, double bounceFactor)
{
	dContact contact;
	contact.surface.mode = dContactBounce;// | dContactSoftCFM;
	contact.surface.mu = dInfinity;
	contact.surface.mu2 = 0;
	contact.surface.bounce = bounceFactor;
	contact.surface.bounce_vel = bounceVel;
	contact.surface.soft_cfm = 0.01;

	for (int i=0; i<noContacts; i++)
	{
		// Create the contact joints for this collision
		contact.geom = contacts[i];

		PhysicsEngine &engine =
			context_->actionController->getPhysics();

		dBodyID b1 = dGeomGetBody(o1);
		dBodyID b2 = dGeomGetBody(o2);
		dJointID c = dJointCreateContact(
			engine.getWorld(),
			engine.getContactGroup(),
			&contact);
		dJointAttach (c,b1,b2);
	}
}
