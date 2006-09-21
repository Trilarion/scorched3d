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

#if !defined(__INCLUDE_GLWPlanViewh_INCLUDE__)
#define __INCLUDE_GLWPlanViewh_INCLUDE__

#include <GLW/GLWidget.h>
#include <common/Vector.h>
#include <list>

class GLWPlanView : public GLWidget
{
public:
	GLWPlanView(float x = 0.0f, float y = 0.0f,
		float w = 0.0f, float h = 0.0f);
	virtual ~GLWPlanView();

	virtual void simulate(float frameTime);
	virtual void mouseDown(int button, float x, float y, bool &skipRest);
	virtual void mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest);
	virtual void mouseUp(int button, float x, float y, bool &skipRest);
	virtual void draw();

	REGISTER_CLASS_HEADER(GLWPlanView);

protected:
	float animationTime_;
	float flashTime_;
	float totalTime_;
	bool flash_;
	bool dragging_;
	float dragLastX_, dragLastY_;
	std::list<Vector> dragPoints_;

	void drawMap();
	void drawTexture();
	void drawWall();
	void drawCameraPointer();
	void drawTanks();
	void drawCurrentTank();
	void drawLines();

};

#endif
