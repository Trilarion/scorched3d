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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <weapons/AccessoryStore.h>
#include <weapons/EconomyStore.h>
#include <coms/NetLoopBack.h>
#include <coms/NetServer.h>
#include <common/Defines.h>
#include <common/Clock.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsGame.h>
#include <common/OptionsParam.h>
#include <common/OptionsTransient.h>
#include <engine/ActionController.h>
#include <engine/ModFiles.h>
#include <landscapedef/LandscapeDefinitions.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStore.h>
#include <tankgraph/TankModelStore.h>
#include <scorched/ServerDialog.h>
#include <server/ServerTimedMessage.h>
#include <server/ServerMessageHandler.h>
#include <server/ServerPlayerReadyHandler.h>
#include <server/ServerTextHandler.h>
#include <server/ServerDefenseHandler.h>
#include <server/ServerPlayedMoveHandler.h>
#include <server/ServerAddPlayerHandler.h>
#include <server/ServerAdminHandler.h>
#include <server/ServerPlayerAimHandler.h>
#include <server/ServerHaveModFilesHandler.h>
#include <server/ServerBuyAccessoryHandler.h>
#include <server/ServerKeepAliveHandler.h>
#include <server/ServerFileAkHandler.h>
#include <server/ServerInitializeHandler.h>
#include <server/ServerConnectHandler.h>
#include <server/ServerFileServer.h>
#include <server/ServerRegistration.h>
#include <server/ServerWebServer.h>
#include <server/ServerLog.h>
#include <server/ServerBrowserInfo.h>
#include <server/ServerState.h>
#include <server/ServerCommon.h>
#include <server/ServerBanned.h>
#include <server/ServerMain.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <SDL/SDL.h>

Clock serverTimer;

void checkSettings()
{
	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 
		ScorchedServer::instance()->getOptionsGame().getNoMinPlayers())
	{
		dialogExit("ScorchedServer",
			"Cannot start a game with more teams than minimum players");
	}
	
	if (ScorchedServer::instance()->getOptionsGame().getTeamBallance() == 
		OptionsGame::TeamBallanceBotsVs &&
		ScorchedServer::instance()->getOptionsGame().getTeams() > 2)
	{
		dialogExit("ScorchedServer",
			"Cannot start a game with more than 2 teams in the bots vs mode");		
	}
}

bool startServer(bool local, ProgressCounter *counter)
{
	checkSettings();

	// Setup the message handling classes
	if (!local)
	{
		// Only create a net server for the actual multiplayer case
		// A loopback is created by the client for a single player game 
		ScorchedServer::instance()->getContext().netInterface = 
			//new NetServer(new NetServerScorchedProtocol());
			new NetServer(new NetServerCompressedProtocol());
	}

	ScorchedServer::instance()->getOptionsGame().updateChangeSet();
	ScorchedServer::instance()->getNetInterface().setMessageHandler(
		&ScorchedServer::instance()->getComsMessageHandler());
	ScorchedServer::instance()->getComsMessageHandler().setConnectionHandler(
		ServerMessageHandler::instance());
	ServerConnectHandler::instance();
	ServerTextHandler::instance();
	ServerPlayerReadyHandler::instance();
	ServerPlayerAimHandler::instance();
	ServerAdminHandler::instance();
	ServerHaveModFilesHandler::instance();
	ServerInitializeHandler::instance();
	ServerKeepAliveHandler::instance();
	ServerPlayedMoveHandler::instance();
	ServerFileAkHandler::instance();
	ServerBuyAccessoryHandler::instance();
	ServerAddPlayerHandler::instance();
	ServerDefenseHandler::instance();

	// Set the mod
	setDataFileMod(
		ScorchedServer::instance()->getOptionsGame().getMod());

	// Load mod
	if (OptionsParam::instance()->getDedicatedServer() ||
		OptionsParam::instance()->getLoadModFiles())
	{
		if (!ScorchedServer::instance()->getModFiles().loadModFiles(
			ScorchedServer::instance()->getOptionsGame().getMod(), false,
			counter)) return false;
	}

	if (!ScorchedServer::instance()->getAccessoryStore().parseFile(
		ScorchedServer::instance()->getOptionsGame(),
		counter)) return false;
	if (!ScorchedServer::instance()->getTankModels().loadTankMeshes(
		ScorchedServer::instance()->getContext(), counter))
		return false;
	ScorchedServer::instance()->getOptionsTransient().reset();
	if (!ScorchedServer::instance()->getLandscapes().readLandscapeDefinitions()) return false;

	// Add the server side bots
	// Add any new AIs
	if (!ScorchedServer::instance()->getTankAIs().loadAIs(
		&ScorchedServer::instance()->getAccessoryStore())) return false;
	TankAIAdder::addTankAIs(*ScorchedServer::instance());

	// Start the state machine
	ServerState::setupStates(ScorchedServer::instance()->getGameState());
	EconomyStore::instance();

	return true;
}

void serverMain(ProgressCounter *counter)
{
	// Create the server states
	if (!startServer(false, counter)) exit(64);

	// Try to start the server
	if (!ScorchedServer::instance()->getContext().netInterface->start(
		ScorchedServer::instance()->getOptionsGame().getPortNo()) ||
		!ServerBrowserInfo::instance()->start())
	{
		dialogExit("Scorched3D Server", 
			formatString("Failed to start the server.\n\n"
			"Could not bind to the server ports.\n"
			"Ensure the specified ports (%i, %i) do not conflict with any other program.",
			ScorchedServer::instance()->getOptionsGame().getPortNo(),
			ScorchedServer::instance()->getOptionsGame().getPortNo() + 1));
	}

 	if (ScorchedServer::instance()->getOptionsGame().getPublishServer()) 
	{
		ServerRegistration::instance()->start();
	}

	if (ScorchedServer::instance()->getOptionsGame().getManagementPortNo() > 0)
	{
		ServerWebServer::instance()->start(
			ScorchedServer::instance()->getOptionsGame().getManagementPortNo());
	}
	ServerLog::instance();

	Logger::log( "Server started");
}

void serverLoop()
{
	// Main server loop:
	if (ScorchedServer::instance()->getContext().netInterface)
	{
		Logger::processLogEntries();
		ScorchedServer::instance()->getNetInterface().processMessages();
		ServerBrowserInfo::instance()->processMessages();
		ServerWebServer::instance()->processMessages();

		float timeDifference = serverTimer.getTimeDifference();
		ScorchedServer::instance()->getGameState().simulate(timeDifference);
		ServerFileServer::instance()->simulate(timeDifference);
		ScorchedServerUtil::instance()->timedMessage.simulate();
		ServerKeepAliveHandler::instance()->checkKeepAlives();

		if (timeDifference > 5.0f &&
			OptionsParam::instance()->getDedicatedServer())
		{
			Logger::log(formatString("Warning: Server loop took %.2f seconds", 
				timeDifference));
		}
	}
}

class ConsoleServerProgressCounter : public ProgressCounterI
{
public:
	ConsoleServerProgressCounter() : lastOp_(""), hashes_(0) {}

	virtual void progressChange(const char *op, const float percentage)
	{
		if (0 != strcmp(op, lastOp_.c_str()))
		{
			Logger::log(op);
			printf("\n%s:", op);
			lastOp_ = op;
			hashes_ = 0;
		}

		int neededHashes = int(percentage / 10.0f);
		for (;hashes_<=neededHashes; hashes_++)
		{
			printf("#");
		}
		fflush(stdout);
	}
protected:
	std::string lastOp_;
	int hashes_;
};

void consoleServer()
{
	ProgressCounter progressCounter;
	ConsoleServerProgressCounter progressCounterI;
	progressCounter.setUser(&progressCounterI);

	printf("\nStarting Server...");
	ServerCommon::startFileLogger();
	serverMain(&progressCounter);
	printf("\nServer Started.\n");

	for (;;)
	{
		SDL_Delay(10);
		serverLoop();
	}
}

