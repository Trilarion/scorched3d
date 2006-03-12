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

#include <GLW/GLWLabel.h>
#include <GLW/GLWFont.h>

REGISTER_CLASS_SOURCE(GLWLabel);

GLWLabel::GLWLabel(float x, float y, char *buttonText, float size) : 
	GLWidget(x, y, 0.0f, 20.0f), size_(size),
	color_(GLWFont::widgetFontColor)
{
	if (buttonText) setText(buttonText);
	tooltipTransparent_ = true;
}

GLWLabel::~GLWLabel()
{

}

void GLWLabel::setColor(const Vector &color)
{
	color_ = color;
}

void GLWLabel::setSize(float size)
{
	size_ = size;
	w_ = 0.0f;
}

void GLWLabel::setText(const char *text)
{ 
	buttonText_ = text; 
	w_ = 0.0f;
}

void GLWLabel::calcWidth()
{
	if (w_ == 0.0f) w_ = GLWFont::instance()->getLargePtFont()->getWidth(
		size_, (char *) buttonText_.c_str());
}

void GLWLabel::draw()
{
	GLWidget::draw();

	glColor3f(1.0f, 0.0f, 0.0f);
	calcWidth();
	GLWFont::instance()->getLargePtFont()->draw(
		color_, size_,
		x_, y_ + 6.0f, 0.0f, (char *) buttonText_.c_str());
}
