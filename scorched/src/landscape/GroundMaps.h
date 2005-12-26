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

#if !defined(__INCLUDE_GroundMapsh_INCLUDE__)
#define __INCLUDE_GroundMapsh_INCLUDE__

#include <landscape/HeightMap.h>
#include <landscape/LandscapeObjects.h>

class ScorchedContext;
class LandscapeDefinitionCache;
class GroundMaps
{
public:
	GroundMaps(LandscapeDefinitionCache &defnCache);
	virtual ~GroundMaps();

	// Generates the next level
	void generateMaps(
		ScorchedContext &context,
		ProgressCounter *counter = 0);

	// Height map functions
	float getHeight(int w, int h);
	float getInterpHeight(float w, float h);
	Vector &getNormal(int w, int h);
	void getInterpNormal(float w, float h, Vector &normal);
	bool getIntersect(Line &direction, Vector &intersect);

	// Playable landscape area fns
	int getMapWidth() { return map_.getMapWidth(); }
	int getMapHeight() { return map_.getMapHeight(); }

	// Objects funtions
	LandscapeObjects &getObjects() { return objects_; }

	// Actual heightmap
	HeightMap &getHeightMap() { return map_; }
	HeightMap &getSurroundMap() { return smap_; }

protected:
	HeightMap map_; // The current level's heightmap
	HeightMap smap_; // The surround of the landscape
	LandscapeObjects objects_; // The objects in the scene
	LandscapeDefinitionCache &defnCache_;

	// Generate levels
	void generateHMap(
		ScorchedContext &context,
		ProgressCounter *counter = 0);
	void generateObjects(
		ScorchedContext &context,
		ProgressCounter *counter = 0);

};

#endif // __INCLUDE_GroundMapsh_INCLUDE__