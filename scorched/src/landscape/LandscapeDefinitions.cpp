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
#include <common/Defines.h>
#include <stdlib.h>
#include <time.h>

bool LandscapeDefinition::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(name);
	buffer.addToBuffer(landSeed);
	buffer.addToBuffer(landHillsMin);
	buffer.addToBuffer(landHillsMax);
	buffer.addToBuffer(landWidthX);
	buffer.addToBuffer(landWidthY);
	buffer.addToBuffer(landHeightMin);
	buffer.addToBuffer(landHeightMax);
	buffer.addToBuffer(landPeakWidthXMin);
	buffer.addToBuffer(landPeakWidthXMax);
	buffer.addToBuffer(landPeakWidthYMin);
	buffer.addToBuffer(landPeakWidthYMax);
	buffer.addToBuffer(landPeakHeightMin);
	buffer.addToBuffer(landPeakHeightMax);
	buffer.addToBuffer(landSmoothing);
	buffer.addToBuffer(tankStartCloseness);
	buffer.addToBuffer(tankStartHeightMin);
	buffer.addToBuffer(tankStartHeightMax);
	buffer.addToBuffer(resourceFile);
	buffer.addToBuffer(heightMapFile);
	buffer.addToBuffer(textureMapFile);

	return true;
}

bool LandscapeDefinition::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(name)) return false;
	if (!reader.getFromBuffer(landSeed)) return false;
	if (!reader.getFromBuffer(landHillsMin)) return false;
	if (!reader.getFromBuffer(landHillsMax)) return false;
	if (!reader.getFromBuffer(landWidthX)) return false;
	if (!reader.getFromBuffer(landWidthY)) return false;
	if (!reader.getFromBuffer(landHeightMin)) return false;
	if (!reader.getFromBuffer(landHeightMax)) return false;
	if (!reader.getFromBuffer(landPeakWidthXMin)) return false;
	if (!reader.getFromBuffer(landPeakWidthXMax)) return false;
	if (!reader.getFromBuffer(landPeakWidthYMin)) return false;
	if (!reader.getFromBuffer(landPeakWidthYMax)) return false;
	if (!reader.getFromBuffer(landPeakHeightMin)) return false;
	if (!reader.getFromBuffer(landPeakHeightMax)) return false;
	if (!reader.getFromBuffer(landSmoothing)) return false;
	if (!reader.getFromBuffer(tankStartCloseness)) return false;
	if (!reader.getFromBuffer(tankStartHeightMin)) return false;
	if (!reader.getFromBuffer(tankStartHeightMax)) return false;
	if (!reader.getFromBuffer(resourceFile)) return false;
	if (!reader.getFromBuffer(heightMapFile)) return false;
	if (!reader.getFromBuffer(textureMapFile)) return false;

	return true;
}

bool LandscapeDefinition::readXML(XMLNode *node)
{
	landSeed = rand();
	XMLNode *nameNode = node->getNamedChild("name");
	if (!nameNode)
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to find name node in \"landscape/landscape.xml\"");
		return false;
	}
	name = nameNode->getContent();

	if ((weight = node->getNamedFloatChild("weight", "landscape/landscape.xml")) == 0.0f) return false;

	XMLNode *heightmapNode = node->getNamedChild("heightmap");
	if (!heightmapNode)
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to find heightmap node in \"landscape/landscape.xml\" %s",
					  nameNode->getContent());
		return false;
	}
	if (!readXMLMinMax(heightmapNode, "landhills", landHillsMin, landHillsMax)) return false;
	if (!readXMLMinMax(heightmapNode, "landheight", landHeightMin, landHeightMax)) return false;
	if ((landWidthX=heightmapNode->getNamedFloatChild("landwidthx", "landscape/landscape.xml")) == 0.0f) return false;
	if ((landWidthY=heightmapNode->getNamedFloatChild("landwidthy", "landscape/landscape.xml")) == 0.0f) return false;
	if (!readXMLMinMax(heightmapNode, "landpeakwidthx", landPeakWidthXMin, landPeakWidthXMax)) return false;
	if (!readXMLMinMax(heightmapNode, "landpeakwidthy", landPeakWidthYMin, landPeakWidthYMax)) return false;
	if (!readXMLMinMax(heightmapNode, "landpeakheight", landPeakHeightMin, landPeakHeightMax)) return false;
	if ((landSmoothing = heightmapNode->getNamedFloatChild("landsmoothing", "landscape/landscape.xml")) == 0.0f) return false;
	if ((tankStartCloseness = heightmapNode->getNamedFloatChild("tankstartcloseness", "landscape/landscape.xml")) == 0.0f) return false;
	if (!readXMLMinMax(heightmapNode, "tankstartheight", tankStartHeightMin, tankStartHeightMax)) return false;

	XMLNode *texturemapNode = node->getNamedChild("texturemap");
	if (!texturemapNode)
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to find texturemap node in \"landscape/landscape.xml\" %s",
					  nameNode->getContent());
		return false;
	}
	XMLNode *resourcesNode = texturemapNode->getNamedChild("resources");
	if (!resourcesNode)
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to find resources node in \"landscape/landscape.xml\" %s",
					  nameNode->getContent());
		return false;
	}

    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = resourcesNode->getChildren();
    for (childrenItor = children.begin();
        childrenItor != children.end();
        childrenItor++)
    {
		XMLNode *child = *childrenItor;
		resourceFile = child->getContent();
		resourceFiles.push_back(child->getContent());
	}
	if (resourceFiles.empty())
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to find any resource files in \"landscape/landscape.xml\"");
		return false;
	}

	return true;
}

bool LandscapeDefinition::readXMLMinMax(XMLNode *node, 
	const char *name, 
	float &min, float &max)
{
	XMLNode *subnode = node->getNamedChild(name);
	if (!subnode)
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to find node in \"landscape/landscape.xml\" %s",
					 name);
		return false;
	}

	min = subnode->getNamedFloatChild("min", "landscape/landscape.xml");
	max = subnode->getNamedFloatChild("max", "landscape/landscape.xml");
	if (min == 0.0f || max == 0.0f) return false;
	return true;
}

void LandscapeDefinition::generate()
{
	srand(time(0));
	landSeed = rand();

	unsigned int pos = (unsigned int)(RAND * float(resourceFiles.size()));
	DIALOG_ASSERT(pos >= 0 && pos < resourceFiles.size());
	resourceFile = resourceFiles[pos];
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

LandscapeDefinitions::LandscapeDefinitions() : totalWeight_(0.0f)
{
}

LandscapeDefinitions::~LandscapeDefinitions()
{
}

bool LandscapeDefinitions::readLandscapeDefinitions()
{
	// Load landscape definition file
	XMLFile file;
    if (!file.readFile(PKGDIR "data/landscape.xml"))
	{
		dialogMessage("Scorched Landscape", 
					  "Failed to parse \"data/landscape.xml\"\n%s", 
					  file.getParserError());
		return false;
	}

	// Check file exists
	if (!file.getRootNode())
	{
		dialogMessage("Scorched Landscape",
					"Failed to find tank definition file \"data/landscape.xml\"");
		return false;		
	}

	// Itterate all of the landscapes in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
        childrenItor != children.end();
        childrenItor++)
    {
		LandscapeDefinition newDefn;
		if (!newDefn.readXML(*childrenItor)) return false;
		totalWeight_ += newDefn.weight;
		definitions_.push_back(newDefn);
	}
	return true;
}

LandscapeDefinition &LandscapeDefinitions::getLandscapeDefn()
{
	float pos = RAND * totalWeight_;
	float soFar = 0.0f;

	std::vector<LandscapeDefinition>::iterator itor;
	for (itor = definitions_.begin();
		itor != definitions_.end();
		itor++)
	{
		LandscapeDefinition &result = *itor;
		soFar += result.weight;

		if (pos <= soFar)
		{
			result.generate();
			return result;
		}

	}
	DIALOG_ASSERT(0);
	static LandscapeDefinition null;
	return null;
}
