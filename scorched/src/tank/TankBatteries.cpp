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

#include <tank/TankBatteries.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <common/OptionsGame.h>
#include <stdio.h>

TankBatteries::TankBatteries(ScorchedContext &context) :
	context_(context), infinite_(false)
{
	reset();
}

TankBatteries::~TankBatteries()
{

}

void TankBatteries::reset()
{
	infinite_ = false;
	batteryCount_ = 0;
	std::list<Accessory *> accessories = 
		context_.accessoryStore->getAllOthers();
	std::list<Accessory *>::iterator itor;
	for (itor = accessories.begin();
		itor != accessories.end();
		itor++)
	{
		Accessory *accessory = (*itor);
		if (accessory->getType() == AccessoryPart::AccessoryBattery)
		{
			if (accessory->getMaximumNumber() > 0)
			{
				if (context_.optionsGame->getGiveAllWeapons() ||
					accessory->getStartingNumber() == -1)
				{
					addBatteries(20);
					infinite_ = true;
				}
				else if (accessory->getStartingNumber() > 0)
				{
					addBatteries(accessory->getStartingNumber());
				}
			}
		}
	}
}

void TankBatteries::rmBatteries(int no)
{
	if (infinite_) return;

	batteryCount_ -= no;
	if (batteryCount_ < 0)
	{
		batteryCount_ = 0;
	}
}

void TankBatteries::addBatteries(int no)
{
	batteryCount_+=no;
}

bool TankBatteries::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(batteryCount_);
	buffer.addToBuffer(infinite_);
	return true;
}

bool TankBatteries::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(batteryCount_)) return false;
	if (!reader.getFromBuffer(infinite_)) return false;
	return true;
}
