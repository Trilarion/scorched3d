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


#if !defined(__INCLUDE_PlayerDialogh_INCLUDE__)
#define __INCLUDE_PlayerDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWButton.h>
#include <GLW/GLWTextBox.h>
#include <tankgraph/GLWTankViewer.h>

class PlayerDialog : public GLWWindow,
						  public GLWButtonI
{
public:
	static PlayerDialog *instance();

	const char *getPlayerName() 
		{ return playerName_->getText().c_str(); }
	const char *getModelName()
		{ return viewer_->getModelName(); }
	const char *getPlayerPassword()
		{ return playerPassword_->getText().c_str(); }

	// Inherited from GLWWindow
	virtual void draw();

	// Inherited from GLWButtonI
	virtual void buttonDown(unsigned int id);

protected:
	static PlayerDialog *instance_;
	GLWTankViewer *viewer_;
	GLWTextBox *playerName_;
	GLWTextBox *playerPassword_;
	unsigned int okId_;
	bool skipInit_;

private:
	PlayerDialog();
	virtual ~PlayerDialog();

};


#endif
