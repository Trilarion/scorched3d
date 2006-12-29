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

#if !defined(__INCLUDE_TargetSpaceh_INCLUDE__)
#define __INCLUDE_TargetSpaceh_INCLUDE__

#include <target/Target.h>
#include <map>
#include <set>

class TargetSpace
{
public:
	TargetSpace();
	virtual ~TargetSpace();

	void addTarget(Target *target);
	void removeTarget(Target *target);

	Target *getCollision(Vector &position);
	void getCollisionSet(Vector &position, float radius, 
		std::map<unsigned int, Target *> &collisionTargets, 
		bool ignoreHeight = false);

	void draw();

protected:
	class Square
	{
	public:
		std::map<unsigned int, Target *> targets;
	};

	Square *squares_;
	int spaceX_, spaceY_; // Position of bottom left of space
	int spaceW_, spaceH_; // Width, height of space
	int spaceWSq_, spaceHSq_; // Number of squares in width and height
	int spaceSq_; // Width of each square
	int noSquares_; // The total number of squares

	void normalizeCoords(int &x, int &y)
	{
		// Make sure x and y are in the space
		x = MIN(x, spaceW_ + spaceX_);
		x = MAX(x, spaceX_);
		y = MIN(y, spaceH_ + spaceY_);
		y = MAX(y, spaceY_);

		// Find the square pos
		x -= spaceX_;
		y -= spaceY_;
		x /= spaceSq_;
		y /= spaceSq_;
	}

private:
	TargetSpace(TargetSpace &other);
	TargetSpace &operator=(TargetSpace &other);
};

#endif // __INCLUDE_TargetSpaceh_INCLUDE__