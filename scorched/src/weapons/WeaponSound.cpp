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

#include <weapons/WeaponSound.h>
#include <engine/ActionController.h>
#include <actions/SoundAction.h>
#include <common/Defines.h>
#include <stdlib.h>

REGISTER_ACCESSORY_SOURCE(WeaponSound);

WeaponSound::WeaponSound()
{

}

WeaponSound::~WeaponSound()
{

}

const char *WeaponSound::getSound()
{
	std::string &sound = sounds_[rand() % sounds_.size()];
	return sound.c_str();
}

bool WeaponSound::parseXML(OptionsGame &context, 
	AccessoryStore *store, XMLNode *accessoryNode)
{
	if (!Weapon::parseXML(context, store, accessoryNode)) return false;

	std::string sound;
	while (accessoryNode->getNamedChild("sound", sound, false))
	{
		if (!checkDataFile("%s", sound.c_str())) return false;
		sounds_.push_back(sound);
	}
	return true;
}

void WeaponSound::fireWeapon(ScorchedContext &context,
	unsigned int playerId, Vector &position, Vector &velocity,
	unsigned int data)
{
	context.actionController->addAction(new SoundAction(this));
}
