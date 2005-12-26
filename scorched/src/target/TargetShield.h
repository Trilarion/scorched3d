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

#if !defined(__INCLUDE_TargetShieldh_INCLUDE__)
#define __INCLUDE_TargetShieldh_INCLUDE__

#include <engine/ScorchedCollisionIds.h>
#include <engine/PhysicsEngine.h>
#include <coms/NetBuffer.h>

class Accessory;
class ScorchedContext;
class TargetShield
{
public:
	TargetShield(ScorchedContext &context,
		unsigned int playerId);
	virtual ~TargetShield();

	virtual void newGame();

	Accessory *getCurrentShield() { return currentShield_; }
	void setCurrentShield(Accessory *sh);

	void setPosition(Vector &position);

	float getShieldPower() { return power_; }
	void setShieldPower(float power);

	// Serialize
    bool writeMessage(NetBuffer &buffer);
    bool readMessage(NetBufferReader &reader);

protected:
	Vector position_;
	ScorchedContext &context_;
	Accessory *currentShield_;
	float power_;

	// Physics item
	dGeomID shieldGeom_;
	ScorchedCollisionInfo shieldInfo_;
};

#endif // __INCLUDE_TargetShieldh_INCLUDE__