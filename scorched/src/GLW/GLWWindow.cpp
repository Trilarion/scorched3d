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

#include <GLEXT/GLState.h>
#include <GLEXT/GLTexture.h>
#include <GLEXT/GLBitmap.h>
#include <GLEXT/GLViewPort.h>
#include <XML/XMLParser.h>
#include <GLW/GLWWindow.h>
#include <GLW/GLWWindowManager.h>
#include <client/ScorchedClient.h>

static const float roundSize = 20.0f;
static const float smallRoundSize = 10.0f;
static const float smallTitleHeight = 14.0f;
static const float minWindowSize = 50.0f;
static const float titleWidth = 100.0f;
static const float titleHeight = 20.0f;
static const float shadowWidth = 10.0f;

GLWWindow::GLWWindow(const char *name, float x, float y, 
					 float w, float h,
					 unsigned int states,
					 const char *description) : 
	GLWPanel(x, y, w, h), dragging_(NoDrag), 
	needCentered_(false), showTitle_(false), name_(name),
	disabled_(false), windowState_(states), maxWindowSize_(0.0f),
	description_(description), toolTip_(name, description),
	initPosition_(false), windowLevel_(100000)
{
	getDrawPanel() = false;
}

GLWWindow::GLWWindow(const char *name, float w, float h,
					 unsigned int states,
					 const char *description) :
	GLWPanel(0.0f, 0.0f, w, h), dragging_(NoDrag), 
	needCentered_(true), showTitle_(false), name_(name),
	disabled_(false), windowState_(states),
	description_(description), toolTip_(name, description),
	windowLevel_(100000)
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

void GLWWindow::windowHide()
{

}

void GLWWindow::drawOutlinePoints(float x, float y, float w, float h)
{
	float tw = titleWidth;
	if (w < titleWidth + 25.0f) tw = w - 25.0f;

	float th = titleHeight;
	float sr = roundSize;
	float sz = 20.0f;
	if (windowState_ & eSmallTitle)
	{
		th = smallTitleHeight;
		sr = smallRoundSize;
		sz = 10.0f;
	}

	if (windowState_ & eResizeable)
		drawCircle(8, 4, x + w - smallRoundSize, y + smallRoundSize, 10.0f);
	else
		drawCircle(8, 4, x + w - roundSize, y + roundSize, 20.0f);
	drawCircle(4, 0, x + w - roundSize, y + h - roundSize, 20.0f);
	if (((tw > 70.0f) || (tw > 50.0f && windowState_ & eSmallTitle)) && 
		!(windowState_ & eNoTitle))
	{
		showTitle_ = true;
		drawCircle(8, 10, x + tw, y + h + sr, sz);
		drawCircle(2, 0, x + tw - sr - sr, y + h + th - sr, sz);
		drawCircle(0, -4, x + roundSize, y + h + th - roundSize, 20.0f);
	}
	else
	{
		showTitle_ = false;
		drawCircle(0, -4, x + roundSize, y + h - roundSize, 20.0f);
	}
	drawCircle(-4, -8, x + roundSize, y + roundSize, 20.0f);
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

void GLWWindow::drawWindowCircle(float x, float y, float w, float h)
{
	glColor4f(0.5f, 0.5f, 1.0f, 0.3f);	
	float halfWidth = w / 2.0f;
	float halfHeight = h / 2.0f;
	glPushMatrix();
		glTranslatef(x + halfWidth, y + halfHeight, 0.0f);
		glScalef(halfWidth, halfHeight, 0.0f);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(0.0f, 0.0f);
			drawWholeCircle(true);
		glEnd();
		glColor4f(0.9f, 0.9f, 1.0f, 0.5f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
			drawWholeCircle(false);
		glEnd();
		glLineWidth(1.0f);
	glPopMatrix();
}

void GLWWindow::drawMaximizedWindow()
{
	if (windowState_ & eCircle ||
		windowState_ & eNoDraw)
	{
		if (!moveTexture_.textureValid())
		{
			GLBitmap moveMap(getDataFile("data/windows/move.bmp"), true);
			moveTexture_.create(moveMap, GL_RGBA, false);
		}

		if (windowState_ & eCircle)
		{
			GLState state(GLState::BLEND_ON);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
			drawWindowCircle(x_, y_, w_, h_);
		}

		glPushMatrix();
			GLWPanel::draw();
		glPopMatrix();

		int x = ScorchedClient::instance()->getGameState().getMouseX();
		int y = ScorchedClient::instance()->getGameState().getMouseY();
		if (GLWWindowManager::instance()->getFocus(x, y) == getId())
		{
			if (windowState_ & eResizeable &&
				windowState_ & eCircle)
			{
				drawWindowCircle(x_ + w_ - 12.0f, y_, 12.0f, 12.0f);
			}
			if (!(windowState_ & eNoTitle))
			{
				float sizeX = 20.0f;
				float sizeY = 20.0f;

				static GLWTip moveTip("Move",
					"Left click and drag to move the window.");
				GLWToolTip::instance()->addToolTip(&moveTip, 
					x_, y_ + h_ - sizeY, sizeX, sizeY);

				GLState currentStateBlend(GLState::BLEND_ON | GLState::TEXTURE_ON);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
				moveTexture_.draw();
				glColor4f(0.8f, 0.0f, 0.0f, 0.8f);
				glPushMatrix();
					glTranslatef(x_, y_ + h_ - sizeY, 0.0f);
					glScalef(sizeX / 16.0f, sizeY / 16.0f, 1.0f);
					drawIconBox(0.0f, 0.0f);
				glPopMatrix();
			}
		}
	}
	else if (windowState_ & eTransparent)
	{
		{
			// NOTE WE DONT NEED CUNNING STUFF NOW
			// AS WE DO A DEPTH CLEAR IN 2DCAMERA

			//GLState newState(GLState::DEPTH_ON);
			// Do some cunning depth stuff to ensure that
			// anything drawn over the window will still be drawn
			// even if it is drawn with DEPTH_ON
			//GLint func;
			//glGetIntegerv(GL_DEPTH_FUNC, &func);
			//glDepthFunc(GL_ALWAYS);
			glPushMatrix();
				glTranslatef(0.0f, 0.0f, 0.0f);
				{
					GLState currentStateBlend(GLState::BLEND_ON);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
					glColor4f(0.4f, 0.6f, 0.8f, 0.6f);
					drawBackSurface(x_, y_, w_, h_);
				}
				glLineWidth(2.0f);
				glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
				drawSurround(x_, y_, w_, h_);
				glLineWidth(1.0f);
			glPopMatrix();
			//glDepthFunc(func);
		}

		glPushMatrix();
			GLWPanel::draw();
		glPopMatrix();
	}
	else
	{
		GLState currentStateBlend(GLState::BLEND_ON);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		{
			glColor3f(0.8f, 0.8f, 1.0f);
			drawBackSurface(x_, y_, w_, h_);
		}
		if (showTitle_ && !(windowState_ & eSmallTitle)) drawTitleBar(x_, y_, w_, h_);
		glLineWidth(2.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		drawSurround(x_, y_, w_, h_);
		glLineWidth(1.0f);

		glPushMatrix();
			GLWPanel::draw();
		glPopMatrix();
	}
}

void GLWWindow::draw()
{
	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF);

	if (!initPosition_)
	{
		if (x_ < 0.0f) setX(GLViewPort::getWidth() + x_);
		if (y_ < 0.0f) setY(GLViewPort::getHeight() + y_);
		initPosition_ = true;
	}
	if (needCentered_)
	{
		int wWidth = GLViewPort::getWidth();
		int wHeight = GLViewPort::getHeight();
		setX((wWidth - getW()) / 2.0f);
		setY((wHeight - getH()) / 2.0f);
		needCentered_ = false;
	}

	drawMaximizedWindow();
}

void GLWWindow::mouseDown(float x, float y, bool &skipRest)
{
	if (disabled_) return;

	if (x > x_ && x < x_ + w_)
	{
		if (windowState_ & eCircle ||
			windowState_ & eNoDraw)
		{
			float sizeX = w_ / 5.6f;
			float sizeY = w_ / 5.6f;
			if (x < x_ + sizeX &&
				y < y_ + h_ &&
				y > y_ + h_ - sizeY)
			{
				// Start window drag
				dragging_ = TitleDrag;
				skipRest = true;
			}
			else if (y > y_ && y < y_ + h_)
			{
				// There is a mouse down in the actual window
				GLWPanel::mouseDown(x, y, skipRest);
				skipRest = true;
			}
		}
		else
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
					GLWPanel::mouseDown(x, y, skipRest);
					skipRest = true;
				}
			}	
		}
	}
}

void GLWWindow::mouseUp(float x, float y, bool &skipRest)
{
	if (disabled_) return;

	dragging_ = NoDrag;
	GLWPanel::mouseUp(x, y, skipRest);
}

void GLWWindow::mouseDrag(float mx, float my, float x, float y, bool &skipRest)
{
	if (disabled_) return;

	switch(dragging_)
	{
	case TitleDrag:
		x_ += x;
		y_ -= y;
		skipRest = true;
		break;
	case SizeDrag:
		if (w_ + x > minWindowSize &&
			(maxWindowSize_ == 0.0f || w_ + x < maxWindowSize_))
		{
			w_ += x;
		}
		if (h_ + y > minWindowSize &&
			(maxWindowSize_ == 0.0f || h_ + y < maxWindowSize_))
		{
			h_ += y;
			y_ -= y;
		}

		skipRest = true;
		break;
	default:
		GLWPanel::mouseDrag(mx, my, x, -y, skipRest);
		break;
	}
}

void GLWWindow::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	if (disabled_) return;

	GLWPanel::keyDown(buffer, keyState, history, hisCount, skipRest);
}

void GLWWindow::drawInfoBox(float x, float y, float w)
{
	glColor4f(0.5f, 0.5f, 1.0f, 0.3f);	
	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(x + 20.0f, y - 8.0f);
		glVertex2f(x + 20.0f, y - 18.0f);
		drawRoundBox(x, y - 18.0f, w, 18.0f, 9.0f);
		glVertex2f(x + 20.0f, y - 18.0f);
	glEnd();

	glColor4f(0.9f, 0.9f, 1.0f, 0.5f);
	glLineWidth(2.0f);
	glBegin(GL_LINE_LOOP);
		drawRoundBox(x, y - 18.0f, w, 18.0f, 9.0f);
	glEnd();
	glLineWidth(1.0f);
}

void GLWWindow::drawJoin(float x, float y)
{
	glColor4f(0.8f, 0.8f, 0.9f, 0.8f);
	glPushMatrix();
		glTranslatef(x, y, 0.0f);
		glScalef(3.0f, 3.0f, 3.0f);
		glBegin(GL_TRIANGLE_FAN);
			drawWholeCircle(true);
		glEnd();
	glPopMatrix();
}

void GLWWindow::drawIconBox(float x, float y)
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x, y);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x + 16.0f, y);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x + 16.0f, y + 16.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x, y + 16.0f);
	glEnd();
}

bool GLWWindow::initFromXML(XMLNode *node)
{
	if (!GLWPanel::initFromXML(node)) return false;
	
	// Name
	XMLNode *nameNode = node->getNamedChild("name", true, true);
	if (!nameNode) return false; name_ = nameNode->getContent();

	// Desc
	XMLNode *descNode = node->getNamedChild("description", true, true);
	if (!descNode) return false; description_ = descNode->getContent();
	toolTip_.setText(name_.c_str(), description_.c_str());

	return true;
}
