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

#if !defined(__INCLUDE_ResourceBundleEntryImpl_INCLUDE__)
#define __INCLUDE_ResourceBundleEntryImpl_INCLUDE__

#include <lang/LangStringParameterized.h>
#include <lang/ResourceBundleEntry.h>
#include <vector>

class ResourceBundleEntryImpl : public ResourceBundleEntry
{
public:
	ResourceBundleEntryImpl(const std::string &key);
	ResourceBundleEntryImpl(const std::string &key, 
		const LangString &value);

	virtual const char *getKey() { return key_.c_str(); }
	virtual LangString getValue();

	virtual LangString getString();
	virtual LangString getString(const LangStringConverter &param1);
	virtual LangString getString(const LangStringConverter &param1, 
		const LangStringConverter &param2);
	virtual LangString getString(const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3);
	virtual LangString getString(const LangStringConverter &param1, 
		const LangStringConverter &param2, const LangStringConverter &param3, const LangStringConverter &param4);

private:
	LangStringParameterized parameterizedString_;
	std::string key_;
};

#endif // __INCLUDE_ResourceBundleEntryImpl_INCLUDE__
