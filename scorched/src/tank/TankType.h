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

#if !defined(__INCLUDE_TankTypeh_INCLUDE__)
#define __INCLUDE_TankTypeh_INCLUDE__

#include <string>
#include <map>
#include <set>

class XMLNode;
class Accessory;
class ScorchedContext;
class TankType
{
public:
	TankType();
	virtual ~TankType();

	const char *getName() { return name_.c_str(); }
	const char *getDescription();
	float getLife() { return life_; }
	float getPower() { return power_; }
	bool getMovementSmoke() { return movementSmoke_; }

	std::map<Accessory *, int> &getAccessories() { return accessories_; }
	bool getAccessoryDisabled(Accessory *accessory);

	bool initFromXML(ScorchedContext &context, XMLNode *node);

protected:
	std::string name_;
	std::map<Accessory *, int> accessories_;
	std::set<Accessory *> disabledAccessories_;
	float life_;
	float power_;
	bool movementSmoke_;
};

#endif // __INCLUDE_TankTypeh_INCLUDE__
