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


#ifndef SCORCHED_netBufferUtil_h
#define SCORCHED_netBufferUtil_h

#include <coms/NetMessage.h>

class NetBufferUtil
{
public:
	static bool sendBuffer(NetBuffer &buffer, TCPsocket &socket);
	static NetMessage *readBuffer(TCPsocket &socket);

	static unsigned int getBytesIn() { return bytesIn_; }
	static unsigned int getBytesOut() { return bytesOut_; }

protected:
	static unsigned int bytesIn_;
	static unsigned int bytesOut_;

private:
	NetBufferUtil();
	virtual ~NetBufferUtil();

	NetBufferUtil(const NetBufferUtil &);
	const NetBufferUtil & operator=(const NetBufferUtil &);

};

#endif // SCORCHED_netBufferUtil_h

