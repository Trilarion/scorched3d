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

#include <coms/ComsChannelMessage.h>

ComsChannelMessage::ComsChannelMessage() :
	ComsMessage("ComsChannelMessage")
{
}

ComsChannelMessage::ComsChannelMessage(RequestType type, unsigned int id) :
	ComsMessage("ComsChannelMessage"),
	type_(type), id_(id)
{
}

ComsChannelMessage::~ComsChannelMessage()
{
}

bool ComsChannelMessage::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) type_);
	buffer.addToBuffer(id_);
	buffer.addToBuffer((int) channels_.size());
	std::list<std::string>::iterator itor;
	for (itor = channels_.begin();
		itor != channels_.end();
		itor++)
	{
		buffer.addToBuffer(*itor);
	}
	buffer.addToBuffer((int) availableChannels_.size());
	for (itor = availableChannels_.begin();
		itor != availableChannels_.end();
		itor++)
	{
		buffer.addToBuffer(*itor);
	}
	return true;
}

bool ComsChannelMessage::readMessage(NetBufferReader &reader)
{
	std::string channel;
	int type, size;
	if (!reader.getFromBuffer(type)) return false;
	type_ = (RequestType) type;
	if (!reader.getFromBuffer(id_)) return false;
	if (!reader.getFromBuffer(size)) return false;
	for (int s=0; s<size; s++)
	{
		if (!reader.getFromBuffer(channel)) return false;
		channels_.push_back(channel);
	}
	if (!reader.getFromBuffer(size)) return false;
	for (int s=0; s<size; s++)
	{
		if (!reader.getFromBuffer(channel)) return false;
		availableChannels_.push_back(channel);
	}
	return true;
}
