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

#if !defined(__INCLUDE_TargetMovementh_INCLUDE__)
#define __INCLUDE_TargetMovementh_INCLUDE__

#include <landscapedef/LandscapeInclude.h>
#include <movement/TargetMovementEntry.h>

class ScorchedContext;
class RandomGenerator;
class TargetMovement
{
public:
	TargetMovement();
	virtual ~TargetMovement();

	void generate(ScorchedContext &context);
	void simulate(float frameTime);
	void draw();

	// Serialize the movement data (if any)
	bool writeMessage(NetBuffer &buffer);
	bool readMessage(NetBufferReader &reader);

protected:
	std::vector<TargetMovementEntry *> movements_;

	void addMovements(ScorchedContext &context, 
		RandomGenerator &random, 
		std::vector<LandscapeInclude *> &movements);
	void addMovementType(ScorchedContext &context, 
		RandomGenerator &random, 
		std::vector<LandscapeMovementType *> &movementtype);
};

#endif // __INCLUDE_TargetMovementh_INCLUDE__
