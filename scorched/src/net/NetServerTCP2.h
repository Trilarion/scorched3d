////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_NetServerTCP2h_INCLUDE__)
#define __INCLUDE_NetServerTCP2h_INCLUDE__

#include <net/NetServerTCP2Destination.h>
#include <map>

class NetServerTCP2 : 
	public NetInterface,
	public NetMessageHandlerI
{
public:
	NetServerTCP2();
	virtual ~NetServerTCP2();

	// NetInterface
	virtual bool started();
	virtual bool connect(const char *hostName, int portNo);
	virtual bool start(int portNo);
	virtual void stop();

	virtual int processMessages();
	virtual void setMessageHandler(NetMessageHandlerI *handler);

	virtual void disconnectAllClients();
	virtual void disconnectClient(unsigned int client);
	virtual void sendMessageServer(NetBuffer &buffer, 
		unsigned int flags = 0);
	virtual void sendMessageDest(NetBuffer &buffer, 
		unsigned int destination, unsigned int flags = 0);

	// NetMessageHandlerI
	virtual void processMessage(NetMessage &message);

protected:
	friend class NetServerTCP2Destination;

	NetMessageHandler outgoingMessageHandler_;
	NetMessageHandler incomingMessageHandler_;
	TCPsocket serverSock_;
	SDLNet_SocketSet serverSockSet_;
	SDL_Thread *sendRecvThread_;
	bool stopped_;
	std::map<unsigned int, NetServerTCP2Destination *> destinations_;
	std::list<NetServerTCP2Destination *> finishedDestinations_;
	unsigned int serverDestinationId_;
	unsigned int nextDestinationId_;

	void checkNewConnections();
	void checkClients();
	bool startProcessing();

	void actualSendRecvFunc();
	static int sendRecvThreadFunc(void *);

	void destroyDestination(unsigned int destinationId, 
		NetMessage::DisconnectFlags type);
	unsigned int addDestination(TCPsocket &socket);
};

#endif // __INCLUDE_NetServerTCP2h_INCLUDE__
