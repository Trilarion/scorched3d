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


// TalkRenderer.h: interface for the TalkRenderer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TalkRenderer_H__53C71D24_C1E9_41C2_8757_FF947E1B4425__INCLUDED_)
#define AFX_TalkRenderer_H__53C71D24_C1E9_41C2_8757_FF947E1B4425__INCLUDED_

#include <engine/Action.h>
#include <common/Vector.h>
#include <GLEXT/GLTexture.h>

class TalkRenderer : public ActionRenderer 
{
public:
	TalkRenderer(Vector &position);
	virtual ~TalkRenderer();

	virtual void simulate(Action *action, float timepassed, bool &remove);
	virtual void draw(Action *action);

protected:
	float frameTime_;
	Vector position_;
	static GLTexture talkTex_;

};

#endif // !defined(AFX_TalkRenderer_H__53C71D24_C1E9_41C2_8757_FF947E1B4425__INCLUDED_)
