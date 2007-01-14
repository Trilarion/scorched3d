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

#if !defined(AFX_ShieldRound_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_)
#define AFX_ShieldRound_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_

#include <weapons/Shield.h>

class ShieldRound : public Shield
{
public:
	ShieldRound();
	virtual ~ShieldRound();

	virtual bool parseXML(AccessoryCreateContext &context,
		XMLNode *accessoryNode);

	// ShieldRound attributes
	float getActualRadius() { return radius_; }
	bool getHalfShield() { return halfShield_; }
	bool getGlow() { return glow_; }

	virtual bool inShield(Vector &offset);
	virtual ShieldType getShieldType();
	virtual bool getRound() { return true; }

	REGISTER_ACCESSORY_HEADER(ShieldRound, AccessoryPart::AccessoryShield);

protected:
	float radius_;
	bool halfShield_;
	bool glow_;
};

#endif // !defined(AFX_ShieldRound_H__F9BCDF39_FB62_4BB4_9D64_C70215669F9C__INCLUDED_)
