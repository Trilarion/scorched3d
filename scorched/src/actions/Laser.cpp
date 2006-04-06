////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <actions/Laser.h>
#include <actions/TankDamage.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <weapons/AccessoryStore.h>
#include <weapons/Shield.h>
#include <actions/ShieldHit.h>
#include <target/TargetContainer.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <GLEXT/GLState.h>
#include <math.h>
#include <set>

REGISTER_ACTION_SOURCE(Laser);

Laser::Laser() :
	totalTime_(0.0f),
	drawLength_(0.0f),
	firstTime_(true)
{
}

Laser::Laser(unsigned int playerId,
		WeaponLaser *weapon,
		Vector &position, Vector &direction,
		unsigned int data) :
	totalTime_(0.0f),
	drawLength_(0.0f),
	firstTime_(true),
	playerId_(playerId), 
	weapon_(weapon),
	position_(position), 
	direction_(direction),
	data_(data)
{
}

Laser::~Laser()
{
}

void Laser::init()
{
	float per = direction_.Magnitude() / 50.0f;
	length_ = weapon_->getMinimumDistance() + 
		(weapon_->getMaximumDistance() - weapon_->getMinimumDistance()) * per;
	damage_ = weapon_->getMinimumHurt() + 
		(weapon_->getMaximumHurt() - weapon_->getMinimumHurt()) * (1.0f - per);

	Vector dir = direction_.Normalize();
	angXY_ = 180.0f - atan2f(dir[0], dir[1]) / 3.14f * 180.0f;
	angYZ_ = acosf(dir[2]) / 3.14f * 180.0f;
}

void Laser::simulate(float frameTime, bool &remove)
{
	if (firstTime_)
	{
		firstTime_ = false;
		if (damage_ > 0.0f && direction_.Magnitude() > 0.0f)
		{
			std::set<unsigned int> damagedTargets_;

			// Build a set of all tanks in the path of the laser
			Vector pos = position_;
			Vector dir = direction_.Normalize() / 10.0f;
			bool end = false;
			while (!end)
			{
				std::map<unsigned int, Target *> &targets = 
					context_->targetContainer->getTargets();
				std::map<unsigned int, Target *>::iterator itor;
				for (itor = targets.begin();
					itor != targets.end();
					itor++)
				{
					Target *current = (*itor).second;
					if (current->getAlive() &&
						current->getPlayerId() != playerId_)
					{
						float targetDistance = 
							(current->getTargetPosition() -	pos).Magnitude();
						if (current->getShield().getCurrentShield())
						{
							Shield *shield = (Shield *)
								current->getShield().getCurrentShield()->getAction();
							if (shield->getLaserProof() &&
								targetDistance < shield->getActualRadius())
							{
								context_->actionController->addAction(
									new ShieldHit(current->getPlayerId(), pos, 0.0f));

								end = true;
								break;
							}
						}

						if (targetDistance < weapon_->getHurtRadius() + 
							MAX(current->getLife().getSize()[0], current->getLife().getSize()[1]))
						{
							damagedTargets_.insert(current->getPlayerId());
						}
					}
				}

				if (!end)
				{
					pos += dir;
					drawLength_ = (pos - position_).Magnitude();
					if (drawLength_ > length_) end = true;
				}
			}

			// Subtract set amount from all tanks
			std::set<unsigned int>::iterator itor;
			for (itor = damagedTargets_.begin();
				itor != damagedTargets_.end();
				itor++)
			{
				unsigned int damagedTarget = (*itor);
				context_->actionController->addAction(
					new TankDamage(
						weapon_, damagedTarget, playerId_,
						damage_, false, false, false, data_));
			}
		}
	}

	totalTime_ += frameTime;

	remove = (totalTime_ > weapon_->getTotalTime());
	Action::simulate(frameTime, remove);
}

void Laser::draw()
{
	if (!context_->serverMode && (drawLength_ > 0.0f))
	{
		static GLUquadric *obj = 0;
		if (!obj)
		{
			obj = gluNewQuadric();
		}
		float timePer = (1.0f - totalTime_ / weapon_->getTotalTime()) * 0.5f;
		float radius1 = 0.05f / 2.0f * weapon_->getHurtRadius();
		float radius2 = 0.2f / 2.0f * weapon_->getHurtRadius();

		GLState glState(GLState::TEXTURE_OFF | GLState::BLEND_ON);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColor4f(1.0f, 1.0f, 1.0f,	timePer);
		glPushMatrix();
			glTranslatef(position_[0], position_[1], position_[2]);
			glRotatef(angXY_, 0.0f, 0.0f, 1.0f);
			glRotatef(angYZ_, 1.0f, 0.0f, 0.0f);

			glColor4f(1.0f, 1.0f, 1.0f,	timePer);
			gluCylinder(obj, radius1, radius1, drawLength_, 3, 1);

			glColor4f(
				weapon_->getColor()[0],
				weapon_->getColor()[1],
				weapon_->getColor()[2],
				timePer);
			gluCylinder(obj, radius2, radius2, drawLength_, 5, 1);
		glPopMatrix();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

bool Laser::writeAction(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(position_);
	buffer.addToBuffer(direction_);
	buffer.addToBuffer(data_);
	context_->accessoryStore->writeWeapon(buffer, weapon_);
	return true;
}

bool Laser::readAction(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(position_)) return false;
	if (!reader.getFromBuffer(direction_)) return false;
	if (!reader.getFromBuffer(data_)) return false;
	weapon_ = (WeaponLaser *) context_->accessoryStore->readWeapon(reader); if (!weapon_) return false;
	return true;
}
