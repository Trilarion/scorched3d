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

#include <server/ServerStateTooFewPlayersState.h>
#include <server/ServerTooFewPlayersStimulus.h>
#include <server/ScorchedServer.h>
#include <server/ServerNewGameState.h>
#include <server/ServerState.h>
#include <scorched/ServerDialog.h>
#include <coms/ComsGameStoppedMessage.h>
#include <coms/ComsMessageSender.h>
#include <common/OptionsGame.h>
#include <common/Logger.h>

ServerStateTooFewPlayersState::ServerStateTooFewPlayersState()
{
}

ServerStateTooFewPlayersState::~ServerStateTooFewPlayersState()
{
}

void ServerStateTooFewPlayersState::enterState(const unsigned state)
{
	// Make sure all tanks are dead
	ScorchedServer::instance()->getTankContainer().setAllDead();

	// Tell any clients why the game has been abandonded
	ComsGameStoppedMessage gameStopped;
	ComsMessageSender::sendToAllConnectedClients(gameStopped);

	serverLog(0, "Too few players, stopping play");
	sendString(0, "Too few players, stopping play");
}

bool ServerStateTooFewPlayersState::acceptStateChange(const unsigned state, 
		const unsigned nextState,
		float frameTime)
{
	bool readyToPlay = !ServerTooFewPlayersStimulus::instance()->
		acceptStateChange(state, nextState, frameTime);

	if (!readyToPlay)
	{
		if (ServerNewGameState::addTanksToGame(state) > 0)
		{
			ScorchedServer::instance()->getGameState().stimulate(
				ServerState::ServerStimulusReady);
		}
	}
	else
	{
		// Just to make it more obvious we do one thing or the other
		return true;
	}

	return false;
}
