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

#include <GLW/GLWDropDown.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWFont.h>
#include <client/ScorchedClient.h>

GLWDropDownI::~GLWDropDownI()
{

}

GLWDropDown::GLWDropDown(float x, float y, float w) :
	GLWVisibleWidget(x, y, w - 21, 25.0f), text_("None"), 
	button_(x + w - 20.0f, y, 20.0f, 25.0f),
	handler_(0)
{

}

GLWDropDown::~GLWDropDown()
{

}

void GLWDropDown::setHandler(GLWDropDownI *handler)
{
	handler_ = handler;
}

void GLWDropDown::clear()
{
	texts_.clear();
}

int GLWDropDown::getCurrentPosition()
{
	std::list<std::string>::iterator itor;
	int pos = 0;
	for (itor = texts_.begin();
		itor != texts_.end();
		itor++, pos++)
	{
		if (strcmp(text_.c_str(), (*itor).c_str()) == 0)
		{
			return pos;
		}
	}

	return -1;
}

void GLWDropDown::setCurrentPosition(int pos)
{
	int position = 0;
	std::list<std::string>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		itor++)
	{
		text_ = (*itor);
		if (position++ >= pos) break;
	}
}

void GLWDropDown::addText(const char *text)
{
	if (texts_.empty())
	{
		text_ = text;
	}
	texts_.push_back(text);
}

void GLWDropDown::draw()
{
	static int iVPort[4];
	glGetIntegerv(GL_VIEWPORT, iVPort);
	int windowHeight = iVPort[3];

	float mouseX = float(ScorchedClient::instance()->getGameState().getMouseX());
	mouseX -= GLWTranslate::getPosX();
	float mouseY = float(windowHeight - ScorchedClient::instance()->getGameState().getMouseY());
	mouseY -= GLWTranslate::getPosY();

	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 10.0f, false);
	glEnd();

	button_.draw();
	float offset = 0.0f;
	if(button_.getPressed()) offset = 1.0f;

	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_TRIANGLES);
		glVertex2d(x_ + w_ + 16.0f + offset, y_ + 17.0f - offset);
		glVertex2d(x_ + w_ + 5.0f + offset, y_ + 17.0f - offset);
		glVertex2d(x_ + w_ + 10.0f + offset, y_ + 7.0f - offset);
	glEnd();

	GLWFont::instance()->getFont()->draw(
		GLWFont::widgetFontColor, 16,
		x_ + 5.0f, y_ + 5.0f, 0.0f, text_.c_str());

	if (button_.getPressed())
	{
		glColor3f(0.8f, 0.8f, 1.0f);
		float dropSize = float(20.0f * texts_.size());
		glBegin(GL_QUADS);
			glVertex2f(x_, y_ - dropSize - 1);
			glVertex2f(x_ + w_, y_ - dropSize - 1);
			glVertex2f(x_ + w_, y_ - 1);
			glVertex2f(x_, y_ - 1);
		glEnd();
		glBegin(GL_LINE_LOOP);
			drawShadedRoundBox(x_, y_ - dropSize - 1, w_, dropSize, 10.0f, true);
		glEnd();

		float top = y_ - 24.0f;
		std::list<std::string>::iterator itor;
		for (itor = texts_.begin();
			itor != texts_.end();
			itor++)
		{
			static Vector selectedColor(0.7f, 0.7f, 0.7f);
			bool selected = 
				inBox(mouseX, mouseY, x_, top, w_, 19.0f);

			GLWFont::instance()->getFont()->draw(
				selected?selectedColor:GLWFont::widgetFontColor, 16,
				x_ + 5.0f, top + 5.0f, 0.0f, (*itor).c_str());
			top -= 20.0f;
		}
	}
}

void GLWDropDown::mouseDown(float x, float y, bool &skipRest)
{
	button_.mouseDown(x, y, skipRest);
	if (!skipRest)
	{
		if (button_.getPressed())
		{
			button_.getPressed() = false;
			skipRest = true;

			float dropSize = float(20.0f * texts_.size());
			if (inBox(x, y, x_, y_ - dropSize - 1, w_, dropSize))
			{
				int pos = 0;
				float top = y_ - 24.0f;
				std::list<std::string>::iterator itor;
				for (itor = texts_.begin();
					itor != texts_.end();
					itor++)
				{
					if (inBox(x, y, x_, top, w_, 19.0f))
					{
						text_ = (*itor);
						if (handler_)
						{
							handler_->select(id_, pos, text_.c_str());
							return;
						}
					}

					pos++;
					top -= 20.0f;
				}
			}
		}
	}
}

void GLWDropDown::mouseUp(float x, float y, bool &skipRest)
{
	button_.mouseUp(x, y, skipRest);
}
