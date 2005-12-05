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

#if !defined(AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_)
#define AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_

#include <list>
#include <vector>
#include <map>
#include <weapons/Weapon.h>
#include <weapons/Accessory.h>
#include <common/ProgressCounter.h>

class XMLNode;
class AccessoryStore
{
public:
	AccessoryStore();
	virtual ~AccessoryStore();

	bool parseFile(OptionsGame &context, ProgressCounter *counter = 0);
	void clearAccessories();

	Accessory *findByAccessoryType(AccessoryPart::AccessoryType type);
	Accessory *findByAccessoryId(unsigned int id);
	Accessory *findByPrimaryAccessoryName(const char *name); // **careful there**
	AccessoryPart *createAccessoryPart(OptionsGame &context, 
		Accessory *parent, XMLNode *currentNode);

	Weapon *getDeathAnimation();
	Weapon *getMuzzelFlash();

	std::list<Accessory *> getAllWeapons(bool sort=false);
	std::list<Accessory *> getAllOthers(bool sort=false);	
	std::list<Accessory *> getAllAccessories(bool sort=false);	
	std::map<std::string, XMLNode *> &getParsingNodes() { return parsingNodes_; }

	bool writeWeapon(NetBuffer &buffer, Weapon *weapon);
	Weapon *readWeapon(NetBufferReader &reader);

	bool writeEconomyToBuffer(NetBuffer &buffer);
	bool readEconomyFromBuffer(NetBufferReader &reader);

	static void sortList(std::list<Accessory *> &accList);

protected:
	AccessoryPart *findByAccessoryPartId(unsigned int id);

	std::list<Accessory *> accessories_;
	std::list<AccessoryPart *> accessoryParts_;
	std::vector<Weapon *> deathAnimations_;
	std::map<std::string, XMLNode *> parsingNodes_;
	Weapon *muzzleFlash_;

};

#endif // !defined(AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_)
