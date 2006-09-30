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

#if !defined(__INCLUDE_WeaponTypeActionh_INCLUDE__)
#define __INCLUDE_WeaponTypeActionh_INCLUDE__

#include <actions/CallbackWeapon.h>

class WeaponTypeAction  : public WeaponCallback
{
public:
	WeaponTypeAction();
	virtual ~WeaponTypeAction();

	virtual bool parseXML(OptionsGame &context, 
		AccessoryStore *store, XMLNode *accessoryNode);

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		unsigned int playerId, Vector &position, Vector &velocity,
		unsigned int data = 0);

	// Inherited from WeaponCallback
	virtual void weaponCallback(
		ScorchedContext &context,
		unsigned int playerId, Vector &position, Vector &velocity,
		unsigned int data,
		unsigned int userData);

	REGISTER_ACCESSORY_HEADER(WeaponTypeAction, AccessoryPart::AccessoryWeapon);

protected:
	std::map<std::string, Weapon *> actions_;

};

#endif // __INCLUDE_WeaponTypeActionh_INCLUDE__
