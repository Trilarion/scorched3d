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

#if !defined(AFX_SHIELD_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_)
#define AFX_SHIELD_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_

#include <weapons/AccessoryPart.h>
#include <common/Vector.h>

const float ShieldSizeSmallSize = 3.0f;
const float ShieldSizeLargeSize = 6.0f;

class Shield : public AccessoryPart
{
public:
	enum ShieldSize
	{
		ShieldSizeSmall,
		ShieldSizeLarge
	};
	enum ShieldType
	{
		ShieldTypeNormal,
		ShieldTypeReflective,
		ShieldTypeMag
	};

	Shield();
	virtual ~Shield();

	virtual bool parseXML(OptionsGame &context, 
		AccessoryStore *store, XMLNode *accessoryNode);

	// Shield attributes
	const char *getCollisionSound();
	float getHitRemovePower() { return removePower_; }
	float getHitPenetration() { return penetration_; }
	ShieldSize getRadius() { return radius_; }
	Vector &getColor() { return color_; }
	bool getHalfShield() { return halfShield_; }
	virtual ShieldType getShieldType();

	REGISTER_ACCESSORY_HEADER(Shield, AccessoryPart::AccessoryShield);

protected:
	std::string collisionSound_;
	ShieldSize radius_;
	Vector color_;
	float removePower_;
	float penetration_;
	bool halfShield_;
};

#endif // !defined(AFX_SHIELD_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_)
