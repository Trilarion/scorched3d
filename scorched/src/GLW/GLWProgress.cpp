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


// GLWProgress.cpp: implementation of the GLWProgress class.
//
//////////////////////////////////////////////////////////////////////

#include <GLEXT/GLState.h>
#include <GLW/GLWProgress.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GLWProgress::GLWProgress(float x, float y, float w, float min, float max) :
	GLWVisiblePanel(x, y, w, 25.0f, true), x_(x), y_(y), w_(w), min_(min), max_(max), current_(min)
{

}

GLWProgress::~GLWProgress()
{

}

void GLWProgress::draw()
{
	GLfloat width = ((current_ - min_) / (max_ - min_)) * (w_ - 4);

	GLWVisiblePanel::draw();

	glColor3f(0.4f, 0.4f, 0.5f);
	glBegin(GL_QUADS);
		glVertex2f(x_ + 2.0f, y_ + 2.0f);
		glVertex2f(x_ + width + 2.0f, y_ + 2.0f);
		glVertex2f(x_ + width + 2.0f, y_ + 22.0f);
		glVertex2f(x_ + 2.0f, y_ + 22.0f);
	glEnd();
}

void GLWProgress::setCurrent(float newCurrent)
{
	if (newCurrent < min_) current_ = min_;
	else if (newCurrent > max_) current_ = max_;
	else current_ = newCurrent;
}
