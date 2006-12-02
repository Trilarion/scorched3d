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

#if !defined(__INCLUDE_ClientParamsh_INCLUDE__)
#define __INCLUDE_ClientParamsh_INCLUDE__

#include <common/OptionsParameters.h>

class ClientParams : public OptionsParameters
{
public:
	static ClientParams *instance();

	const char *getConnect() { return connect_.getValue(); }
	const char *getClientFile() { return client_.getValue(); }
	const char *getSaveFile() { return save_.getValue(); }
	const char *getUserName() { return username_.getValue(); }
	const char *getPassword() { return password_.getValue(); }
	bool getStartCustom() { return startcustom_.getValue(); }
	bool getConnectedToServer() { return (getConnect()[0] != '\0'); }

protected:
	static ClientParams *instance_;

	OptionEntryString connect_;
	OptionEntryString client_;
	OptionEntryString password_;
	OptionEntryString username_;
	OptionEntryString save_;
	OptionEntryBool startcustom_;

private:
	ClientParams();
	virtual ~ClientParams();

};

#endif // __INCLUDE_ClientParamsh_INCLUDE__