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

#if !defined(__INCLUDE_GLWSelectorh_INCLUDE__)
#define __INCLUDE_GLWSelectorh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLW/GLWToolTip.h>
#include <GLEXT/GLTexture.h>
#include <string>
#include <list>

/**
Defines the contents of one row of the selection dialog.
*/
class GLWSelectorEntry
{
public:
	GLWSelectorEntry(const char *text = "", 
		GLWTip *tooltip = 0, 
		bool selected = false,
		GLTexture *icon = 0);
	
	const char *getText() { return text_.c_str(); }
	GLWTip *getToolTip() { return tip_; }
	GLTexture *getIcon() { return icon_; }
	bool getSelected() { return selected_; }
	
protected:
	std::string text_;
	GLTexture *icon_;
	GLWTip *tip_;
	bool selected_;
};

/**
The user interested in the chosen selection.
*/
class GLWSelectorI
{
public:
	virtual void itemSelected(GLWSelectorEntry *entry, int position) = 0;
};

/**
A class that presents the user with an on screen menu and
allows them to select one item.
This class is used by other classes that throw up a selection
window to the user.
*/
class GLWSelector : public GLWWindow
{
public:
    static GLWSelector *instance();

	// Show the selector as the specified position
	void showSelector(GLWSelectorI *user,
		float x, float y,
		std::list<GLWSelectorEntry> &entries);
	// Hide the selector
	void hideSelector();

	// Inherited from GLWWindow
	virtual void draw();
	virtual void mouseDown(float x, float y, bool &skipRest);
	virtual void mouseUp(float x, float y, bool &skipRest);
	virtual void mouseDrag(float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

protected:
	static GLWSelector *instance_;

	GLWSelectorI *user_;
	std::list<GLWSelectorEntry> entries_;
	float drawX_, drawY_;
	float drawW_, drawH_;
	bool visible_;

private:
	GLWSelector();
	virtual ~GLWSelector();

};

#endif
