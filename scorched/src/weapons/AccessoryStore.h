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


// AccessoryStore.h: interface for the AccessoryStore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_)
#define AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_

#pragma warning(disable: 4786)

#include <list>
#include <vector>
#include <weapons/Weapon.h>

class Tank;
class AccessoryStore
{
public:
	static AccessoryStore *instance();

	Accessory *findByAccessoryType(Accessory::AccessoryType type);
	Accessory *findByAccessoryName(const char *name);

	Weapon *getDeathAnimation();

	std::list<Accessory *> getAllWeapons();
	std::list<Accessory *> getAllOthers();
	std::list<Accessory *> &getAllAccessories();

protected:
	static AccessoryStore *instance_;
	std::list<Accessory *> accessories_;
	std::vector<Weapon *> deathAnimations_;
	bool parseFile();
	Accessory *createAccessoryType(const char *type);

private:
	AccessoryStore();
	virtual ~AccessoryStore();

};

#endif // !defined(AFX_ACCESSORYSTORE_H__6A964E56_3784_42FA_A900_2AB4B98F99A8__INCLUDED_)
