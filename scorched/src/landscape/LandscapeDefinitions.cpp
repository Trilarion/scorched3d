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
#include <common/OptionsGame.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

bool LandscapeDefinitionsEntry::readXML(XMLNode *node)
{
	if (!node->getNamedString("name", name)) return false;
	if (!node->getNamedFloat("weight", weight)) return false;
	if (!node->getNamedString("description", description)) return false;
	if (!node->getNamedString("picture", picture)) return false;

	XMLNode *tex, *defn, *tmp;
	if (!node->getNamedChild("defn", defn)) return false;
	while (defn->getNamedChild("item", tmp, false, true))
		defns.push_back(tmp->getContent());
	if (!node->getNamedChild("tex", tex)) return false;
	while (tex->getNamedChild("item", tmp, false, true))
		texs.push_back(tmp->getContent());

	DIALOG_ASSERT(!texs.empty() && !defns.empty());
	return node->failChildren();
}

LandscapeDefinitions *LandscapeDefinitions::instance_ = 0;

LandscapeDefinitions *LandscapeDefinitions::instance()
{
	if (!instance_)	
	{
		instance_ = new LandscapeDefinitions;
	}
	return instance_;
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
	while (!texs_.empty()) 
	{
		delete texs_.front();
		texs_.pop_front();
	}
	while (!defns_.empty())
	{
		delete defns_.front();
		defns_.pop_front();
	}
}

bool LandscapeDefinitions::readLandscapeDefinitions()
{
	if (!readTexs()) return false;
	if (!readDefns()) return false;
	if (!readDefinitions()) return false;
	return true;
}

bool LandscapeDefinitions::readTexs()
{
	// Load landscape definition file
	XMLFile file;
	if (!file.readFile(getDataFile("data/landscapestex.xml")) ||
		!file.getRootNode())
	{
		dialogMessage("Scorched Landscape",
			"Failed to parse \"data/landscapestex.xml\"\n%s",
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
		LandscapeTex *newTex = new LandscapeTex;
		if (!newTex->readXML(*childrenItor))
		{
			dialogMessage("Scorched Landscape",
				"Failed to parse  \"data/landscapestex.xml\"");
			return false;
		}
		texs_.push_back(newTex);
	}
	return true;
}

LandscapeTex *LandscapeDefinitions::getTex(const char *name)
{
	std::list<LandscapeTex*>::iterator itor;
	for (itor = texs_.begin();
		itor != texs_.end();
		itor++)
	{
		LandscapeTex *tex = (*itor);
		if (0 == strcmp(tex->name.c_str(), name)) return tex;
	}
	return 0;
}

bool LandscapeDefinitions::readDefns()
{
	// Load landscape definition file
	XMLFile file;
	if (!file.readFile(getDataFile("data/landscapesdefn.xml")) ||
		!file.getRootNode())
	{
		dialogMessage("Scorched Landscape",
			"Failed to parse \"data/landscapesdefn.xml\"\n%s",
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
		LandscapeDefn *newDefn = new LandscapeDefn;
		if (!newDefn->readXML(*childrenItor))
		{
			dialogMessage("Scorched Landscape",
				"Failed to parse  \"data/landscapesdefn.xml\"");
			return false;
		}
		defns_.push_back(newDefn);
	}
	return true;
}

LandscapeDefn *LandscapeDefinitions::getDefn(const char *name)
{
	std::list<LandscapeDefn *>::iterator itor;
	for (itor = defns_.begin();
		itor != defns_.end();
		itor++)
	{
		LandscapeDefn *defn = *itor;
		if (0 == strcmp(defn->name.c_str(), name)) return defn;
	}
	return 0;
}

bool LandscapeDefinitions::readDefinitions()
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
		if (!newDefn.readXML(*childrenItor))
		{
			dialogMessage("Scorched Landscape", 
					  "Failed to parse \"data/landscapes.xml\"");
			return false;
		}
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

LandscapeDefinition *LandscapeDefinitions::getRandomLandscapeDefn(
	OptionsGame &context)
{
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

	float pos = RAND * totalWeight;
	float soFar = 0.0f;

	std::list<LandscapeDefinitionsEntry*>::iterator passedItor;
	for (passedItor = passedLandscapes.begin();
		passedItor != passedLandscapes.end();
		passedItor++)
	{
		LandscapeDefinitionsEntry *result = *passedItor;
		soFar += result->weight;

		if (pos <= soFar)
		{
			int texPos = int(RAND * float(result->texs.size()));
			int defnPos = int(RAND * float(result->defns.size()));
			DIALOG_ASSERT(texPos < result->texs.size());
			DIALOG_ASSERT(defnPos < result->defns.size());
			LandscapeTex *tex = getTex(result->texs[texPos].c_str());
			LandscapeDefn *defn = getDefn(result->defns[defnPos].c_str());
			DIALOG_ASSERT(tex && defn);
			unsigned int seed = (unsigned int) rand();
			LandscapeDefinition *entry = 
				new LandscapeDefinition(tex, defn, seed);
			return entry;
		}

	}

	return 0;
}

