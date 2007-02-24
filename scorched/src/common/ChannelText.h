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

#if !defined(__INCLUDE_ChannelTexth_INCLUDE__)
#define __INCLUDE_ChannelTexth_INCLUDE__

#include <string>
#include <net/NetBuffer.h>

class ChannelDefinition
{
public:
	enum ChannelDefinitionTypes
	{
		eReadOnlyChannel = 1,
		eWhisperChannel = 2
	};

	ChannelDefinition(const char *channel = "", 
		unsigned int type = 0);

	const char *getChannel() { return channel_.c_str(); }
	void setChannel(const char *c) { channel_ = c; }

	unsigned int getType() { return type_; }
	void setType(unsigned int t) { type_ = t; }

protected:
	std::string channel_;
	unsigned int type_;
};

class ChannelText
{
public:
	ChannelText(
		const char *channel = "", 
		const char *message = "");

	void setChannel(const char *channel) { channel_ = channel; }
	void setMessage(const char *message) { message_ = message; }
	void setSrcPlayerId(unsigned int srcPlayerId) { srcPlayerId_ = srcPlayerId; }
	void setDestPlayerId(unsigned int destPlayerId) { destPlayerId_ = destPlayerId; }

	const char *getChannel() { return channel_.c_str(); }
	const char *getMessage() { return message_.c_str(); }
    unsigned int getSrcPlayerId() { return srcPlayerId_; }
	unsigned int getDestPlayerId() { return destPlayerId_; }

    bool writeMessage(NetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

protected:
	unsigned int srcPlayerId_;
	unsigned int destPlayerId_;
	std::string channel_;
	std::string message_;
};

#endif // __INCLUDE_ChannelTexth_INCLUDE__