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


// TankAIComputer.h: interface for the TankAIComputer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANKAICOMPUTER_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
#define AFX_TANKAICOMPUTER_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_

#include <tank/Tank.h>
#include <tank/TankAI.h>
#include <tankai/TankAIComputerBuyer.h>

#define TANKAI_DEFINE(x, y) \
	virtual const char *getName() { return #x; } \
	virtual TankAIComputer *getCopy(Tank *tank) { return new y(tank); }

class TankAIComputer : public TankAI
{
public:
	TankAIComputer(Tank *tank);
	virtual ~TankAIComputer();

	virtual bool isHuman() { return false; }

	virtual const char *getName() = 0;
	virtual TankAIComputer *getCopy(Tank *tank) = 0;

	// Inherited from TankAI
	virtual void newGame();
	virtual void nextRound();
	virtual void tankHurt(Weapon *weapon, unsigned int firer);
	virtual void shotLanded(Weapon *weapon, unsigned int firer, 
		Vector &position);

	// Notificiation that the shot fired has landed
	virtual void buyAccessories();
	virtual void ourShotLanded(Weapon *weapon, Vector &position);

protected:
	TankAIComputerBuyer tankBuyer_;
	bool primaryShot_;

	void say(const char *text);
	void fireShot();
	void selectFirstShield();

};

#endif // !defined(AFX_TANKAICOMPUTER_H__5F21C9C7_0F71_4CCC_ABB9_976CF0A5C5EC__INCLUDED_)
