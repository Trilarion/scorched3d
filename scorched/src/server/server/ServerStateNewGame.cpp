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

#include <server/ServerStateNewGame.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerSimulator.h>
#include <server/ServerConsoleProgressCounter.h>
#include <server/ServerChannelManager.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerLoadLevel.h>
#include <weapons/EconomyStore.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <landscapemap/LandscapeMaps.h>
#include <common/OptionsTransient.h>

ServerStateNewGame::ServerStateNewGame()
{
}

ServerStateNewGame::~ServerStateNewGame()
{
}

void ServerStateNewGame::newGame()
{
	newGameState();

	// Store this as the current level
	ScorchedServer::instance()->getServerSimulator().newLevel();

	// Make sure tanks are in correct state
	std::set<unsigned int> loadingDestinations;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		tankItor++)
	{
		Tank *tank = tankItor->second;
		if (tank->getState().getState() != TankState::sLoading)
		{
			tank->getState().setState(TankState::sLoading);
			if (loadingDestinations.find(tank->getDestinationId()) == loadingDestinations.end())
			{
				loadingDestinations.insert(tank->getDestinationId());
			}
		}
	}

	// Tell all destinations to load
	std::set<unsigned int>::iterator destItor;
	for (destItor = loadingDestinations.begin();
		destItor != loadingDestinations.end();
		destItor++)
	{
		ServerLoadLevel::destinationLoadLevel(*destItor);
	}
}

void ServerStateNewGame::newGameState()
{
	// Make sure options are up to date
	if (ScorchedServer::instance()->getOptionsGame().commitChanges())
	{
		ServerChannelManager::instance()->sendText(
			ChannelText("info", 
				"GAME_OPTIONS_CHANGED", 
				"Game options have been changed!"),
			true);
	}	

	// Make any enconomic changes
	EconomyStore::instance()->getEconomy()->calculatePrices();
	EconomyStore::instance()->getEconomy()->savePrices();

	// Get a landscape definition to use
	ServerCommon::serverLog("Generating landscape");
	LandscapeDefinition defn;
	if (ScorchedServer::instance()->getLandscapeMaps().getDefinitions().
		getDefinition().getDefinitionNumber() == 0)
	{
		defn = ScorchedServer::instance()->getLandscapes().getBlankLandscapeDefn();
	}
	else
	{
		defn = ScorchedServer::instance()->getLandscapes().getRandomLandscapeDefn(
			ScorchedServer::instance()->getContext().getOptionsGame(),
			ScorchedServer::instance()->getContext().getTankContainer());
	}

	// Set all options (wind etc..)
	ScorchedServer::instance()->getContext().getOptionsTransient().newGame();

	// Generate the new level
	ScorchedServer::instance()->getLandscapeMaps().generateMaps(
		ScorchedServer::instance()->getContext(), defn, 
		ServerConsoleProgressCounter::instance()->getProgressCounter());
	ServerCommon::serverLog("Finished generating landscape");
}