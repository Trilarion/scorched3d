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

#include <placement/PlacementObjectRandom.h>
#include <common/RandomGenerator.h>
#include <XML/XMLParser.h>

PlacementObjectRandom::PlacementObjectRandom()
{
}

PlacementObjectRandom::~PlacementObjectRandom()
{
}

bool PlacementObjectRandom::readXML(XMLNode *initialNode)
{
	XMLNode *node;
	while (initialNode->getNamedChild("randomobject", node, false))
	{
		RandomObject randomObject;

		std::string objecttype;
		XMLNode *objectNode;
		if (!node->getNamedChild("object", objectNode)) return false;
		if (!objectNode->getNamedParameter("type", objecttype)) return false;
		if (!(randomObject.object = PlacementObject::create(objecttype.c_str()))) return false;
		if (!randomObject.object->readXML(objectNode)) return false;

		objects_.push_back(randomObject);
	}
	if (!node->failChildren()) return false;

	return PlacementObject::readXML(node);
}

void PlacementObjectRandom::createObject(ScorchedContext &context,
	RandomGenerator &generator,
	unsigned int &playerId,
	PlacementType::Information &information,
	PlacementType::Position &position)
{
	int entryPos = generator.getRandUInt() % objects_.size();

	PlacementObject *entry = objects_[entryPos].object;
	entry->createObject(context, generator, playerId, information, position);
}