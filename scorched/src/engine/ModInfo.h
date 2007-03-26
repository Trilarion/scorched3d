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

#if !defined(__INCLUDE_ModInfoh_INCLUDE__)
#define __INCLUDE_ModInfoh_INCLUDE__

#include <string>
#include <list>

class ModInfo
{
public:
	struct MenuEntry
	{
		std::string icon;
		std::string shortdescription;
		std::string description;
		std::string gamefile;
	};

	ModInfo(const char *name);
	virtual ~ModInfo();

	bool parse(const char *fileName);

	const char *getName() { return name_.c_str(); }
	const char *getUrl() { return url_.c_str(); }
	const char *getIcon() { return icon_.c_str(); }
	const char *getDescription() { return description_.c_str(); }
	const char *getShortDescription() { return shortDescription_.c_str(); }
	const char *getProtocolVersion() { return protocolversion_.c_str(); }
	std::list<MenuEntry> &getMenuEntries() { return entries_; }

protected:
	std::string name_;
	std::string url_;
	std::string icon_;
	std::string description_;
	std::string shortDescription_;
	std::string protocolversion_;
	std::list<MenuEntry> entries_;
};

#endif // __INCLUDE_ModInfoh_INCLUDE__
