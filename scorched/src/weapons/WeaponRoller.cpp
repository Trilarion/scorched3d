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

#include <weapons/WeaponRoller.h>
#include <actions/Roller.h>
#include <common/Defines.h>

REGISTER_ACCESSORY_SOURCE(WeaponRoller);

WeaponRoller::WeaponRoller()
{

}

WeaponRoller::~WeaponRoller()
{

}

bool WeaponRoller::parseXML(XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(accessoryNode)) return false;

	// Get the accessory size
	XMLNode *sizeNode = accessoryNode->removeNamedChild("size");
	if (!sizeNode)
	{
		dialogMessage("Accessory",
			"Failed to find size node in accessory \"%s\"",
			name_.c_str());
		return false;
	}
	size_ = atoi(sizeNode->getContent());

	XMLNode *timeNode = accessoryNode->removeNamedChild("rollertime");
	if (!timeNode)
	{
		dialogMessage("Accessory",
			"Failed to find rollertime node in accessory \"%s\"",
			name_.c_str());
		return false;
	}
	rollerTime_ = (float) atof(timeNode->getContent());

	XMLNode *heightNode = accessoryNode->removeNamedChild("rollerheight");
	if (!heightNode)
	{
		dialogMessage("Accessory",
			"Failed to find rollerheight node in accessory \"%s\"",
			name_.c_str());
		return false;
	}
	rollerHeight_ = (float) atof(heightNode->getContent());

	XMLNode *stepNode = accessoryNode->removeNamedChild("steptime");
	if (!stepNode)
	{
		dialogMessage("Accessory",
			"Failed to find steptime node in accessory \"%s\"",
			name_.c_str());
		return false;
	}
	stepTime_ = (float) atof(stepNode->getContent());

	XMLNode *rollersNode = accessoryNode->removeNamedChild("numberrollers");
	if (!rollersNode)
	{
		dialogMessage("Accessory",
			"Failed to find numberrollers node in accessory \"%s\"",
			name_.c_str());
		return false;
	}
	numberRollers_ = atoi(rollersNode->getContent());	

	return true;
}

bool WeaponRoller::writeAccessory(NetBuffer &buffer)
{
	if (!Weapon::writeAccessory(buffer)) return false;
	buffer.addToBuffer(rollerTime_);
	buffer.addToBuffer(rollerHeight_);
	buffer.addToBuffer(stepTime_);
	buffer.addToBuffer(numberRollers_);
	buffer.addToBuffer(size_);
	return true;
}

bool WeaponRoller::readAccessory(NetBufferReader &reader)
{
	if (!Weapon::readAccessory(reader)) return false;
	if (!reader.getFromBuffer(rollerTime_)) return false;
	if (!reader.getFromBuffer(rollerHeight_)) return false;
	if (!reader.getFromBuffer(stepTime_)) return false;
	if (!reader.getFromBuffer(numberRollers_)) return false;
	if (!reader.getFromBuffer(size_)) return false;
	return true;
}

void WeaponRoller::fireWeapon(ScorchedContext &context,
	unsigned int playerId, Vector &position, Vector &velocity)
{
	for (int i=0; i<getNumberRollers(); i++)
	{
		int x = int(position[0] + RAND * 4.0f - 2.0f);
		int y = int(position[1] + RAND * 4.0f - 2.0f);
		addRoller(context, playerId, x, y);
	}
}

void WeaponRoller::addRoller(ScorchedContext &context,
	unsigned int playerId,
	int x, int y)
{
	// Ensure that the Roller has not hit the walls
	// or anything outside the landscape
	if (x > 1 && y > 1 &&
		x < context.landscapeMaps.getHMap().getWidth() - 1 &&
		y < context.landscapeMaps.getHMap().getWidth() - 1)
	{
		context.actionController.addAction(
			new Roller(x, y, this, playerId));
	}
}
