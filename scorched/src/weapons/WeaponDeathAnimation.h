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

#if !defined(AFX_WeaponDeathAnimation_H__A96ADD10_0901_4E1D_A49B_9BE78AD33B9B__INCLUDED_)
#define AFX_WeaponDeathAnimation_H__A96ADD10_0901_4E1D_A49B_9BE78AD33B9B__INCLUDED_

#include <weapons/Weapon.h>

class WeaponDeathAnimation : public Weapon
{
public:
	WeaponDeathAnimation();
	virtual ~WeaponDeathAnimation();

	// This weapon can never be bought
	virtual bool getPrimary() { return false; } 
	virtual void setPrimary(bool p) { }

	virtual bool parseXML(XMLNode *accessoryNode);
	virtual bool writeAccessory(NetBuffer &buffer);
	virtual bool readAccessory(NetBufferReader &reader);

	// Inherited from Weapon
	virtual void fireWeapon(ScorchedContext &context,
		unsigned int playerId, Vector &position, Vector &velocity);

	REGISTER_ACCESSORY_HEADER(WeaponDeathAnimation, Accessory::AccessoryWeapon);

protected:
	std::string rendererName_;

};

#endif // !defined(AFX_WeaponDeathAnimation_H__A96ADD10_0901_4E1D_A49B_9BE78AD33B9B__INCLUDED_)
