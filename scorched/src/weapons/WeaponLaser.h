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

#if !defined(__INCLUDE_WeaponLaserh_INCLUDE__)
#define __INCLUDE_WeaponLaserh_INCLUDE__

#include <weapons/Weapon.h>

class WeaponLaser  : public Weapon
{
public:
	WeaponLaser();
	virtual ~WeaponLaser();

	virtual bool parseXML(OptionsGame &context, 
		AccessoryStore *store, XMLNode *accessoryNode);

	float getMinimumHurt() { return minimumHurt_; }
	float getMaximumHurt() { return maximumHurt_; }
	float getMinimumDistance() { return minimumDistance_; }
	float getMaximumDistance() { return maximumDistance_; }
	float getTotalTime() { return totalTime_; }
	Vector &getColor() { return color_; }

	// Inherited from Weapon
	void fireWeapon(ScorchedContext &context,
		unsigned int playerId, Vector &position, Vector &velocity,
		unsigned int data = 0);

	REGISTER_ACCESSORY_HEADER(WeaponLaser, AccessoryPart::AccessoryWeapon);

protected:
	float minimumHurt_, maximumHurt_;
	float minimumDistance_, maximumDistance_;
	float totalTime_;
	Vector color_;

};

#endif // __INCLUDE_WeaponLaserh_INCLUDE__