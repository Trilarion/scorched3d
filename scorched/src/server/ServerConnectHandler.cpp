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

#include <server/ServerConnectHandler.h>
#include <server/ServerState.h>
#include <server/ScorchedServer.h>
#include <server/TurnController.h>
#include <server/ServerCommon.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankDeadContainer.h>
#include <tank/TankContainer.h>
#include <tankai/TankAIAdder.h>
#include <tankai/TankAIStrings.h>
#include <common/OptionsParam.h>
#include <common/Defines.h>
#include <common/FileLines.h>
#include <common/OptionsGame.h>
#include <common/StatsLogger.h>
#include <common/Logger.h>
#include <coms/ComsAddPlayerMessage.h>
#include <coms/ComsConnectAcceptMessage.h>
#include <coms/ComsConnectMessage.h>
#include <coms/ComsMessageSender.h>
#include <coms/NetServer.h>

ServerConnectHandler *ServerConnectHandler::instance_ = 0;

ServerConnectHandler *ServerConnectHandler::instance()
{
	if (!instance_)
	{
		instance_ = new ServerConnectHandler;
	}
	return instance_;
}

ServerConnectHandler::ServerConnectHandler()
{
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsConnectMessage",
		this);
}

ServerConnectHandler::~ServerConnectHandler()
{
}

bool ServerConnectHandler::processMessage(unsigned int destinationId,
	const char *messageType, NetBufferReader &reader)
{
	unsigned int ipAddress = 0;
	// Only do this on the server, the client can have all bots
	if (OptionsParam::instance()->getDedicatedServer())
	{
		// First things, first
		// Check we can actually accept the connection
		if (ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers() <=
			ScorchedServer::instance()->getTankContainer().getNoOfTanks())
		{
			ServerCommon::sendString(destinationId, "Too many players");
			ServerCommon::kickDestination(destinationId, true);
			return true;		
		}

		// Get the ip address
		ipAddress = NetServer::getIpAddress((TCPsocket) destinationId);
	}

	// Decode the connect message
	ComsConnectMessage message;

	// Check the player protocol versions are the same (correct)
	if (!message.readMessage(reader) ||
		(0 != strcmp(message.getProtocolVersion(), ScorchedProtocolVersion)))
	{
		ServerCommon::sendString(destinationId, 			
			"The version of Scorched you are running\n"
			"does not match the server's version.\n"
			"This server is running Scorched build %s (%s).\n"
			"You are running Scorched build %s (%s).\n"
			"New versions can be downloaded from\n"
			"http://www.scorched3d.co.uk\n"
			"Connection failed.", 
			ScorchedVersion, ScorchedProtocolVersion,
			message.getVersion(), message.getProtocolVersion());
		Logger::log(0, 
			"ERROR: Player connected with out of date version \"%s(%s)\"",
			message.getVersion(),
			message.getProtocolVersion());

		ServerCommon::kickDestination(destinationId, true);
		return true;
	}

	// Check player availability
	if ((int) message.getNoPlayers() > 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers() -
		ScorchedServer::instance()->getTankContainer().getNoOfTanks())
	{
		ServerCommon::sendString(destinationId, "Too many players");
		ServerCommon::kickDestination(destinationId, true);
		return true;
	}

	// Check for a password (if any)
	if (ScorchedServer::instance()->getOptionsGame().getServerPassword()[0])
	{
		if (0 != strcmp(message.getPassword(),
						ScorchedServer::instance()->getOptionsGame().getServerPassword()))
		{
			ServerCommon::sendString(destinationId, 			
					   "This server is running a password protected game.\n"
					   "Your supplied password does not match.\n"
					"Connection failed.");
			Logger::log(0, "ERROR: Player connected with an invalid password");
			
			ServerCommon::kickDestination(destinationId, true);
			return true;
		}
	}

	// Check if a player from this destination has connected already
	std::map<unsigned int, Tank *> &playingTanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator playingItor;
	for (playingItor = playingTanks.begin();
		playingItor != playingTanks.end();
		playingItor++)
	{
		Tank *current = (*playingItor).second;
		if (current->getDestinationId() == destinationId)
		{
			Logger::log(0, "ERROR: Duplicate connection from destination \"%i\"", 
				destinationId);
			ServerCommon::kickDestination(destinationId);
			return true;
		}
		
		if (!ScorchedServer::instance()->getOptionsGame().getAllowSameIP() &&
			ipAddress != 0)
		{
			if (ipAddress == current->getIpAddress())
			{
				Logger::log(0, "Duplicate ip connection from destination \"%i\"", 
					destinationId);
				ServerCommon::kickDestination(destinationId);
				return true;
			}
		}
	}

	// Generate the players unique id (if we need to)
	const char *uniqueId = message.getUniqueId();
	if (0 == uniqueId[0]) // No ID
	{
		uniqueId = StatsLogger::instance()->allocateId();
	}

	// Send the connection accepted message to the client
	ComsConnectAcceptMessage acceptMessage(
		destinationId,
		ScorchedServer::instance()->getOptionsGame().getServerName(),
		ScorchedServer::instance()->getOptionsGame().getPublishAddress(),
		uniqueId);
	if (!ComsMessageSender::sendToSingleClient(acceptMessage, destinationId))
	{
		Logger::log(0,
			"ERROR: Failed to send accept to client \"%i\"",
			destinationId);
		ServerCommon::kickDestination(destinationId);
		return true;
	}

	// Send all current tanks to the new client
	std::map<unsigned int, Tank *>::iterator itor;
	std::map<unsigned int, Tank *> &tankList = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	for (itor = tankList.begin();
		itor != tankList.end();
		itor++)
	{
		Tank *tank = (*itor).second;

		ComsAddPlayerMessage oldPlayerMessage(
			tank->getPlayerId(),
			tank->getName(),
			tank->getColor(),
			tank->getModel().getModelName(),
			tank->getDestinationId(),
			tank->getTeam()); 
		oldPlayerMessage.setPlayerIconName(tank->getAvatar().getName());
		oldPlayerMessage.getPlayerIcon().addDataToBuffer(
			tank->getAvatar().getFile().getBuffer(),
			tank->getAvatar().getFile().getBufferUsed());
		ComsMessageSender::sendToSingleClient(oldPlayerMessage, destinationId);
	}

	// Add all the new tanks
	for (unsigned int i=0; i<message.getNoPlayers(); i++)
	{
		addNextTank(destinationId,
			ipAddress,	
			uniqueId,
			message.getHostDesc(),
			false);
	}

	// For the single player game
	// Add a spectator that will always remain a spectator
	// this is so if we only have computer players we still
	// send messages to them
	if (!OptionsParam::instance()->getDedicatedServer())
	{
		addNextTank(destinationId,
			ipAddress,
			uniqueId,
			message.getHostDesc(),
			true);
	}

	return true;
}

void ServerConnectHandler::addNextTank(unsigned int destinationId,
	unsigned int ipAddress,
	const char *sentUniqueId,
	const char *sentHostDesc,
	bool extraSpectator)
{
	// The player has connected
	Vector color;
	unsigned int tankId = 0;
	const char *playerName = "";
	TankModelId modelId("Random");
	if (extraSpectator)
	{
		tankId = 1;
		playerName = "Spectator";
		color = Vector(0.7f, 0.7f, 0.7f);
	}
	else
	{
		playerName = TankAIStrings::instance()->getPlayerName();
		color = TankColorGenerator::instance()->getNextColor();
		// Get the next available tankId
		tankId = TankAIAdder::getNextTankId();
		while (ScorchedServer::instance()->getTankContainer().getTankById(tankId))
		{
			tankId = TankAIAdder::getNextTankId();
		}
	}

	// Make sure host desc does not contain \"
	for (char *h=(char *)sentHostDesc; *h; h++) if (*h == '\"') *h=' ';
	
	// Create the new tank 
	Tank *tank = new Tank(
		ScorchedServer::instance()->getContext(),
		tankId,
		destinationId,
		playerName,
		color,
		modelId);
	tank->setUniqueId(sentUniqueId);
	tank->setIpAddress(ipAddress);
	tank->setHostDesc(sentHostDesc);
	tank->getState().setSpectator(true);
	tank->getState().setLoading(true);

	// Check if this is a bot
	// if not update from any residual tank we have.
	// Residual tanks are only available until the next
	// whole game starts.
	if (destinationId != 0)
	{
		Tank *savedTank = ScorchedServer::instance()->
			getTankDeadContainer().getTank(sentUniqueId);
		if (savedTank)
		{
			Logger::log(0, "Found residual player info");
			NetBufferDefault::defaultBuffer.reset();
			if (savedTank->getAccessories().writeMessage(
					NetBufferDefault::defaultBuffer) &&
				savedTank->getScore().writeMessage(
					NetBufferDefault::defaultBuffer))
			{
				NetBufferReader reader(NetBufferDefault::defaultBuffer);
				if (!tank->getAccessories().readMessage(reader) ||
					!tank->getScore().readMessage(reader))
				{
					Logger::log(0, "Failed to update residual player info (read)");
				}
			}
			else 
			{
				Logger::log(0, "Failed to update residual player info (write)");
			}
			delete savedTank;
		}
	}

	// Add the tank to the list of tanks
	ScorchedServer::instance()->getTankContainer().addTank(tank);

	// Tell the clients to create this tank
	ComsAddPlayerMessage addPlayerMessage(
		tank->getPlayerId(),
		tank->getName(),
		tank->getColor(),
		tank->getModel().getModelName(),
		tank->getDestinationId(),
		tank->getTeam());
	ComsMessageSender::sendToAllConnectedClients(addPlayerMessage);

	// Tell this computer that a new tank has connected
	if (OptionsParam::instance()->getDedicatedServer())
	{
		// Add to dialog
		Logger::log(0, "Player connected dest=\"%i\" id=\"%i\" name=\"%s\" unique=[%s]",
			tank->getDestinationId(),
			tank->getPlayerId(),
			tank->getName(),
			tank->getUniqueId());

		ServerCommon::sendString(0, "Player connected \"%s\"",
			playerName);
	}
}
