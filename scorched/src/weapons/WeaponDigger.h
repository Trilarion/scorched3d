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

#if !defined(AFX_WEAPONDIGGER_H__24F2D834_712D_4355_AC74_3571E2F4B14D__INCLUDED_)
#define AFX_WEAPONDIGGER_H__24F2D834_712D_4355_AC74_3571E2F4B14D__INCLUDED_

#include <weapons/Weapon.h>

class WeaponDigger : public Weapon
{
public:
	WeaponDigger();
	virtual ~WeaponDigger();

	virtual bool parseXML(XMLNode *accessoryNode);
	virtual bool writeAccessory(NetBuffer &buffer);
	virtual bool readAccessory(NetBufferReader &reader);

	// Inherited from Weapon
	Action *fireWeapon(unsigned int playerId, Vector &position, Vector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponDigger, Accessory::AccessoryWeapon);

protected:
	int warHeads_;

};

#endif // !defined(AFX_WEAPONDIGGER_H__24F2D834_712D_4355_AC74_3571E2F4B14D__INCLUDED_)
