////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <server/ServerState.h>

ServerState *ServerState::instance_ = 0;

ServerState *ServerState::instance()
{
	if (!instance_) instance_ = new ServerState();
	return instance_;
}

ServerState::ServerState() :
	serverState_(ServerStartupState)
{
}

ServerState::~ServerState()
{
}

void ServerState::simulate(float frameTime)
{
	switch (serverState_)
	{
	case ServerStartupState:
		newGame_.newGame();
		serverState_ = ServerWaitingForPlayersState;
		break;
	case ServerWaitingForPlayersState:
		newGame_.checkTeams();
		newGame_.checkBots(true);
		if (enoughPlayers_.enoughPlayers())
		{
			startingMatch_.reset();
			serverState_ = ServerMatchCountDownState;
		}
		break;
	case ServerMatchCountDownState:
		newGame_.checkTeams();
		newGame_.checkBots(true);
		if (enoughPlayers_.enoughPlayers())
		{
			if (startingMatch_.startingMatch(frameTime))
			{
				serverState_ = ServerNewLevelState;
			}
		}
		else
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		break;
	case ServerNewLevelState:
		serverState_ = ServerBuyingState;
		buying_.enterState();
		break;
	case ServerBuyingState:
		if (enoughPlayers_.enoughPlayers())
		{
			if (buying_.simulate(frameTime))
			{
				serverState_ = ServerPlayingState;
				playing_.enterState();
			}
		}
		else
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		break;
	case ServerPlayingState:
		if (enoughPlayers_.enoughPlayers())
		{
			if (playing_.simulate(frameTime))
			{
				serverState_ = ServerShotsState;
				shots_.enterState();
			}
		}
		else
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		
		break;
	case ServerShotsState:
		if (enoughPlayers_.enoughPlayers())
		{
			if (shots_.simulate(frameTime))
			{
				serverState_ = ServerPlayingState;
				playing_.enterState();
			}
		}
		else
		{
			startingMatch_.stoppingMatch();
			serverState_ = ServerWaitingForPlayersState;
		}
		break;
	}
}
