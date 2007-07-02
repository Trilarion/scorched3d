////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_Water2Patchh_INCLUDE__)
#define __INCLUDE_Water2Patchh_INCLUDE__

#include <water/Water2PatchIndexs.h>
#include <common/Vector.h>
#include <vector>

class GLVertexBufferObject;
class Water2Patch
{
public:
	Water2Patch();
	~Water2Patch();

	struct Data
	{
		// Fix me, we don't need all this info all the time!
		float x, y, z;
		float nx, ny, nz;
	};

	void generate(std::vector<Vector> &heights,
		int size, int totalSize,
		int posX, int posY,
		float waterHeight);
	void draw(Water2PatchIndexs &indexes, 
		int indexPosition, int borders);

	Data *getData(int x, int y);

protected:
	int size_;
	Data *data_;
	GLVertexBufferObject *bufferObject_;

	void draw(Water2PatchIndex &index);
};

#endif // __INCLUDE_Water2Patchh_INCLUDE__