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

#include <weapons/WeaponLeapFrog.h>
#include <weapons/AccessoryStore.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapemap/HeightMap.h>

REGISTER_ACCESSORY_SOURCE(WeaponLeapFrog);

WeaponLeapFrog::WeaponLeapFrog():  
	collisionAction_(0), bounce_(0.6f)
{

}

WeaponLeapFrog::~WeaponLeapFrog()
{

}

bool WeaponLeapFrog::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the next weapon
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("collisionaction", subNode)) return false;

	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore()->
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	collisionAction_ = (Weapon*) accessory;

	// Get the bounce
	if (!accessoryNode->getNamedChild("bounce", bounce_)) return false;

	return true;
}

void WeaponLeapFrog::fireWeapon(ScorchedContext &context,
	unsigned int playerId, Vector &position, Vector &velocity,
	unsigned int data)
{
	Vector newVelocity = velocity * bounce_;
	if (newVelocity[2] < 0.0f) newVelocity[2] *= -1.0f;

	Vector newPosition = position;
	float minHeight = context.landscapeMaps->getGroundMaps().getInterpHeight(
		position[0], position[1]);

	if (position[2] < minHeight + 0.7f)
	{
		// Make sure position is not fired underground
		if (minHeight - position[2] > 6.5f) // Give room for shields as well
		{
		}
		else
		{
			newPosition[2] = minHeight + 0.7f;
		}
	}

	collisionAction_->fireWeapon(context, playerId, newPosition, newVelocity, data);
}
