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

#include <weapons/WeaponRedirect.h>
#include <weapons/AccessoryStore.h>
#include <engine/ActionController.h>
#include <server/ScorchedServer.h>
#include <common/Defines.h>
#include <tank/TankLib.h>
#include <math.h>

REGISTER_ACCESSORY_SOURCE(WeaponRedirect);

WeaponRedirect::WeaponRedirect() :
	hredirect_(0.0f), vredirect_(0.0f),
	habs_(false), vabs_(false),
	nextAction_(0)
{

}

WeaponRedirect::~WeaponRedirect()
{

}

bool WeaponRedirect::parseXML(XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(accessoryNode)) return false;

	if (!accessoryNode->getNamedChild("hredirect", hredirect_)) return false;
	if (!accessoryNode->getNamedChild("habs", habs_)) return false;
	if (!accessoryNode->getNamedChild("vredirect", vredirect_)) return false;
	if (!accessoryNode->getNamedChild("vabs", vabs_)) return false;

	XMLNode *subNode = 0;
	if (!accessoryNode->getNamedChild("nextaction", subNode)) return false;
	
	// Check next weapon is correct type
	Accessory *accessory = 
		ScorchedServer::instance()->getAccessoryStore().createAccessory(subNode);
	if (!accessory || accessory->getType() != Accessory::AccessoryWeapon)
	{
		dialogMessage("Accessory",
			"Sub weapon of wrong type \"%s\"",
			name_.c_str());
		return false;
	}
	nextAction_ = (Weapon*) accessory;

	return true;
}

bool WeaponRedirect::writeAccessory(NetBuffer &buffer)
{
	if (!Weapon::writeAccessory(buffer)) return false;
	if (!Weapon::write(buffer, nextAction_)) return false;
	buffer.addToBuffer(hredirect_);
	buffer.addToBuffer(vredirect_);
	buffer.addToBuffer(habs_);
	buffer.addToBuffer(vabs_);
	return true;
}

bool WeaponRedirect::readAccessory(NetBufferReader &reader)
{
	if (!Weapon::readAccessory(reader)) return false;
	nextAction_ = Weapon::read(reader); if (!nextAction_) return false;
	if (!reader.getFromBuffer(hredirect_)) return false;
	if (!reader.getFromBuffer(vredirect_)) return false;
	if (!reader.getFromBuffer(habs_)) return false;
	if (!reader.getFromBuffer(vabs_)) return false;
	return true;
}

void WeaponRedirect::fireWeapon(ScorchedContext &context,
	unsigned int playerId, Vector &position, Vector &velocity,
	unsigned int data)
{
	float currentMag = velocity.Magnitude();
	float currenth = (atan2f(velocity[1], velocity[0]) / 3.14f * 180.0f) - 90.0f;
	float dist = (float) sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1]);
	float currentv = atan2f(dist, velocity[2]) / 3.14f * 180.0f;

	if (habs_) currenth = hredirect_;
	else currenth += hredirect_;
	if (vabs_) currentv = vredirect_;
	else currentv += vredirect_;
	
	Vector newVelocity = TankLib::getVelocityVector(currenth, 90.0f - currentv);
	newVelocity.StoreNormalize();
	newVelocity *= currentMag;
	
	nextAction_->fireWeapon(context, playerId, position, newVelocity, data);
}

