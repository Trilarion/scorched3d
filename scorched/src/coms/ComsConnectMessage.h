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

#ifndef _comsConnectMessage_h
#define _comsConnectMessage_h

// The very first message sent from the client to the server
// requesting a connection.
//
// Since version 37 the format of this message has been changed
// it is now sent/serialized as a name/value pair list
// this means we can add any extra args can be added without breaking
// backward compatability

#include <coms/ComsMessage.h>
#include <stdlib.h>
#include <map>
#include <string>

class ComsConnectMessage : public ComsMessage
{
public:
	ComsConnectMessage();
	virtual ~ComsConnectMessage();

	void setVersion(const char *version) { setValue("version", version); }
	void setProtocolVersion(const char *pversion) { setValue("pversion", pversion); }
	void setPassword(const char *password) { setValue("password", password); }
	void setUniqueId(const char *uid) { setValue("uid", uid); }
	void setHostDesc(const char *host) { setValue("host", host); }
	void setNoPlayers(unsigned int players) { 
		char buf[10]; sprintf(buf, "%i", players); setValue("numplayers", buf); }

	const char *getVersion() { return getValue("version"); }
	const char *getProtocolVersion() { return getValue("pversion"); }
	const char *getPassword() { return getValue("password"); }
	const char *getHostDesc() { return getValue("host"); }
	const char *getUniqueId() { return getValue("uid"); }
	unsigned int getNoPlayers() { 
		return (unsigned int) atoi(getValue("numplayers")?getValue("numplayers"):"0"); }

	// Inherited from ComsMessage
	virtual bool writeMessage(NetBuffer &buffer);
	virtual bool readMessage(NetBufferReader &reader);

protected:
	std::map<std::string, std::string> values_;

	void setValue(const char *name, const char *value);
	const char *getValue(const char *name);

private:
	ComsConnectMessage(const ComsConnectMessage &);
	const ComsConnectMessage & operator=(const ComsConnectMessage &);

};

#endif // _comsConnectMessage_h

