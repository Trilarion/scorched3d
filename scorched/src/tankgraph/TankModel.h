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

#if !defined(__INCLUDE_TankModelh_INCLUDE__)
#define __INCLUDE_TankModelh_INCLUDE__

#include <3dsparse/ModelID.h>
#include <3dsparse/ImageID.h>
#include <common/Vector.h>
#include <set>
#include <string>

class ScorchedContext;
class TankMesh;
class TankType;
class TankModel
{
public:
	TankModel();
	virtual ~TankModel();

	bool initFromXML(ScorchedContext &context, XMLNode *node);

	virtual void draw(bool drawS, float angle, Vector &position, 
		float fireOffSet, float rotXY, float rotXZ, 
		bool absCenter = false, float scale = 1.0f, float LOD = 1.0f);
	virtual int getNoTris();

	virtual bool lessThan(TankModel *other);
	void clear();

	const char *getName() { return tankName_.c_str(); }
	const char *getTypeName() { return typeName_.c_str(); }
	ModelID &getTankModelID() { return modelId_; }
	ModelID &getProjectileModelID() { return projectileModelId_; }
	ImageID &getTracksVId() { return tracksVId_; }
	ImageID &getTracksHId() { return tracksHId_; }
	ImageID &getTracksVHId() { return tracksVHId_; }
	ImageID &getTracksHVId() { return tracksHVId_; }
	TankMesh *getTankMesh() { return tankMesh_; }
	std::set<std::string> &getCatagories() { return catagories_; }
	bool getAiOnly() { return aiOnly_; }
	bool getMovementSmoke() { return movementSmoke_; }

	bool isOfAi(bool ai);
	bool isOfCatagory(const char *catagory);
	bool isOfTeam(int team);

protected:
	bool init_;
	bool aiOnly_;
	bool movementSmoke_;
	std::string tankName_;
	std::string typeName_;
	ModelID modelId_;
	ModelID projectileModelId_;
	ImageID tracksVId_;
	ImageID tracksHId_;
	ImageID tracksVHId_;
	ImageID tracksHVId_;
	TankMesh *tankMesh_;
	std::set<std::string> catagories_;
	std::set<int> teams_;

	bool loadImage(XMLNode *node, const char *nodeName, 
		ImageID &image, const char *backupImage);
};

#endif
