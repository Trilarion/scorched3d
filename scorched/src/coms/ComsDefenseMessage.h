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


#if !defined(__INCLUDE_ComsDefenseMessageh_INCLUDE__)
#define __INCLUDE_ComsDefenseMessageh_INCLUDE__

#include <coms/ComsMessage.h>

class ComsDefenseMessage : public ComsMessage
{
public:
	enum DefenseChange
	{
		eNoChange,
		eShieldUp,
		eShieldDown,
		eParachutesUp,
		eParachutesDown,
		eBatteryUse
	};

	ComsDefenseMessage(unsigned int playerId = 0,
		DefenseChange change = eNoChange,
		const char *info = "");
	virtual ~ComsDefenseMessage();

	void setPlayerId(unsigned int id) { playerId_ = id; }
	unsigned int getPlayerId() { return playerId_; }
	DefenseChange getChange() { return change_; }
	const char *getInfo() { return info_.c_str(); }

	// Inherited from ComsMessage
    virtual bool writeMessage(NetBuffer &buffer);
    virtual bool readMessage(NetBufferReader &reader);

protected:
	unsigned int playerId_;
	std::string info_;
	DefenseChange change_;

private:
	ComsDefenseMessage(const ComsDefenseMessage &);
	const ComsDefenseMessage & operator=(const ComsDefenseMessage &);
};


#endif
