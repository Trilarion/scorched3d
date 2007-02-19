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

#if !defined(__INCLUDE_ServerChannelManagerh_INCLUDE__)
#define __INCLUDE_ServerChannelManagerh_INCLUDE__

#include <coms/ComsMessageHandler.h>
#include <server/ServerChannelFilter.h>
#include <set>
#include <map>

class ServerChannelManager : 
	public ComsMessageHandlerI
{
public:
	static ServerChannelManager *instance();

	void sendText(const ChannelText &text);
	void sendText(const ChannelText &text, unsigned int destination);
	std::list<std::string> &getLastMessages() { return lastMessages_; }

	// Notification of when players disconnect
	void destinationDisconnected(unsigned int destinationId);

	// Inherited from ComsMessageHandlerI
	virtual bool processMessage(
		NetMessage &message,
		const char *messageType,
		NetBufferReader &reader);

protected:
	static ServerChannelManager *instance_;

	class DestinationLocalEntry
	{
	public:
		DestinationLocalEntry(unsigned int localId = 0);

		unsigned int getLocalId() { return localId_; }
		std::set<std::string> &getChannels() { return channels_; }

	protected:
		unsigned int localId_;
		std::set<std::string> channels_;
	};
	class DestinationEntry 
	{
	public:
		DestinationEntry(unsigned int destinationId);

		unsigned int getDestinationId() { return destinationId_; }

		bool hasChannel(const char *channel);
		void addChannel(const char *channel, unsigned int localId);
		void removeChannel(const char *channel, unsigned int localId);

		bool hasLocalId(unsigned int localId);
		void addLocalId(unsigned int localId);
		void removeLocalId(unsigned int localId);

		void getLocalIds(const char *channel, std::list<unsigned int> &ids);

	protected:
		unsigned int destinationId_;
		std::set<std::string> channels_;
		std::map<unsigned int, DestinationLocalEntry> localEntries_;

		void updateChannels();
	};
	class ChannelEntry
	{
	public:
		ChannelEntry(const char *channelName,
			ServerChannelFilter *filter = 0);
		virtual ~ChannelEntry();

		const char *getName() { return channelName_.c_str(); }
		ServerChannelFilter *getFilter() { return filter_; }

	protected:
		std::string channelName_;
		ServerChannelFilter *filter_;
	};

	std::map<unsigned int, DestinationEntry *> destinationEntries_;
	std::list<ChannelEntry *> channelEntries_;
	std::list<std::string> lastMessages_;

	ChannelEntry *getChannelEntryByName(const char *name);
	DestinationEntry *getDestinationEntryById(unsigned int destinationId);

	void registerClient(unsigned int destinationId, unsigned int localId,
		std::list<std::string> &startChannels);
	void deregisterClient(unsigned int destinationId, unsigned int localId);
	void joinClient(unsigned int destinationId, unsigned int localId,
		std::list<std::string> &startChannels);
	void actualSend(const ChannelText &constText,
		std::map<unsigned int, DestinationEntry *> &destinations);

private:
	ServerChannelManager();
	virtual ~ServerChannelManager();

};

#endif
