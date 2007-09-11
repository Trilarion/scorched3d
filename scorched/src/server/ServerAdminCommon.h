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

#if !defined(__INCLUDE_ServerAdminCommonh_INCLUDE__)
#define __INCLUDE_ServerAdminCommonh_INCLUDE__

#include <common/DefinesString.h>
#include <server/ServerAdminSessions.h>

namespace ServerAdminCommon
{
	bool kickPlayer(const char *adminUser, unsigned int playerId);
	bool poorPlayer(const char *adminUser, unsigned int playerId);
	bool banPlayer(const char *adminUser, unsigned int playerId, const char *reason);
	bool flagPlayer(const char *adminUser, unsigned int playerId, const char *reason);
	bool slapPlayer(const char *adminUser, unsigned int playerId, float slap);
	bool mutePlayer(const char *adminUser, unsigned int playerId, bool mute);
	bool permMutePlayer(const char *adminUser, unsigned int playerId, const char *reason);
	bool unpermMutePlayer(const char *adminUser, unsigned int playerId);
	bool newGame(const char *adminUser);
	bool killAll(const char *adminUser);

	bool adminSay(ServerAdminSessions::SessionParams *session,
		const char *channel, const char *text);
}

#endif
