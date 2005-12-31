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

#include <landscape/LandscapeDefinitions.h>
#include <landscape/LandscapeTex.h>
#include <landscape/LandscapeDefn.h>
#include <landscape/LandscapePlace.h>
#include <landscape/LandscapeSound.h>
#include <common/OptionsGame.h>
#include <common/OptionsParam.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

bool LandscapeDefinitionsEntry::readXML(LandscapeDefinitions *definitions, XMLNode *node)
{
	if (!node->getNamedChild("name", name)) return false;
	if (!node->getNamedChild("weight", weight)) return false;
	if (!node->getNamedChild("description", description)) return false;
	if (!node->getNamedChild("picture", picture)) return false;

	XMLNode *tex, *defn, *tmp;
	if (!node->getNamedChild("defn", defn)) return false;
	while (defn->getNamedChild("item", tmp, false, true))
	{
		const char *landscapeDefnFile = tmp->getContent();
		LandscapeDefn *landscapeDefn = 
			definitions->getDefn(landscapeDefnFile, true);
		if (!landscapeDefn) return false;
		defns.push_back(landscapeDefnFile);
	}
	if (!node->getNamedChild("tex", tex)) return false;
	while (tex->getNamedChild("item", tmp, false, true))
	{
		const char *landscapeTexFile = tmp->getContent();
		LandscapeTex *landscapeTex = 
			definitions->getTex(landscapeTexFile, true);
		if (!landscapeTex) return false;
		texs.push_back(landscapeTexFile);
	}

	DIALOG_ASSERT(!texs.empty() && !defns.empty());
	return node->failChildren();
}

LandscapeDefinitions::LandscapeDefinitions()
{
}

LandscapeDefinitions::~LandscapeDefinitions()
{
}

void LandscapeDefinitions::clearLandscapeDefinitions()
{
	entries_.clear();
	defns_.clearItems();
	texs_.clearItems();
	sounds_.clearItems();
	places_.clearItems();
}

LandscapeTex *LandscapeDefinitions::getTex(const char *file, bool load)
{
	return texs_.getItem(this, file, load);
}

LandscapePlace *LandscapeDefinitions::getPlace(const char *file, bool load)
{
	return places_.getItem(this, file, load);
}

LandscapeSound *LandscapeDefinitions::getSound(const char *file, bool load)
{
	return sounds_.getItem(this, file, load);
}

LandscapeDefn *LandscapeDefinitions::getDefn(const char *file, bool load)
{
	return defns_.getItem(this, file, load);
}

bool LandscapeDefinitions::readLandscapeDefinitions()
{
	// Load landscape definition file
	XMLFile file;
	if (!file.readFile(getDataFile("data/landscapes.xml")) ||
		!file.getRootNode())
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to parse \"data/landscapes.xml\"\n%s", 
					  file.getParserError());
		return false;
	}

	// Itterate all of the landscapes in the file
	std::list<XMLNode *>::iterator childrenItor;
		std::list<XMLNode *> &children = file.getRootNode()->getChildren();
	for (childrenItor = children.begin();
		childrenItor != children.end();
		childrenItor++)
	{
		LandscapeDefinitionsEntry newDefn;
		if (!newDefn.readXML(this, *childrenItor)) return false;
		entries_.push_back(newDefn);
	}
	return true;
}

bool LandscapeDefinitions::landscapeEnabled(OptionsGame &context, 
											const char *name)
{
	std::string landscapes = context.getLandscapes();
	if (landscapes.empty()) return true; // Default un-initialized state

	char *token = strtok((char *) landscapes.c_str(), ":");
	while(token != 0)
	{
		if (0 == strcmp(token, name)) return true;
		token = strtok(0, ":");
	}
	return false;
}

LandscapeDefinition LandscapeDefinitions::getRandomLandscapeDefn(
	OptionsGame &context)
{
	// Build a list of the maps that are enabled
	float totalWeight = 0.0f;
	std::list<LandscapeDefinitionsEntry *> passedLandscapes;
	std::list<LandscapeDefinitionsEntry>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		itor++)
	{
		LandscapeDefinitionsEntry &result = *itor;
		if (landscapeEnabled(context, result.name.c_str()))
		{
			passedLandscapes.push_back(&result);
			totalWeight += result.weight;
		}
	}

	// Check we have a least one map
	if (passedLandscapes.empty())
	{
		dialogExit("Scorched3D",
			"No existing landscapes are enabled (Landscapes : %s)",
			context.getLandscapes());
	}

	// Map cycle mode
	LandscapeDefinitionsEntry *result = 0;
	if (context.getCycleMaps())
	{
		// Just cycle through the maps
		static LandscapeDefinitionsEntry* last = 0;
		bool next = false;
		result = passedLandscapes.front();

		std::list<LandscapeDefinitionsEntry*>::iterator passedItor;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			passedItor++)
		{
			LandscapeDefinitionsEntry *current = *passedItor;
			if (next) 
			{
				result = current;
				break;
			}
			if (current == last) next = true;
		}
		last = result;
	}
	else
	{
		// Choose a map based on probablity
		float pos = RAND * totalWeight;
		float soFar = 0.0f;

		std::list<LandscapeDefinitionsEntry*>::iterator passedItor;
		for (passedItor = passedLandscapes.begin();
			passedItor != passedLandscapes.end();
			passedItor++)
		{
			LandscapeDefinitionsEntry *current = *passedItor;
			soFar += current->weight;

			if (pos <= soFar)
			{
				result = current;
				break;
			}
		}
	}

	// Check we found map
	if (!result)
	{
		dialogExit("Scorched3D",
			"Failed to select a landscape definition");
	}

	// Return the chosen definition
	unsigned int texPos = rand() % result->texs.size();
	unsigned int defnPos = rand() % result->defns.size();
	DIALOG_ASSERT(texPos >=0 && texPos < result->texs.size());
	DIALOG_ASSERT(defnPos >= 0 && defnPos < result->defns.size());
	std::string tex = result->texs[texPos].c_str();
	std::string defn = result->defns[defnPos].c_str();
	unsigned int seed = (unsigned int) rand();
	if (OptionsParam::instance()->getSeed() != 0)
	{
		seed = (unsigned int) OptionsParam::instance()->getSeed();
	}

	LandscapeDefinition entry(
			tex.c_str(), defn.c_str(), seed);
	return entry;
}

