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

#if !defined(__INCLUDE_LandscapeDefinitionsh_INCLUDE__)
#define __INCLUDE_LandscapeDefinitionsh_INCLUDE__

#include <landscape/LandscapeDefinition.h>
#include <landscape/LandscapeDefinitionsItem.h>
#include <XML/XMLFile.h>
#include <string>
#include <vector>
#include <list>
#include <map>

class LandscapeDefinitions;
class LandscapePlace;
class LandscapeSound;
class LandscapeDefn;
class LandscapeTex;
class OptionsGame;

class LandscapeDefinitionsEntry
{
public:
	std::string name; 
	std::vector<std::string> texs; 
	std::vector<std::string> defns; 
	float weight; // The posibility this defn will be choosen
	std::string description;  // Description of this landscape definition type
	std::string picture; // Visible view of this landscape definition type

	virtual bool readXML(LandscapeDefinitions *definitions, XMLNode *node);
};

class LandscapeDefinitions
{
public:
	LandscapeDefinitions();
	virtual ~LandscapeDefinitions();

	bool readLandscapeDefinitions();
	void clearLandscapeDefinitions();

	LandscapeDefinition getRandomLandscapeDefn(OptionsGame &context);
	LandscapeTex *getTex(const char *file, bool load = false);
	LandscapeDefn *getDefn(const char *file, bool load = false);
	LandscapePlace *getPlace(const char *file, bool load = false);
	LandscapeSound *getSound(const char *file, bool load = false);

	bool landscapeEnabled(OptionsGame &context, const char *name);
	std::list<LandscapeDefinitionsEntry> &getAllLandscapes() 
		{ return entries_; }

protected:
	std::list<LandscapeDefinitionsEntry> entries_;
	LandscapeDefinitionsItem<LandscapeTex> texs_;
	LandscapeDefinitionsItem<LandscapeDefn> defns_;
	LandscapeDefinitionsItem<LandscapePlace> places_;
	LandscapeDefinitionsItem<LandscapeSound> sounds_;

};

#endif
