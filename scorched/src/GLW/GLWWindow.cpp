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


// GLWWindow.cpp: implementation of the GLWWindow class.
//
//////////////////////////////////////////////////////////////////////

#include <GLEXT/GLState.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLBitmap.h>
#include <GLW/GLWWindow.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static const float roundSize = 20.0f;
static const float smallRoundSize = 10.0f;
static const float smallTitleHeight = 10.0f;
static const float minWindowSize = 50.0f;
static const float titleWidth = 100.0f;
static const float titleHeight = 20.0f;
static const float shadowWidth = 10.0f;

GLWWindow::GLWWindow(const char *name, float x, float y, 
					 float w, float h,
					 unsigned int states) : 
	GLWVisiblePanel(x, y, w, h), dragging_(NoDrag), 
	needCentered_(false), showTitle_(false), name_(name),
	disabled_(false), windowState_(states)
{
	getDrawPanel() = false;
}

GLWWindow::GLWWindow(const char *name, float w, float h,
					 unsigned int states) :
	GLWVisiblePanel(0.0f, 0.0f, w, h), dragging_(NoDrag), 
	needCentered_(true), showTitle_(false), name_(name),
	disabled_(false), windowState_(states)
{
	getDrawPanel() = false;
}

GLWWindow::~GLWWindow()
{

}

void GLWWindow::windowInit(const unsigned state)
{

}

void GLWWindow::windowDisplay()
{

}

void GLWWindow::drawOutlinePoints(float x, float y, float w, float h)
{
	float tw = titleWidth;
	if (w < titleWidth + 25.0f) tw = w - 25.0f;

	float th = titleHeight;
	float sr = roundSize;
	bool sz = true;
	if (windowState_ & eSmallTitle)
	{
		th = smallTitleHeight;
		sr = smallRoundSize;
		sz = false;
	}

	if (windowState_ & eResizeable)
		drawCircle(8, 4, x + w - smallRoundSize, y + smallRoundSize, false);
	else
		drawCircle(8, 4, x + w - roundSize, y + roundSize, true);
	drawCircle(4, 0, x + w - roundSize, y + h - roundSize, true);
	if (((tw > 70.0f) || (tw > 50.0f && windowState_ & eSmallTitle)) && !(windowState_ & eNoTitle))
	{
		showTitle_ = true;
		drawCircle(8, 10, x + tw, y + h + sr, sz);
		drawCircle(2, 0, x + tw - sr - sr, y + h + th - sr, sz);
		drawCircle(0, -4, x + roundSize, y + h + th - roundSize, true);
	}
	else
	{
		showTitle_ = false;
		drawCircle(0, -4, x + roundSize, y + h - roundSize, true);
	}
	drawCircle(-4, -8, x + roundSize, y + roundSize, true);
}

void GLWWindow::drawBackSurface(float x, float y, float w, float h)
{
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x + roundSize, y + roundSize);
		glVertex2f(x + roundSize, y);
		drawOutlinePoints(x, y, w, h);
		glVertex2f(x + roundSize, y);
	glEnd();
}

void GLWWindow::drawSurround(float x, float y, float w, float h)
{
	glBegin(GL_LINE_LOOP);
		drawOutlinePoints(x, y, w, h);
	glEnd();
}

void GLWWindow::drawTitleBar(float x, float y, float w, float h)
{
	// Draw the indented lines in the header
	glBegin(GL_LINES);
		for (int i=0; i<=8; i+=4)
		{
			float top = y + h + titleHeight - i - 6.0f;

			glColor3f(0.4f, 0.4f, 0.6f);
			glVertex2f(x + 10.0f, top);
			glVertex2f(x + titleWidth - 32.0f, top);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x + 10.0f, top - 1.0f);
			glVertex2f(x + titleWidth - 32.0f, top - 1.0f);
		}
	glEnd();
}

void GLWWindow::drawMaximizedWindow()
{
	if (windowState_ & eTransparent)
	{
		{
			GLState newState(GLState::DEPTH_ON);
			// Do some cunning depth stuff to ensure that
			// anything drawn over the window will still be drawn
			// even if it is drawn with DEPTH_ON
			GLint func;
			glGetIntegerv(GL_DEPTH_FUNC, &func);
			glDepthFunc(GL_ALWAYS);
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, -90.0f);
				{
					GLState currentStateBlend(GLState::BLEND_ON);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
					glColor4f(0.0f, 0.0, 0.0f, 0.5f);
					drawBackSurface(x_, y_, w_, h_);
				}
				glColor3f(0.0f, 0.0f, 0.0f);
				drawSurround(x_, y_, w_, h_);
			glPopMatrix();
			glDepthFunc(func);
		}

		glPushMatrix();
			GLWVisiblePanel::draw();
		glPopMatrix();
	}
	else
	{
		{
			GLState currentStateBlend(GLState::BLEND_ON);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
			glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
			drawBackSurface(x_ + shadowWidth, y_ - shadowWidth, w_, h_);
		}
		{
			glColor3f(0.8f, 0.8f, 1.0f);
			drawBackSurface(x_, y_, w_, h_);
		}
		if (showTitle_ && !(windowState_ & eSmallTitle)) drawTitleBar(x_, y_, w_, h_);
		glColor3f(0.0f, 0.0f, 0.0f);
		drawSurround(x_, y_, w_, h_);

		glPushMatrix();
			GLWVisiblePanel::draw();
		glPopMatrix();
	}
}

void GLWWindow::draw()
{
	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF);
	if (needCentered_)
	{
		static float fVPort[4];
		glGetFloatv(GL_VIEWPORT, fVPort);

		setX((fVPort[2] - getW()) / 2.0f);
		setY((fVPort[3] - getH()) / 2.0f);
		needCentered_ = false;
	}

	drawMaximizedWindow();
}

void GLWWindow::mouseDown(float x, float y, bool &skipRest)
{
	if (disabled_) return;

	static float fVPort[4];
	glGetFloatv(GL_VIEWPORT, fVPort);
	float currentTop = fVPort[3];

	y = currentTop - y;

	if (x > x_ && x < x_ + w_)
	{
		float th = titleHeight;
		if (windowState_ & eSmallTitle) th = smallTitleHeight;
		if (y > y_ + h_ && y < y_ + h_ + th && showTitle_ && x < x_ + titleWidth)
		{
			// Start window drag
			dragging_ = TitleDrag;
			skipRest = true;
		}
		else
		{
			if ((windowState_ & eResizeable) && 
				inBox(x, y, x_ + w_ - 12.0f, y_, 12.0f, 12.0f))
			{
				// Start resize window drag
				dragging_ = SizeDrag;
				skipRest = true;
			}
			else if (y > y_ && y < y_ + h_)
			{
				// There is a mouse down in the actual window
				GLWVisiblePanel::mouseDown(x, y, skipRest);
				skipRest = true;
			}
		}	
	}
}

void GLWWindow::mouseUp(float x, float y, bool &skipRest)
{
	if (disabled_) return;

	dragging_ = NoDrag;
	GLWVisiblePanel::mouseUp(x, y, skipRest);
}

void GLWWindow::mouseDrag(float mx, float my, float x, float y, bool &skipRest)
{
	if (disabled_) return;

	static float fVPort[4];
	glGetFloatv(GL_VIEWPORT, fVPort);
	float currentTop = fVPort[3];

	my = currentTop - my;

	switch(dragging_)
	{
	case TitleDrag:
		x_ += x;
		y_ -= y;
		skipRest = true;
		break;
	case SizeDrag:
		if (w_ + x > minWindowSize)
		{
			w_ += x;
		}
		if (h_ + y > minWindowSize)
		{
			h_ += y;
			y_ -= y;
		}

		skipRest = true;
		break;
	default:
		GLWVisiblePanel::mouseDrag(mx, my, x, -y, skipRest);
		break;
	}
}

void GLWWindow::keyDown(char *buffer, int bufferCount, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	if (disabled_) return;

	GLWVisiblePanel::keyDown(buffer, bufferCount, history, hisCount, skipRest);
}
