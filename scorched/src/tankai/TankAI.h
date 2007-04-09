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

#if !defined(AFX_TankAI_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TankAI_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <engine/ScorchedCollisionIds.h>
#include <common/Vector.h>
#include <common/ToolTip.h>
#include <string>

class Weapon;
class XMLNode;
class Tank;
class TankAI
{
public:
	TankAI();
	virtual ~TankAI();

	// Instance init
	virtual TankAI *createCopy(Tank *tank) = 0;

	// Onetime init
	virtual bool parseConfig(XMLNode *node);

	// Other
	virtual const char *getName() { return name_.c_str(); }
	virtual const char *getDescription() { return description_.c_str(); }
	virtual ToolTip *getToolTip();

	// Notification of actions to perform
	virtual void newMatch() = 0;
	virtual void newGame() = 0;
	virtual void playMove() = 0;
	virtual void buyAccessories() = 0;
	virtual void autoDefense() = 0;

	// Notification of actions happened
	virtual void tankHurt(Weapon *weapon, float damage, 
		unsigned int damaged, unsigned int firer) = 0;
	virtual void shotLanded(ScorchedCollisionId collision,
		Weapon *weapon, unsigned int firer, 
		Vector &position) = 0;

	// Indicates if this computer ai is available for choice by
	// the random tank ai type
	virtual bool availableForRandom() { return availableForRandom_; }
	virtual bool availableForPlayers() { return availableForPlayers_; }

protected:
	std::string name_, description_;
	bool availableForRandom_;
	bool availableForPlayers_;
	ToolTip toolTip_;

};

#endif // !defined(AFX_TankAI_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
