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

#if !defined(__INCLUDE_WindDialogh_INCLUDE__)
#define __INCLUDE_WindDialogh_INCLUDE__

#include <GLW/GLWWindow.h>
#include <GLEXT/GLVertexSet.h>
#include <GLEXT/GLTexture.h>

class WindDialogToolTip : public GLWTip
{
public:
	WindDialogToolTip();
	virtual ~WindDialogToolTip();

	virtual void populate();
};

class WindDialog : public GLWWindow 
{
public:
	static WindDialog *instance();

	void buildMap();

	// Inherited from GLWWindow
	virtual void draw();

protected:
	static WindDialog *instance_;
	GLVertexSet *windModel_;
	GLTexture windTexture_;
	GLuint listNo_;
	WindDialogToolTip tip_;

	void drawArrow();
	void drawScene();
	void drawDisplay();

private:
	WindDialog();
	virtual ~WindDialog();
};


#endif
