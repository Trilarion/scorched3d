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

#include <server/ServerChannelManager.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerCommon.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsChannelMessage.h>
#include <coms/ComsChannelTextMessage.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>

ServerChannelManager::ChannelEntry::ChannelEntry(
	ChannelDefinition def,
	ServerChannelFilter *filter,
	ServerChannelAuth *auth) :
	channelDef_(def),
	filter_(filter),
	auth_(auth)
{
}

ServerChannelManager::ChannelEntry::~ChannelEntry()
{
}

ServerChannelManager::DestinationLocalEntry::DestinationLocalEntry(
	unsigned int localId) :
	localId_(localId)
{
}

ServerChannelManager::DestinationEntry::DestinationEntry(
	unsigned int destinationId) :
	destinationId_(destinationId),
	messageCount_(0), muteTime_(0)
{
}

bool ServerChannelManager::DestinationEntry::hasChannel(const char *channel) 
{ 
	return (channels_.find(channel) != channels_.end()); 
}

void ServerChannelManager::DestinationEntry::addChannel(const char *channel, unsigned int localId, bool current)
{
	if (!hasLocalId(localId)) return;

	if (current)
	{
		localEntries_[localId].getChannels().insert(channel);
		localEntries_[localId].getAvailableChannels().erase(channel);
	}
	else
	{
		localEntries_[localId].getChannels().erase(channel);
		localEntries_[localId].getAvailableChannels().insert(channel);
	}

	updateChannels();
}

void ServerChannelManager::DestinationEntry::removeChannel(const char *channel, unsigned int localId)
{
	if (!hasLocalId(localId)) return;
	localEntries_[localId].getChannels().erase(channel);
	localEntries_[localId].getAvailableChannels().erase(channel);
	updateChannels();
}

void ServerChannelManager::DestinationEntry::getLocalIds(const char *channel, std::list<unsigned int> &ids)
{
	std::map<unsigned int, DestinationLocalEntry>::iterator itor;
	for (itor = localEntries_.begin();
		itor != localEntries_.end();
		itor++)
	{
		DestinationLocalEntry &entry = (*itor).second;
		if (entry.getChannels().find(channel) != entry.getChannels().end())
		{
			ids.push_back((*itor).first);
		}
	}
}

bool ServerChannelManager::DestinationEntry::hasLocalId(unsigned int localId)
{
	return (localEntries_.find(localId) != localEntries_.end());
}

void ServerChannelManager::DestinationEntry::addLocalId(unsigned int localId)
{
	if (hasLocalId(localId)) return;
	localEntries_[localId] = DestinationLocalEntry(localId);
	updateChannels();
}

void ServerChannelManager::DestinationEntry::removeLocalId(unsigned int localId)
{
	if (!hasLocalId(localId)) return;
	localEntries_.erase(localId);
	updateChannels();
}

void ServerChannelManager::DestinationEntry::updateChannels()
{
	channels_.clear();
	std::map<unsigned int, DestinationLocalEntry>::iterator itor;
	for (itor = localEntries_.begin();
		itor != localEntries_.end();
		itor++)
	{
		DestinationLocalEntry &entry = (*itor).second;
		channels_.insert(entry.getChannels().begin(), entry.getChannels().end());
	}
}

ServerChannelManager *ServerChannelManager::instance_ = 0;

ServerChannelManager *ServerChannelManager::instance()
{
	if (!instance_)
	{
		instance_ = new ServerChannelManager;
	}
	return instance_;
}

ServerChannelManager::ServerChannelManager() :
	totalTime_(0.0f), lastMessageId_(0)
{
	// Register to recieve comms messages
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsChannelMessage",
		this);
	ScorchedServer::instance()->getComsMessageHandler().addHandler(
		"ComsChannelTextMessage",
		this);

	// Create some default channels
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("info", ChannelDefinition::eReadOnlyChannel)));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("general")));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("team"), 
		new ServerChannelFilterTeams()));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("spam")));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("combat", ChannelDefinition::eReadOnlyChannel)));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("banner", ChannelDefinition::eReadOnlyChannel)));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("admin"),
		0, 
		new ServerChannelAuthAdmin()));
	channelEntries_.push_back(new ChannelEntry(
		ChannelDefinition("whisper", ChannelDefinition::eWhisperChannel)));
}

ServerChannelManager::~ServerChannelManager()
{
}

void ServerChannelManager::simulate(float frameTime)
{
	const int MuteTime = 60;
	const int MuteThreshold = 10;

	totalTime_ += frameTime;
	if (totalTime_ > 10.0f)
	{
		totalTime_ = 0.0f;

		// Check each destination for the amount of messages they have
		// sent recently
		// If its more than a certain threshold, mute them for x seconds
		std::map<unsigned int, DestinationEntry *>::iterator itor;
		for (itor = destinationEntries_.begin();
			itor != destinationEntries_.end();
			itor++)
		{
			DestinationEntry *entry = itor->second;
			if (entry->getMessageCount() > MuteThreshold)
			{
				time_t t = time(0);
				entry->setMuteTime(t);

				ChannelText text("info", 
					formatString("You have been muted for %i seconds for spamming!",
					MuteTime));
				sendText(text, entry->getDestinationId());
			}
			else if (entry->getMuteTime())
			{
				time_t t = time(0);
				if (t - entry->getMuteTime() > MuteTime)
				{
					entry->setMuteTime(0);

					ChannelText text("info", "You have been unmuted.");
					sendText(text, entry->getDestinationId());
				}
			}
			entry->setMessageCount(0);
		}
	}
}

void ServerChannelManager::destinationDisconnected(unsigned int destinationId)
{
	DestinationEntry *entry = getDestinationEntryById(destinationId);
	if (!entry) return;

	destinationEntries_.erase(destinationId);
	delete entry;
}

ServerChannelManager::ChannelEntry *ServerChannelManager::getChannelEntryByName(const char *name)
{
	std::list<ChannelEntry *>::iterator itor;
	for (itor = channelEntries_.begin();
		itor != channelEntries_.end();
		itor++)
	{
		ChannelEntry *entry = *itor;
		if (0 == strcmp(entry->getName(), name)) return entry;
	}
	return 0;
}

ServerChannelManager::DestinationEntry *ServerChannelManager::getDestinationEntryById(
	unsigned int destinationId)
{
	std::map<unsigned int, DestinationEntry *>::iterator findItor = 
		destinationEntries_.find(destinationId);
	if (findItor == destinationEntries_.end()) return 0;

	return (*findItor).second;
}

std::list<std::string> ServerChannelManager::getAllChannels()
{
	std::list<std::string> result;
	std::list<ChannelEntry *>::iterator itor;
	for (itor = channelEntries_.begin();
		itor != channelEntries_.end();
		itor++)
	{
		ChannelEntry *channelEntry = (*itor);
		result.push_back(channelEntry->getName());
	}
	return result;
}

void ServerChannelManager::registerClient(unsigned int destinationId, unsigned int localId,
	std::list<ChannelDefinition> &startChannels)
{
	// Get the sender for this message
	DestinationEntry *destEntry = getDestinationEntryById(destinationId);
	if (destEntry && destEntry->hasLocalId(localId)) return; // Already an entry

	// Add the new entry
	if (!destEntry)
	{
		destEntry = new DestinationEntry(destinationId);
		destinationEntries_[destinationId] = destEntry;
	}
	// Add the new local entry
	if (!destEntry->hasLocalId(localId))
	{
		destEntry->addLocalId(localId);
	}

	// Join the client to the new channels
	joinClient(destinationId, localId, startChannels);
}

void ServerChannelManager::deregisterClient(unsigned int destinationId, unsigned int localId)
{
	// Get the sender for this message
	DestinationEntry *destEntry =  getDestinationEntryById(destinationId);
	if (!destEntry || !destEntry->hasLocalId(localId)) return; // No such user

	// Remove the entry
	destEntry->removeLocalId(localId);
}

void ServerChannelManager::refreshDestination(unsigned int destinationId)
{
	// Get the sender for this message
	DestinationEntry *destEntry = getDestinationEntryById(destinationId);

	// Form the list of available and joined channels
	// for each local id and send them back to the client
	std::map<unsigned int, DestinationLocalEntry> &localEntries = 
		destEntry->getLocalEntries();
	std::map<unsigned int, DestinationLocalEntry>::iterator localItor;
	for (localItor = localEntries.begin();
		localItor != localEntries.end();
		localItor++)
	{
		unsigned int localId = localItor->first;
		DestinationLocalEntry &localEntry = localItor->second;

		ComsChannelMessage message(ComsChannelMessage::eJoinRequest, localId);
		bool differentMessage = false;
		std::list<ChannelEntry *>::iterator itor;
		for (itor = channelEntries_.begin();
			itor != channelEntries_.end();
			itor++)
		{
			ChannelEntry *channelEntry = (*itor);

			// Check if a user is allowed a channel at all
			bool allowedChannel = (!channelEntry->getAuth() ||
				channelEntry->getAuth()->allowConnection(
				channelEntry->getName(), destinationId));

			// Check if they currently have the channel
			if (localEntry.getChannels().find(channelEntry->getName()) !=
				localEntry.getChannels().end())
			{
				// They currently have the channel in their 
				// current subscription list
				if (allowedChannel)
				{
					// They are allowed this channel
					// Add it to the list of subscribed channels
					message.getChannels().push_back(
						channelEntry->getDefinition());
				}
				else
				{
					// They are not allowed this channel
					// Remove it from their list
					destEntry->removeChannel(channelEntry->getName(), localId);
					differentMessage = true;
				}
			}
			else if (localEntry.getAvailableChannels().find(channelEntry->getName()) !=
				localEntry.getAvailableChannels().end())
			{
				// They currently have their channel in their
				// avialable channel list
				if (allowedChannel)
				{
					// They are allowed this channel
					// Add it to the list of available channels
					message.getAvailableChannels().push_back(
						channelEntry->getDefinition());
				}
				else
				{
					// They are not allowed this channel
					// Remove it from their list
					destEntry->removeChannel(channelEntry->getName(), localId);
					differentMessage = true;
				}
			}
			else
			{
				// This a new channel the user has not seen
				if (allowedChannel)
				{
					// They are allowed this channel
					// Add it to the list of available channels
					message.getAvailableChannels().push_back(
						channelEntry->getDefinition());
					destEntry->addChannel(channelEntry->getName(), localId, false);
					differentMessage = true;
				}
			}
		}

		if (differentMessage)
		{
			// The user's subscriptions have changed
			// Send the new subscriptions
			ComsMessageSender::sendToSingleClient(message, destinationId);
		}
	}
}

void ServerChannelManager::joinClient(unsigned int destinationId, unsigned int localId,
	std::list<ChannelDefinition> &startChannels)
{
	// Get the sender for this message
	DestinationEntry *destEntry = getDestinationEntryById(destinationId);
	if (!destEntry || !destEntry->hasLocalId(localId)) return; // No such user

	// Form the list of available and joined channels
	// and send them back to the client
	ComsChannelMessage message(ComsChannelMessage::eJoinRequest, localId);
	std::list<ChannelEntry *>::iterator itor;
	for (itor = channelEntries_.begin();
		itor != channelEntries_.end();
		itor++)
	{
		ChannelEntry *channelEntry = (*itor);

		// Check if a user is allowed a channel at all
		if (channelEntry->getAuth() &&
			!channelEntry->getAuth()->allowConnection(
			channelEntry->getName(), destinationId))
		{
			destEntry->removeChannel(channelEntry->getName(), localId);
			continue;
		}

		// Check if the user has asked for this channel
		bool current = false;
		std::list<ChannelDefinition>::iterator startItor;
		for (startItor = startChannels.begin();
			startItor != startChannels.end();
			startItor++)
		{
			const char *startChannel = startItor->getChannel();
			if (0 == strcmp(startChannel, channelEntry->getName()))
			{
				current = true;
				break;
			}
		}

		destEntry->addChannel(channelEntry->getName(), localId, current);
		if (current) message.getChannels().push_back(channelEntry->getDefinition());
		else message.getAvailableChannels().push_back(channelEntry->getDefinition());
	}
	ComsMessageSender::sendToSingleClient(message, destinationId);
}

void ServerChannelManager::sendText(const ChannelText &constText, 
	unsigned int destination)
{
	DestinationEntry *destinationEntry = getDestinationEntryById(
		destination);
	if (destinationEntry)
	{
		std::map<unsigned int, DestinationEntry *> destinations;
		destinations[destination] = destinationEntry;
		actualSend(constText, destinations);
	}
}

void ServerChannelManager::sendText(const ChannelText &constText)
{
	actualSend(constText, destinationEntries_);
}

void ServerChannelManager::actualSend(const ChannelText &constText,
	std::map<unsigned int, DestinationEntry *> &destinations)
{
	ChannelText text = constText;

	// Get the tank for this message (if any)
	Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(
		text.getSrcPlayerId());

	// Check that this channel exists
	ChannelEntry *channelEntry = getChannelEntryByName(text.getChannel());
	if (!channelEntry)
	{
		// This channel does not exist
		return;
	}

	// Filter the string for bad language
	std::string filteredText(text.getMessage());
	ScorchedServerUtil::instance()->textFilter.filterString(filteredText);

	// Remove any bad characters
	for (char *r = (char *) filteredText.c_str(); *r; r++)
	{
		if (*r == '%' || *r < 0) *r = ' ';
	}
	for (char *r = (char *) filteredText.c_str(); *r; r++)
	{
		if (*r == '[') *r = '(';
		else if (*r == ']') *r = ')';
	}

	// Update the server console with the say text
	const char *logtext = 0;
	if (tank)
	{
		logtext = formatString("[%s][%s] : \"%s\"", 
			text.getChannel(),
			tank->getName(),
			filteredText.c_str());
	}
	else
	{
		logtext = formatString("[%s] : \"%s\"", 
			text.getChannel(),
			filteredText.c_str());
	}
	ServerCommon::serverLog(logtext);
	MessageEntry messageEntry;
	messageEntry.message = logtext;
	messageEntry.messageid = ++lastMessageId_;
	lastMessages_.push_back(messageEntry);
	if (lastMessages_.size() > 25) lastMessages_.pop_front();

	// Update the message with the filtered text
	text.setMessage(filteredText.c_str());

	// Send to all clients
	std::map<unsigned int, DestinationEntry *>::iterator destItor;
	for (destItor = destinations.begin();
		destItor != destinations.end();
		destItor++)
	{
		DestinationEntry *entry = (*destItor).second;
		if (!channelEntry->getFilter() || channelEntry->getFilter()->sentToDestination(
			text, entry->getDestinationId()))
		{
			if (entry->hasChannel(text.getChannel()))
			{
				ComsChannelTextMessage newTextMessage(text);
				entry->getLocalIds(text.getChannel(), newTextMessage.getIds());
				ComsMessageSender::sendToSingleClient(
					newTextMessage, entry->getDestinationId());
			}
		}
	}
}

bool ServerChannelManager::processMessage(
	NetMessage &netNessage,
	const char *messageType,
	NetBufferReader &reader)
{
	// Check which message we have got
	if (0 == strcmp("ComsChannelMessage", messageType))
	{
		// We have a ChannelMessage from the server
		ComsChannelMessage channelMessage;
		if (!channelMessage.readMessage(reader)) return false;

		// Check which ChannelMessage was sent
		switch (channelMessage.getType())
		{
		case ComsChannelMessage::eRegisterRequest:
			registerClient(netNessage.getDestinationId(), channelMessage.getId(),
				channelMessage.getChannels());
			break;
		case ComsChannelMessage::eDeregisterRequest:
			deregisterClient(netNessage.getDestinationId(), channelMessage.getId());
			break;
		case ComsChannelMessage::eJoinRequest:
			joinClient(netNessage.getDestinationId(), channelMessage.getId(),
				channelMessage.getChannels());
			break;
		}
	}
	else if (0 == strcmp("ComsChannelTextMessage", messageType))
	{
		// We have a ChannelTextMessage from the server
		ComsChannelTextMessage textMessage;
		if (!textMessage.readMessage(reader)) return false;

		// Validate that this message came from this tank
		Tank *tank = ScorchedServer::instance()->getTankContainer().getTankById(
			textMessage.getChannelText().getSrcPlayerId());
		if (!tank || tank->getDestinationId() != 
			netNessage.getDestinationId()) return true;

		// Check this tank has not been admin muted
		if (tank->getState().getMuted()) return true;

		// Check that this client has this channel
		DestinationEntry *destEntry = 
			getDestinationEntryById(netNessage.getDestinationId());
		if (!destEntry || !destEntry->hasChannel(
			textMessage.getChannelText().getChannel()))
		{
			// This client does not have this channel
			return true;
		}

		// Check this tank has not been muted for spamming
		if (destEntry->getMuteTime()) return true;

		// Check that this channel exists and is not a read only channel
		ChannelEntry *channelEntry = getChannelEntryByName(
			textMessage.getChannelText().getChannel());
		if (!channelEntry)
		{
			// This channel does not exist
			return true;
		}
		if (channelEntry->getDefinition().getType() & 
			ChannelDefinition::eReadOnlyChannel)
		{
			// Cannot send to this channel from a client
			return true;
		}
		if ((!(channelEntry->getDefinition().getType() &
			ChannelDefinition::eWhisperChannel) &&
			textMessage.getChannelText().getDestPlayerId()) ||
			
			((channelEntry->getDefinition().getType() &
			ChannelDefinition::eWhisperChannel) &&
			!textMessage.getChannelText().getDestPlayerId()))
		{
			// Cannot send a whisper message to a non-whisper channel
			// or
			// Cannot send a non-whisper message to a whisper channel
			return true;
		}

		// Check that we don't recieve too much text
		if (strlen(textMessage.getChannelText().getMessage()) > 1024) return true;

		// Increment message count
		destEntry->setMessageCount(destEntry->getMessageCount() + 1);

		// Send the text
		if (textMessage.getChannelText().getDestPlayerId())
		{
			// This is a whisper message, check the destination exists
			Tank *destTank = ScorchedServer::instance()->getTankContainer().
				getTankById(textMessage.getChannelText().getDestPlayerId());
			if (destTank && destTank->getDestinationId())
			{
				// Send to this destination
				sendText(textMessage.getChannelText(), 
					destTank->getDestinationId());
			}
		}
		else
		{
			// Send to all destinations
			sendText(textMessage.getChannelText());
		}
	}
	else return false;

	return true;
}
