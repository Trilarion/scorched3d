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

#include <weapons/AccessoryStore.h>
#include <weapons/WeaponProjectile.h>
#include <actions/ShotProjectile.h>

REGISTER_ACCESSORY_SOURCE(WeaponProjectile);

WeaponProjectile::WeaponProjectile() : 
	under_(false), collisionAction_(0), apexCollision_(false),
	showShotPath_(false), showEndPoint_(false)
{

}

WeaponProjectile::~WeaponProjectile()
{

}

bool WeaponProjectile::parseXML(XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(accessoryNode)) return false;

	// Get the accessory under
	XMLNode *underNode = accessoryNode->removeNamedChild("under");
	if (underNode) under_ = true;

	// Get the smoke trails
	XMLNode *smokeNode = accessoryNode->removeNamedChild("smoke");
	if (smokeNode) showShotPath_ = true;

	// Get the end point
	XMLNode *endPointNode = accessoryNode->removeNamedChild("showendpoint");
	if (endPointNode) showEndPoint_ = true;
	
	// Get the apex point
	XMLNode *apexNode = accessoryNode->removeNamedChild("apexcollision");
	if (apexNode) apexCollision_ = true;

	// Get the next weapon
	XMLNode *subNode = accessoryNode->removeNamedChild("collisionaction");
	if (!subNode)
	{
		dialogMessage("Accessory",
			"Failed to find collisionaction node in accessory \"%s\"",
			name_.c_str());
		return false;
	}
	// Check next weapon is correct type
	Accessory *accessory = AccessoryStore::instance()->createAccessory(subNode);
	if (!accessory || accessory->getType() != Accessory::AccessoryWeapon)
	{
		dialogMessage("Accessory",
			"Sub weapon of wrong type \"%s\"",
			name_.c_str());
		return false;
	}
	collisionAction_ = (Weapon*) accessory;

	return true;
}

bool WeaponProjectile::writeAccessory(NetBuffer &buffer)
{
	if (!Weapon::writeAccessory(buffer)) return false;
	if (!Weapon::write(buffer, collisionAction_)) return false;
	buffer.addToBuffer(showShotPath_);
	buffer.addToBuffer(showEndPoint_);
	buffer.addToBuffer(apexCollision_);
	buffer.addToBuffer(under_);
	return true;
}

bool WeaponProjectile::readAccessory(NetBufferReader &reader)
{
	if (!Weapon::readAccessory(reader)) return false;
	collisionAction_ = Weapon::read(reader); if (!collisionAction_) return false;
	if (!reader.getFromBuffer(showShotPath_)) return false;
	if (!reader.getFromBuffer(showEndPoint_)) return false;
	if (!reader.getFromBuffer(apexCollision_)) return false;
	if (!reader.getFromBuffer(under_)) return false;
	return true;
}

void WeaponProjectile::fireWeapon(ScorchedContext &context,
	unsigned int playerId, Vector &position, Vector &velocity)
{
	Action *action = new ShotProjectile(
		position, 
		velocity,
		this, playerId, 0, under_, 
		showEndPoint_, showShotPath_, apexCollision_);
	context.actionController.addAction(action);	
}
