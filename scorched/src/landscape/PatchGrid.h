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


// PatchGrid.h: interface for the PatchGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATCHGRID_H__78A69B7D_B848_4675_B017_13626E4856D1__INCLUDED_)
#define AFX_PATCHGRID_H__78A69B7D_B848_4675_B017_13626E4856D1__INCLUDED_


#include <landscape/HeightMap.h>
#include <landscape/Patch.h>

class PatchGrid  
{
public:
	PatchGrid(HeightMap *hMap, int patchSize);
	virtual ~PatchGrid();

	void recalculate(int posX, int posY, int dist);
	void draw(PatchSide::DrawType sides);

protected:
	Vector lastPos_;
	int width_;
	int patchSize_;
	Patch **patches_;
	HeightMap *hMap_;

	void reset();
};

#endif // !defined(AFX_PATCHGRID_H__78A69B7D_B848_4675_B017_13626E4856D1__INCLUDED_)
