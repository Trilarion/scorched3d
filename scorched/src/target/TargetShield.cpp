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

#include <target/TargetShield.h>
#include <target/TargetSpace.h>
#include <weapons/AccessoryStore.h>
#include <weapons/ShieldRound.h>
#include <weapons/ShieldSquare.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>

TargetShield::TargetShield(ScorchedContext &context,
	unsigned int playerId) :
	context_(context),
	currentShield_(0),
	power_(0), 
	target_(0)
{
}

TargetShield::~TargetShield()
{
}

void TargetShield::newGame()
{
	setCurrentShield(0);
}

void TargetShield::setPosition(Vector &pos)
{
	position_ = pos;
}

void TargetShield::setCurrentShield(Accessory *sh)
{
	if (sh)
	{
		Shield *shield = (Shield *) sh->getAction();
		power_ = shield->getPower();
		currentShield_ = sh;	
	}
	else
	{
		power_ = 0.0f;
		currentShield_ = 0;
	}

	// Update the target space with this new shield information
	context_.targetSpace->removeTarget(target_);
	context_.targetSpace->addTarget(target_);
}

void TargetShield::setShieldPower(float power)
{
	power_ = power;
	if (power_ <= 0.0f)
	{
		power_ = 0.0f;
		setCurrentShield(0);
	}
}

bool TargetShield::writeMessage(NetBuffer &buffer)
{
	unsigned int shieldId = (currentShield_?currentShield_->getAccessoryId():0);
	buffer.addToBuffer(shieldId);
	if (shieldId != 0) buffer.addToBuffer(power_);
	return true;
}

bool TargetShield::readMessage(NetBufferReader &reader)
{
	unsigned int shieldId;
	if (!reader.getFromBuffer(shieldId)) return false;
	if (shieldId == 0) setCurrentShield(0);
	else setCurrentShield(context_.accessoryStore->findByAccessoryId(shieldId));
	if (shieldId != 0) if (!reader.getFromBuffer(power_)) return false;
	return true;
}
