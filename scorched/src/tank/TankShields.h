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


// TankShields.h: interface for the TankShields class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANKSHIELDS_H__88E5EA32_F84D_41E3_AF97_01CBD2874CE3__INCLUDED_)
#define AFX_TANKSHIELDS_H__88E5EA32_F84D_41E3_AF97_01CBD2874CE3__INCLUDED_

#include <weapons/Shield.h>
#include <coms/NetBuffer.h>
#include <map>
#include <list>

class TankShields  
{
public:
	TankShields();
	virtual ~TankShields();

	void reset();
	void newGame();

	void setCurrentShield(Shield *sh);

	Shield *getCurrentShield() { return currentShield_; }
	float getShieldPower() { return power_; }
	void setShieldPower(float power);

	void addShield(Shield *sh, int count);
	void rmShield(Shield *sh, int count);

	int getShieldCount(Shield *shield);
	std::map<Shield*, int> &getAllShields() { return shields_; }

	// Serialize the tank
    bool writeMessage(NetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

protected:
	std::map<Shield*, int> shields_;
	Shield *currentShield_;
	float power_;

};

#endif // !defined(AFX_TANKSHIELDS_H__88E5EA32_F84D_41E3_AF97_01CBD2874CE3__INCLUDED_)
