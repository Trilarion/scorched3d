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

#include <landscape/LandscapeDefn.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

static bool parseMinMax(XMLNode *parent, const char *name, 
	float &min, float &max)
{
	XMLNode *node = 0;
	if (!parent->getNamedChild(name, node)) return false;
	if (!node->getNamedFloat("max", max)) return false;
	if (!node->getNamedFloat("min", min)) return false;
	return node->failChildren();
}

bool LandscapeDefnStartHeight::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(startcloseness);
	buffer.addToBuffer(heightmin);
	buffer.addToBuffer(heightmax);
	return true;
}

bool LandscapeDefnStartHeight::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(startcloseness)) return false;
	if (!reader.getFromBuffer(heightmin)) return false;
	if (!reader.getFromBuffer(heightmax)) return false;
	return true;
}

bool LandscapeDefnStartHeight::readXML(XMLNode *node)
{
	if (!node->getNamedFloat("startcloseness", 
		startcloseness)) return false;
	if (!parseMinMax(node, "height", 
		heightmin, heightmax)) return false;
	return node->failChildren();
}

bool LandscapeDefnHeightMapGenerate::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(mask);
	buffer.addToBuffer(landhillsmax);
	buffer.addToBuffer(landhillsmin);
	buffer.addToBuffer(landheightmax);
	buffer.addToBuffer(landheightmin);
	buffer.addToBuffer(landwidthx);
	buffer.addToBuffer(landwidthy);
	buffer.addToBuffer(landpeakwidthxmax);
	buffer.addToBuffer(landpeakwidthxmin);
	buffer.addToBuffer(landpeakwidthymax);
	buffer.addToBuffer(landpeakwidthymin);
	buffer.addToBuffer(landpeakheightmax);
	buffer.addToBuffer(landpeakheightmin);
	buffer.addToBuffer(landsmoothing);
	return true;
}

bool LandscapeDefnHeightMapGenerate::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(mask)) return false;
	if (!reader.getFromBuffer(landhillsmax)) return false;
	if (!reader.getFromBuffer(landhillsmin)) return false;
	if (!reader.getFromBuffer(landheightmax)) return false;
	if (!reader.getFromBuffer(landheightmin)) return false;
	if (!reader.getFromBuffer(landwidthx)) return false;
	if (!reader.getFromBuffer(landwidthy)) return false;
	if (!reader.getFromBuffer(landpeakwidthxmax)) return false;
	if (!reader.getFromBuffer(landpeakwidthxmin)) return false;
	if (!reader.getFromBuffer(landpeakwidthymax)) return false;
	if (!reader.getFromBuffer(landpeakwidthymin)) return false;
	if (!reader.getFromBuffer(landpeakheightmax)) return false;
	if (!reader.getFromBuffer(landpeakheightmin)) return false;
	if (!reader.getFromBuffer(landsmoothing)) return false;
	return true;
}

bool LandscapeDefnHeightMapGenerate::readXML(XMLNode *node)
{
	if (!node->getNamedString("mask", mask)) return false;
	if (!parseMinMax(node, "landhills", 
		landhillsmin, landhillsmax)) return false;
	if (!parseMinMax(node, "landheight", 
		landheightmin, landheightmax)) return false;
	if (!node->getNamedFloat("landwidthx", landwidthx)) return false;
	if (!node->getNamedFloat("landwidthy", landwidthy)) return false;
	if (!parseMinMax(node, "landpeakwidthx", 
		landpeakwidthxmin, landpeakwidthxmax)) return false;
	if (!parseMinMax(node, "landpeakwidthy", 
		landpeakwidthymin, landpeakwidthymax)) return false;
	if (!parseMinMax(node, "landpeakheight", 
		landpeakheightmin, landpeakheightmax)) return false;
	if (!node->getNamedFloat("landsmoothing", landsmoothing)) return false;
	return node->failChildren();
}

LandscapeDefn::LandscapeDefn() :
	heightmap(0), tankstart(0)
{
}

LandscapeDefn::~LandscapeDefn()
{
}

bool LandscapeDefn::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer(name);
	buffer.addToBuffer(minplayers);
	buffer.addToBuffer(maxplayers);
	if (!tankstart->writeMessage(buffer)) return false;
	if (!heightmap->writeMessage(buffer)) return false;
	return true;
}

bool LandscapeDefn::readMessage(NetBufferReader &reader)
{
	if (!reader.getFromBuffer(name)) return false;
	if (!reader.getFromBuffer(minplayers)) return false;
	if (!reader.getFromBuffer(maxplayers)) return false;
	if (!tankstart->readMessage(reader)) return false;
	if (!heightmap->readMessage(reader)) return false;
	return true;
}

bool LandscapeDefn::readXML(XMLNode *node)
{
	if (!node->getNamedString("name", name)) return false;
	if (!node->getNamedInt("minplayers", minplayers)) return false;
	if (!node->getNamedInt("maxplayers", maxplayers)) return false;

	XMLNode *startNode;
	if (!node->getNamedChild("tankstart", startNode)) return false;
	tankstart = new LandscapeDefnStartHeight;
	if (!tankstart->readXML(startNode)) return false;

	XMLNode *heightNode;
	if (!node->getNamedChild("heightmap", heightNode)) return false;
	heightmap = new LandscapeDefnHeightMapGenerate;
	if (!heightmap->readXML(heightNode)) return false;

	return node->failChildren();
}

