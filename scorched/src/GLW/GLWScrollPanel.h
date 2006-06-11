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

#if !defined(AFX_GLWSCROLLPANEL_H__53CDEDBB_3EC0_480E_8E9C_E40645155DD0__INCLUDED_)
#define AFX_GLWSCROLLPANEL_H__53CDEDBB_3EC0_480E_8E9C_E40645155DD0__INCLUDED_

#include <GLW/GLWPanel.h>
#include <GLW/GLWScrollW.h>

class GLWScrollPanel : public GLWPanel,
						public GLWScrollWI
{
public:
	GLWScrollPanel(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f);
	virtual ~GLWScrollPanel();

	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(float x, float y, bool &skipRest);
	virtual void mouseUp(float x, float y, bool &skipRest);
	virtual void mouseDrag(float mx, float my, float x, float y, bool &skipRest);
	virtual void mouseWheel(float x, float y, float z, bool &skipRest);

	virtual void positionChange(unsigned int id, int current, int movement);
	virtual void clear();

	virtual void setH(float h);

	bool &getDrawScrollBar() { return drawScrollBar_; }
	GLWScrollW &getScrollBar() { return scrollW_; }

	REGISTER_CLASS_HEADER(GLWScrollPanel);
protected:
	GLWScrollW scrollW_;
	bool drawScrollBar_;
	int maxSee_;
	float widgetHeight_;

};

#endif // !defined(AFX_GLWSCROLLPANEL_H__53CDEDBB_3EC0_480E_8E9C_E40645155DD0__INCLUDED_)
