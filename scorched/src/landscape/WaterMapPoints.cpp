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


#include <landscape/WaterMapPoints.h>

WaterMapPoints::WaterMapPoints(WaterMap &map, int width, int points) :
	pts_(0), noPts_(0), listNo_(0)
{
	obj_ = gluNewQuadric();
	height_ = map.getHeight();
	createPoints(map, width, points);
}

WaterMapPoints::~WaterMapPoints()
{
	gluDeleteQuadric(obj_);
	delete [] pts_;
}

void WaterMapPoints::draw()
{
	GLState currentState(GLState::TEXTURE_OFF);
	Position *current = pts_;
	for (int i=0; i<noPts_; i++)
	{
		glPushMatrix();
			glTranslatef(current->x, current->y, height_ + *current->z);
			if (!listNo_)
			{
				glNewList(listNo_ = glGenLists(1), GL_COMPILE_AND_EXECUTE);
					gluSphere(obj_, 1.0f, 8, 8);
				glEndList();
			}
			else
			{
				glCallList(listNo_);
			}
			//glVertex3f(current->x, current->y, height_ + *current->z);
		glPopMatrix();
		current++;
	}
}

void WaterMapPoints::createPoints(WaterMap &map, int width, int points)
{
	noPts_ = points * 4 - 4;
	pts_ = new Position[noPts_];
	Position *current = pts_;
	int i;
	for (i=0; i<points; i++)
	{
		float pos = float(width) / float(points-1) * float(i);

		findPoint(map, current++, pos, 0.0f);
		findPoint(map, current++, pos, float(width));
	}
	for (i=1; i<points-1; i++)
	{
		float pos = float(width) / float(points-1) * float(i);

		findPoint(map, current++, 0.0f, pos);
		findPoint(map, current++, float(width), pos);
	}
}

void WaterMapPoints::findPoint(WaterMap &map, Position *pos, float x, float y)
{
	Vector point(x, y);
	pos->x = x;
	pos->y = y;
	pos->z = &map.getNearestWaterPoint(point).height;
}
