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


#include <actions/TankSay.h>
#include <tank/TankContainer.h>
#include <sprites/TalkRenderer.h>
#include <engine/ActionController.h>
#include <common/Defines.h>
#include <common/OptionsParam.h>
#include <common/Logger.h>

REGISTER_ACTION_SOURCE(TankSay);

TankSay::TankSay()
{
}

TankSay::TankSay(unsigned int playerId,
		const char *text) :
	playerId_(playerId), text_(text)
{

}

TankSay::~TankSay()
{
}

void TankSay::init()
{
	Tank *tank = 
		TankContainer::instance()->getTankById(playerId_);
	if (tank)
	{
		if (!OptionsParam::instance()->getOnServer()) 
		{
			// put a speach bubble over the talking tank
			TalkRenderer *talk = new TalkRenderer(
				tank->getPhysics().getTankTurretPosition());
			ActionController::instance()->addAction(new SpriteAction(talk));
		}

		Logger::log(tank->getPlayerId(), text_.c_str());
	}
}

void TankSay::simulate(float frameTime, bool &remove)
{
	remove = true;
}

bool TankSay::writeAction(NetBuffer &buffer)
{
	buffer.addToBuffer(playerId_);
	buffer.addToBuffer(text_);
	return true;
}

bool TankSay::readAction(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(playerId_)) return false;
	if (!reader.getFromBuffer(text_)) return false;
	return true;
}
