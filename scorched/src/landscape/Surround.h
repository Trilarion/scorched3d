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


// Surround.h: interface for the Surround class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SURROUND_H__E92604A2_6E7D_4810_9685_B699CE743B19__INCLUDED_)
#define AFX_SURROUND_H__E92604A2_6E7D_4810_9685_B699CE743B19__INCLUDED_

#include <GLEXT/GLState.h>
#include <GLEXT/GLTexture.h>
#include <landscape/SurroundDefs.h>

class Surround
{
public:
	Surround(SurroundDefs &defs);
	virtual ~Surround();

	void simulate(float frameTime);
	void draw();

protected:
	static Surround *instance_;
	float xy_;
	float cloudSpeed_;
	float cloudDirection_;

};

#endif // !defined(AFX_SURROUND_H__E92604A2_6E7D_4810_9685_B699CE743B19__INCLUDED_)
