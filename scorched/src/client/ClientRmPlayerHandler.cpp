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


#include <client/ClientRmPlayerHandler.h>
#include <client/ClientState.h>
#include <tank/TankContainer.h>
#include <common/Logger.h>
#include <coms/ComsRmPlayerMessage.h>

ClientRmPlayerHandler *ClientRmPlayerHandler::instance_ = 0;

ClientRmPlayerHandler *ClientRmPlayerHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ClientRmPlayerHandler;
	}
	return instance_;
}

ClientRmPlayerHandler::ClientRmPlayerHandler()
{
	ComsMessageHandler::instance()->addHandler(
		"ComsRmPlayerMessage",
		this);
}

ClientRmPlayerHandler::~ClientRmPlayerHandler()
{
}

bool ClientRmPlayerHandler::processMessage(NetPlayerID &id,
		const char *messageType,
		NetBufferReader &reader)
{
	ComsRmPlayerMessage message;
	if (!message.readMessage(reader)) return false;

	// Find and remove this tank from tank collections
	Tank *tank = TankContainer::instance()->
		removeTank(message.getPlayerId());
	if (!tank)
	{
		Logger::log(0, "Failed to find player to remove");
	}
	else
	{
		// Print out info about removed tank
		Logger::log(message.getPlayerId(), "Player disconnected \"%s\"",
					tank->getName());	
	}

    delete tank;
	return true;
}