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

#include <weapons/WeaponScatterDirection.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeTex.h>
#include <common/Defines.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponScatterDirection);

WeaponScatterDirection::WeaponScatterDirection()
{

}

WeaponScatterDirection::~WeaponScatterDirection()
{

}

bool WeaponScatterDirection::parseXML(AccessoryCreateContext &context, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, accessoryNode)) return false;

	// Get the next weapon
	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("aimedweapon", subNode)) return false;

	// Check next weapon is correct type
	AccessoryPart *accessory = context.getAccessoryStore()->
		createAccessoryPart(context, parent_, subNode);
	if (!accessory || accessory->getType() != AccessoryPart::AccessoryWeapon)
	{
		return subNode->returnError("Failed to find sub weapon, not a weapon");
	}
	aimedWeapon_ = (Weapon*) accessory;

	if (!accessoryNode->getNamedChild("direction", direction_)) return false;
	if (!accessoryNode->getNamedChild("directionoffset", directionOffset_)) return false;

	return true;
}

void WeaponScatterDirection::fireWeapon(ScorchedContext &context,
	WeaponFireContext &weaponContext, FixedVector &position, FixedVector &v)
{
	RandomGenerator &random = context.actionController->getRandom();
	FixedVector vel;
	vel[0] += direction_[0] - directionOffset_[0] + 
		directionOffset_[0] * 2 * random.getRandFixed();
	vel[1] += direction_[1] - directionOffset_[1] + 
		directionOffset_[1] * 2 * random.getRandFixed();
	vel[2] += direction_[2] - directionOffset_[2] + 
		directionOffset_[2] * 2 * random.getRandFixed();

	aimedWeapon_->fireWeapon(context, weaponContext, position, vel);

}