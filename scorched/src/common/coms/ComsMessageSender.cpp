////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <coms/ComsMessageSender.h>
#include <coms/ComsMessageHandler.h>
#include <landscapedef/LandscapeDescription.h>
#include <landscapemap/LandscapeMaps.h>
#include <net/NetBuffer.h>
#include <net/NetInterface.h>
#include <net/NetBufferPool.h>
#include <common/Defines.h>
#include <common/Logger.h>
#ifndef S3D_SERVER
#include <client/ScorchedClient.h>
#endif
#include <server/ScorchedServer.h>
#include <server/ServerDestinations.h>
#include <set>
#include <zlib.h>

bool ComsMessageSender::formMessage(NetBuffer &buffer, ComsMessage &message, unsigned int flags)
{
	// Write the message and its type to the buffer
	buffer.reset();
	if (!message.writeTypeMessage(buffer))
	{
		Logger::log( "ERROR: ComsMessageSender - Failed to write message type");
		return false;
	}
	if (!message.writeMessage(buffer))
	{
		Logger::log( "ERROR: ComsMessageSender - Failed to write message");
		return false;
	}

	// Compress the message
#ifdef S3D_SERVER
	if (flags & NetInterfaceFlags::fCompress)
	{
		buffer.compressBuffer();
		buffer.addToBuffer(true);
	}
	else
#endif
	{
		buffer.addToBuffer(false);
	}

	return true;
}

#ifndef S3D_SERVER
bool ComsMessageSender::sendToServer(
	ComsMessage &message, unsigned int flags)
{
	NetBufferPoolReturner defaultBuffer(NetBufferPool::instance()->getFromPool());
	if (!ScorchedClient::instance()->getNetInterfaceValid() ||
		!ScorchedClient::instance()->getNetInterface().started()) return false;
	if (!formMessage(*defaultBuffer.getBuffer(), message, flags)) return false;

	if (ScorchedClient::instance()->getComsMessageHandler().getMessageLogging() &&
		message.getComsMessageType().getLogMessage())
	{
		Logger::log(S3D::formatStringBuffer("Client::send(%s, %u%s)", 
			message.getComsMessageType().getName().c_str(),
			defaultBuffer.getBuffer()->getBufferUsed(),
			(flags & NetInterfaceFlags::fCompress)?", compressed":""));
	}	
	ScorchedClient::instance()->getNetInterface().sendMessageServer(
		*defaultBuffer.getBuffer(), flags);
	return true;
}
#endif

bool ComsMessageSender::sendToMultipleClients(
	ComsMessage &message, std::list<unsigned int> sendDestinations, unsigned int flags)
{
	NetBufferPoolReturner defaultBuffer(NetBufferPool::instance()->getFromPool());
	if (sendDestinations.empty()) return true;
	if (!formMessage(*defaultBuffer.getBuffer(), message, flags)) return false;

	// Used to ensure we only send messages to each
	// destination once
	std::set<unsigned int> destinations;
	std::set<unsigned int>::iterator findItor;
	destinations.insert(0); // Make sure we don't send to dest 0

	std::list<unsigned int>::iterator itor;
	for (itor = sendDestinations.begin();
		itor != sendDestinations.end();
		++itor)
	{
		unsigned int destination = *itor;
		findItor = destinations.find(destination);

		if (findItor == destinations.end())
		{
			destinations.insert(destination);

			if (ScorchedServer::instance()->getComsMessageHandler().getMessageLogging() &&
				message.getComsMessageType().getLogMessage())
			{
				Logger::log(S3D::formatStringBuffer("Server::send(%s, %u, %u%s)", 
					message.getComsMessageType().getName().c_str(),
					destination,
					defaultBuffer.getBuffer()->getBufferUsed(),
					(flags & NetInterfaceFlags::fCompress)?", compressed":""));
			}	
			if (!ScorchedServer::instance()->getNetInterfaceValid() ||
				!ScorchedServer::instance()->getNetInterface().started())
			{
				Logger::log( "ERROR: ComsMessageSender::sendToMultipleClients - Server not started");
				return false;
			}
			ScorchedServer::instance()->getNetInterface().sendMessageDest(
				*defaultBuffer.getBuffer(), destination, flags);
		}
	}

	return true;
}

bool ComsMessageSender::sendToSingleClient(ComsMessage &message,
	unsigned int destination, unsigned int flags)
{
	if (destination == 0) return true;

	std::list<unsigned int> destinations;
	destinations.push_back(destination);
	return sendToMultipleClients(message, destinations, flags);
}

bool ComsMessageSender::sendToAllConnectedClients(
	ComsMessage &message, unsigned int flags)
{
	std::list<unsigned int> destinations;
	std::map<unsigned int, ServerDestination *>::iterator itor;
	std::map<unsigned int, ServerDestination *> dests = 
		ScorchedServer::instance()->getServerDestinations().getServerDestinations();
	for (itor = dests.begin();
		itor != dests.end();
		++itor)
	{
		ServerDestination *destination = (*itor).second;
		destinations.push_back(destination->getDestinationId());
	}
	return sendToMultipleClients(message, destinations, flags);
}

bool ComsMessageSender::sendToAllLoadedClients(
	ComsMessage &message, unsigned int flags)
{
	LandscapeDescription &landscapeDescription =
		ScorchedServer::instance()->getLandscapeMaps().getDescriptions().
		getDescription();

	std::list<unsigned int> destinations;
	std::map<unsigned int, ServerDestination *>::iterator itor;
	std::map<unsigned int, ServerDestination *> dests = 
		ScorchedServer::instance()->getServerDestinations().getServerDestinations();
	for (itor = dests.begin();
		itor != dests.end();
		++itor)
	{
		ServerDestination *destination = itor->second;
		if (destination->getState() == ServerDestination::sFinished ||
			(destination->getState() == ServerDestination::sLoadingLevel &&
			destination->getLevelNumber() == landscapeDescription.getDescriptionNumber()))
		{
			destinations.push_back(destination->getDestinationId());
		}
	}
	return sendToMultipleClients(message, destinations, flags);
}
