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

#if !defined(__INCLUDE_TankPhysicsh_INCLUDE__)
#define __INCLUDE_TankPhysicsh_INCLUDE__

#include <engine/ScorchedCollisionIds.h>
#include <engine/PhysicsEngine.h>
#include <coms/NetBuffer.h>
#include <vector>

class Tank;
class ScorchedContext;
class TankPhysics
{
public:

	struct ShotEntry
	{
		ShotEntry(float p = 0.0f, float r = 0.0f, float e = 0.0f) : 
			power(p), rot(r), ele(e) { }
	
		float power;
		float rot;
		float ele;
	};

	TankPhysics(ScorchedContext &context, unsigned int playerId);
	virtual ~TankPhysics();

	void setTank(Tank *tank) { tank_ = tank; }

	// State change
	void clientNewGame();
	void madeShot();
	
	// Saved settings
	float getOldPower();
	float getOldRotationGunXY();
	float getOldRotationGunYZ();
	float getRotationXYDiff();
	float getRotationYZDiff();
	float getPowerDiff();
	void revertSettings(unsigned int index = 0);
	std::vector<ShotEntry> &getOldShots() { return oldShots_; }

	// Gun/Turret Rotation
	float rotateGunXY(float angle, bool diff=true);
	float rotateGunYZ(float angle, bool diff=true);
	float getRotationGunXY() { return turretRotXY_; }
	float getRotationGunYZ() { return turretRotYZ_; }
	
	// Tank Rotation
	void rotateTank(float a) { angle_ = a; }
	float getAngle() { return angle_; }

	// Power of gun
	float getPower() { return power_; }
	float changePower(float power, bool diff=true);

	// Position
	void setTankPosition(Vector &pos);
	Vector &getVelocityVector();
	Vector &getTankPosition(); // Position of center bottom of tank
	Vector &getTankTurretPosition(); // Position of center of turret
	Vector &getTankGunPosition(); // Position of end of gun

	const char *getRotationString();
	const char *getElevationString();
	const char *getPowerString();

	// Physics
	void enablePhysics();
	void disablePhysics();

	// Serialize the tank
	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	ScorchedContext &context_;
	Tank *tank_;

	// Position
	Vector position_;

	// Turret angles
	std::vector<ShotEntry> oldShots_;
	float turretRotXY_, turretRotYZ_, power_;
	float angle_;

	// Physics engine stuff
	dGeomID tankGeom_;
	dGeomID shieldSmallGeom_;
	dGeomID shieldLargeGeom_;
	ScorchedCollisionInfo tankInfo_;
	ScorchedCollisionInfo shieldSmallInfo_;
	ScorchedCollisionInfo shieldLargeInfo_;

};

#endif

