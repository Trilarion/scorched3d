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


// GameState.h: interface for the GameState class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GAMESTATE_H__00A5F6B7_02B2_45B3_9D9B_F8B0AC9F5614__INCLUDED_)
#define AFX_GAMESTATE_H__00A5F6B7_02B2_45B3_9D9B_F8B0AC9F5614__INCLUDED_

#pragma warning(disable: 4786)

#include <map>
#include <list>
#include <engine/MainLoopI.h>

class GameStateI;
class GameStateStimulusI;

class GameState : public MainLoopI
{
public:
	enum MouseButton
	{
		MouseButtonLeft = 0x1,
		MouseButtonMiddle = 0x2,
		MouseButtonRight = 0x4
	};

	static GameState *instance();

	// Called by the simulator
	virtual void simulate(float simTime);
	virtual void draw();

	// Called by SDL subsystem 
	void mouseDown(MouseButton button, int x, int y);
	void mouseUp(MouseButton button, int x, int y);
	void mouseMove(int x, int y);
	void mouseWheel(short z);

	// User fns to change + set state
	void setState(const unsigned state);
	void stimulate(const unsigned stimulus);
	void setFakeMiddleButton(bool fake);
	const unsigned getState() { return currentState_; }

	// User fns to add classes to state management
	void addStateStimulus(const unsigned state, 
						  const unsigned stim, 
						  const unsigned nexts);
	void addStateStimulus(const unsigned state, 
						  GameStateStimulusI *check, 
						  const unsigned nexts);
	void addStateEntry(const unsigned state, 
					   GameStateI *entry);
	void addStateLoop(const unsigned state, 
					  GameStateI *entry, 
					  GameStateI *subEntry);
	void addStateKeyEntry(const unsigned state, 
						  GameStateI *subEntry);
	void addStateMouseDownEntry(const unsigned state, 
								const unsigned buttons, 
								GameStateI *subEntry);
	void addStateMouseUpEntry(const unsigned state, 
							  const unsigned buttons, 
							  GameStateI *subEntry);
	void addStateMouseDragEntry(const unsigned state, 
								const unsigned buttons, 
								GameStateI *subEntry);
	void addStateMouseWheelEntry(const unsigned state, 
								 GameStateI *subEntry);

protected:
	static GameState *instance_;
	typedef std::list<GameStateI *> StateIList;
	typedef std::pair<GameStateStimulusI *, unsigned> SimulusIPair;
	typedef std::list<SimulusIPair> StiulusIList;

	struct GameStateSubEntry
	{
		GameStateI *current;
		StateIList subLoopList;
	};

	struct GameStateEntry 
	{
		// Classes called for every loop
		std::list<GameStateSubEntry> loopList; 

		 // Possible stimuli in this state
		std::map<unsigned, unsigned> stimList;
		// Stimili checked every loop
		StiulusIList condStimList; 

		// Classes called on key events
		StateIList subKeyList; 
		// Classes called on mouse down L
		StateIList subMouseDownLeftList; 
		// Classes called on mouse down M
		StateIList subMouseDownMiddleList; 
		// Classes called on mouse down R
		StateIList subMouseDownRightList;
		// Classes called on mouse up L
		StateIList subMouseUpLeftList;
		// Classes called on mouse up M
		StateIList subMouseUpMiddleList; 
		// Classes called on mouse up R
		StateIList subMouseUpRightList;
		// Classes called on mouse drag L
		StateIList subMouseDragLeftList; 
		// Classes called on mouse drag M
		StateIList subMouseDragMiddleList;
		// Classes called on mouse drag R
		StateIList subMouseDragRightList;
		// Classes called on mouse down W
		StateIList subMouseWheelList;
		// Classes called when entering state
		StateIList enterStateList;
	};

	unsigned currentState_; 
	GameStateEntry *currentEntry_; 
	std::map<unsigned, GameStateEntry> stateList_;
	unsigned pendingStimulus_;
	bool fakeMiddleButton_;

	// Dragging stuff
	// Up or down for each button (bit field)
	unsigned currentMouseState_; 
	int mouseLDragX_, mouseLDragY_;
	int mouseMDragX_, mouseMDragY_;
	int mouseRDragX_, mouseRDragY_;

	bool checkStimulate();
	void mouseMoveCall(const unsigned state, MouseButton button, 
		StateIList &currentList, 
		int mx, int my,
		int dx, int dy);
	void mouseUpDown(MouseButton button, bool down, int x, int y);
	GameState::GameStateEntry* 
		getEntry(const unsigned state);
	GameState::GameStateSubEntry* 
		getSubEntry(const unsigned state, GameStateI *entry);

private:
	GameState();
	virtual ~GameState();

};

#endif // !defined(AFX_GAMESTATE_H__00A5F6B7_02B2_45B3_9D9B_F8B0AC9F5614__INCLUDED_)
