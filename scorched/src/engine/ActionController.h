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


// ActionController.h: interface for the ActionController class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONCONTROLLER_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
#define AFX_ACTIONCONTROLLER_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_

#pragma warning(disable: 4786)

#include <set>
#include <list>
#include <engine/ActionBuffer.h>
#include <engine/PhysicsEngine.h>
#include <engine/GameStateI.h>

class ActionController : public GameStateI
{
public:
	static ActionController* instance();

    // Add an action to be simulated
	void addAction(Action *action);
	bool noReferencedActions();
	void resetTime();
	void clear();

	// Get the current physics engine
	PhysicsEngine &getPhysics() { return physicsEngine_; }
	ActionBuffer &getBuffer() { return buffer_; }

	// Set the simulation speed
	void setFast(float speedMult);
	float getFast() { return speed_; }

	// Inherited from GameStateI
	virtual void simulate(const unsigned state, float frameTime);
	virtual void draw(const unsigned state);

protected:
	static ActionController* instance_;
	std::list<Action *> newActions_;
	std::set<Action *> actions_;
	PhysicsEngine physicsEngine_;
	ActionBuffer buffer_;
	int referenceCount_;
	float speed_;
	float time_;

	void stepActions(float frameTime);
	void addNewActions();

private:
	ActionController();
	virtual ~ActionController();

};

#endif // !defined(AFX_ACTIONCONTROLLER_H__86995B4A_478E_4CFE_BD4C_79128DE51904__INCLUDED_)
