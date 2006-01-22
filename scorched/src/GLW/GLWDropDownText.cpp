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

#include <GLW/GLWDropDownText.h>
#include <GLW/GLWFont.h>
#include <client/ScorchedClient.h>

REGISTER_CLASS_SOURCE(GLWDropDownText);

GLWDropDownText::GLWDropDownText(float x, float y, float w) :
	GLWDropDown(x, y, w)
{
}

GLWDropDownText::~GLWDropDownText()
{
}

void GLWDropDownText::addText(const char *text)
{
	GLWSelectorEntry entry(text);
	addEntry(entry);
}

const char *GLWDropDownText::getCurrentText()
{
	if (!getCurrentEntry()) return "";
	return getCurrentEntry()->getText();
}

void GLWDropDownText::setCurrentText(const char *text)
{
	int position = 0;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor = texts_.begin();
		itor != texts_.end();
		itor++)
	{
		GLWSelectorEntry &entry = *itor;
		if (0 == strcmp(text, entry.getText()))
		{
			current_ = &entry;
			break;
		}

		position++;
	}

	if (handler_)
	{
		handler_->select(id_, position, *current_);
	}
}

void GLWDropDownText::draw()
{
	GLWDropDown::draw();

	if (getCurrentEntry())
	{
		GLWFont::instance()->getLargePtFont()->drawWidth(
			(int) w_ - 25,
			GLWFont::widgetFontColor, 14,
			x_ + 5.0f, y_ + 5.0f, 0.0f,
			getCurrentEntry()->getText());
	}
}