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
#include <actions/WallHit.h>
#include <actions/ShieldHit.h>
#include <engine/ScorchedCollisionHandler.h>
#include <engine/PhysicsParticle.h>
#include <engine/ActionController.h>

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

	// only collide with the ground, walls or landscape,
	// or iteself
	switch (otherInfo->id)
	{
	case CollisionIdGround:
	case CollisionIdLandscape:
		collisionBounce(o1, o2, contacts, noContacts, 10000.0);
		break;
	case CollisionIdBounce:
		//collisionBounce(o1, o2, contacts, noContacts);
		break;
	case CollisionIdWallRight:
	case CollisionIdWallTop:
	case CollisionIdWallBottom:
	case CollisionIdWallLeft:
	case CollisionIdTank:
		{
			Vector bouncePositionV(
				(float) bouncePosition[0],
				(float) bouncePosition[1],
				(float) bouncePosition[2]);

			PhysicsParticleMeta *particle = (PhysicsParticleMeta *) bounceInfo->data;
			particle->collision(bouncePositionV);
		}
		break;
	case CollisionIdShieldSmall:
	case CollisionIdShieldLarge:
		{
			Vector bouncePositionV(
				(float) bouncePosition[0],
				(float) bouncePosition[1],
				(float) bouncePosition[2]);

			unsigned int id = (unsigned int) otherInfo->data;
			ParticleAction action = collisionShield(id, bouncePositionV, 
				((otherInfo->id==CollisionIdShieldLarge)?
				Shield::ShieldSizeLarge:Shield::ShieldSizeSmall));

			// Unless there is no shield, we bounce off all shields
			if (action != ParticleActionNone)
			{
				collisionBounce(o1, o2, contacts, noContacts, 10000.0);
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
		collisionBounce(o1, o2, contacts, noContacts);
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
	case CollisionIdShieldLarge:
	case CollisionIdShieldSmall:
		// May have collided with shield
		{
			// Only collide with other peoples shields
			if (shot->getPlayerId() != id)
			{
				action = collisionShield(id, particlePositionV, 
										 ((otherInfo->id==CollisionIdShieldLarge)?
										 Shield::ShieldSizeLarge:Shield::ShieldSizeSmall));
			}
			else
			{
				action = ParticleActionNone;
			}
		}
		break;
	case CollisionIdTank:
		// A shot collides with a tank
		{
			// Only collide with other tanks
			if (shot->getPlayerId() != id)
			{
				// Make sure tank we are colliding with is alive
				Tank *tank = context_->tankContainer.getTankById(id);
				if (tank &&
					tank->getState().getState() == TankState::sNormal &&
					tank->getState().getSpectator() == false)
				{
					action = ParticleActionFinished;
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

			context_->actionController.addAction(
				new WallHit(particlePositionV, wallSide));

			// Choose what to do depending on the wall
			switch(context_->optionsTransient.getWallType())
			{
			case OptionsTransient::wallBouncy:
				action = ParticleActionBounce;
				break;
			case OptionsTransient::wallWrapAround:
				{
					Vector currentPosition = shot->getCurrentPosition();
					action = ParticleActionNone; // Not strictly true as
					// we are about to move the particle
					switch (otherInfo->id)
					{
					case CollisionIdWallLeft:
						currentPosition[0] = 245.0f;
						break;
					case CollisionIdWallRight:
						currentPosition[0] = 10.0f;
						break;
					case CollisionIdWallTop:
						currentPosition[1] = 245.0f;
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
			default:
				action = ParticleActionFinished;
				break;
			}
		}
		break;
	case CollisionIdGround:
	case CollisionIdLandscape:
		// A shot collides with the landscape
		action = ParticleActionFinished;
		break;
	default:
		action = ParticleActionNone;
		break;
	};

	// Tell all TankAIs about this collision (if any)
	if (action != ParticleActionNone)
	{
		std::map<unsigned int, Tank *> &tanks = 
			context_->tankContainer.getPlayingTanks();
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
					ai->shotLanded(action, otherInfo, 
						shot->getWeapon(), shot->getPlayerId(), 
						shot->getCurrentPosition());
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
		collisionBounce(o1, o2, contacts, noContacts);
		break;
	default:
		break;
	}
}

ParticleAction ScorchedCollisionHandler::collisionShield(
	unsigned int id,
	Vector &collisionPos,
	Shield::ShieldSize size)
{
	// Check tank still exists and is alive
	Tank *tank = context_->tankContainer.getTankById(id);
	if (tank && tank->getState().getState() == TankState::sNormal &&
		tank->getState().getSpectator() == false)
	{
		Shield *shield = tank->getAccessories().getShields().getCurrentShield();
		if (shield)
		{
			if (shield->getRadius() == size)
			{
				switch (shield->getShieldType())
				{
				case Shield::ShieldTypeNormal:
					context_->actionController.addAction(
						new ShieldHit(tank->getPlayerId()));
					return ParticleActionFinished;
				case Shield::ShieldTypeReflective:
					context_->actionController.addAction(
						new ShieldHit(tank->getPlayerId()));
					return ParticleActionBounce;
				case Shield::ShieldTypeReflectiveMag:
				{
					Vector normal = (collisionPos - 
						tank->getPhysics().getTankPosition()).Normalize();
					Vector up(0.0f, 0.0f, 1.0f);
					if (normal.dotP(up) > 0.8f)
					{
						context_->actionController.addAction(
							new ShieldHit(tank->getPlayerId()));
						return ParticleActionBounce;
					}
					return ParticleActionNone;
				}
				}
			}
		}
	}

	return ParticleActionNone;
}

void ScorchedCollisionHandler::collisionBounce(dGeomID o1, dGeomID o2, 
		dContactGeom *contacts, int noContacts, double bounceVel)
{
	// TODO allow the physics to be changed by the server settings
	dContact contact;
	contact.surface.mode = dContactBounce;// | dContactSoftCFM;
	contact.surface.mu = dInfinity;
	contact.surface.mu2 = 0;
	contact.surface.bounce = 1.0;
	contact.surface.bounce_vel = bounceVel;
	contact.surface.soft_cfm = 0.01;

	for (int i=0; i<noContacts; i++)
	{
		// Create the contact joints for this collision
		contact.geom = contacts[i];
		context_->actionController.getPhysics().addCollision(o1, o2, contact);
	}
}
