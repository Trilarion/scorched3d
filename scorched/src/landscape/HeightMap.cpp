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


// HeightMap.cpp: implementation of the HeightMap class.
//
//////////////////////////////////////////////////////////////////////

#include <common/Triangle.h>
#include <landscape/HeightMap.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HeightMap::HeightMap(const int width) :
	width_(width)
{
	hMap_ = new float[(width_ + 1) * (width_ + 1)];
	normals_ = new Vector[(width_ + 1) * (width_ + 1)];

	reset();
}

HeightMap::~HeightMap()
{
	delete [] hMap_;
	delete [] normals_;
}

void HeightMap::reset()
{
	memset(hMap_, 0, sizeof(float)  * (width_ + 1) * (width_ + 1));
	memset(normals_, 0, sizeof(Vector)  * (width_ + 1) * (width_ + 1));
}

bool HeightMap::getVector(Vector &vec, int x, int y)
{
	if (x < 0 || y < 0 || x>width_ || y>width_) return false;

	vec[0] = (float) x;
	vec[1] = (float) y;
	vec[2] = getHeight(x,y);
	return true;
}

void HeightMap::getVectorPos(int pos, int &x, int &y, int dist)
{
	switch (pos)
	{
	case 0:
		x=-dist; y=0;
		break;
	case 1:
		x=0; y=dist;
		break;
	case 2:
		x=dist; y=0;
		break;
	default:
		x=0; y=-dist;
	}
}

void HeightMap::generateNormals(int minX, int maxX, int minY, int maxY, ProgressCounter *counter)
{
	if (counter) counter->setNewOp("Normals");

	for (int x=minX; x<=maxX; x++)
	{
		if (counter) counter->setNewPercentage((100.0f * float(x - minX)) / float(maxX - minX));

		for (int y=minY; y<=maxY; y++)
		{
			static Vector C;
			if (!getVector(C, x, y)) continue;

			static Vector total;
			total.zero();

			int times = 0;
			for (int dist=1; dist<=3; dist+=2)
			{
				for (int a=0, b=1; a<4; a++, b++)
				{
					if (b>3) b=0;

					static Vector A;
					int aPosX, aPosY;
					getVectorPos(a, aPosX, aPosY, dist);
					if (!getVector(A, aPosX + x, aPosY + y)) continue;

					static Vector B;
					int bPosX, bPosY;				
					getVectorPos(b, bPosX, bPosY, dist);
					if (!getVector(B, bPosX + x, bPosY + y)) continue;

					A-=C;
					B.StoreInvert();
					B+=C;
					A *= B;
					A.StoreNormalize();
					total += A;
					times += 1;
				}

				if (times > 4) break;
			}

			getNormal(x, y) = total / (float) times;
		}
	}
}

float HeightMap::getDist(Vector &start, Vector &dir, Vector &pos)
{
	Vector dir2 = dir.get2DPerp();

	float u1 = dir.dotP(pos - start)/dir.dotP(dir);
	Vector pt = dir * u1 + start;
	
	return (pos - pt).Magnitude();
}

bool HeightMap::getIntersect(Line &direction, Vector &intersect)
{
	const float maxFloat = -999999;
	const int searchSquareWidth_ = 4;

	float maxdist = maxFloat;
	for (int x=0; x<width_ - searchSquareWidth_; x+=searchSquareWidth_)
	{
		for (int y=0; y<width_ - searchSquareWidth_; y+=searchSquareWidth_)
		{
			static Triangle triA;
			triA.setPointComponents(
				float(x), float(y), getHeight(x, y),
				getNormal(x, y),
				float(x + searchSquareWidth_), float(y), getHeight(x + searchSquareWidth_, y),
				getNormal(x + searchSquareWidth_, y),
				float(x + searchSquareWidth_), float(y + searchSquareWidth_), getHeight(x + searchSquareWidth_, y + searchSquareWidth_),
				getNormal(x + searchSquareWidth_, y + searchSquareWidth_));

			static Triangle triB;
			triB.setPointComponents(
				float(x + searchSquareWidth_), float(y + searchSquareWidth_), getHeight(x + searchSquareWidth_, y + searchSquareWidth_),
				getNormal(x + searchSquareWidth_, y+  searchSquareWidth_),
				float(x), float(y + searchSquareWidth_), getHeight(x, y + searchSquareWidth_),
				getNormal(x, y+  searchSquareWidth_),
				float(x), float(y), getHeight(x, y),
				getNormal(x, y));

			static Vector newInter, interN;
			float dist;
			if (triA.rayIntersect(direction, newInter, interN, dist, true))
			{
				if (dist > maxdist)
				{
					maxdist = dist;
					intersect = newInter;
				}
			}
			if (triB.rayIntersect(direction, newInter, interN, dist, true))
			{
				if (dist > maxdist)
				{
					maxdist = dist;
					intersect = newInter;
				}
			}
		}
	}

	return (maxdist != maxFloat);
}

float HeightMap::getInterpHeight(float w, float h)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=width_ && h<=width_);

	int ihx = (int) w; if (ihx == width_+1) ihx--;
	int ihy = (int) h; if (ihy == width_+1) ihy--;
	int ihx2 = ihx+1; if (ihx2 == width_+1) ihx2--;
	int ihy2 = ihy+1; if (ihy2 == width_+1) ihy2--;

	float fhx = w - (float) ihx;
	float fhy = h - (float) ihy;
	
	float &heightA = getHeight(ihx, ihy);
	float &heightB = getHeight(ihx, ihy2);
	float &heightC = getHeight(ihx2, ihy);
	float &heightD = getHeight(ihx2, ihy2);

	float heightDiffAB = heightB-heightA;
	float heightDiffCD = heightD-heightC;
	float heightE = heightA + (heightDiffAB * fhy);
	float heightF = heightC + (heightDiffCD * fhy);

	float heightDiffEF = heightF - heightE;
	float height = heightE + (heightDiffEF * fhx);	

	return height;
}

void HeightMap::getInterpNormal(float w, float h, Vector &normal)
{
	DIALOG_ASSERT(w >= 0 && h >= 0 && w<=width_ && h<=width_);

	int ihx = (int) w; if (ihx == width_+1) ihx--;
	int ihy = (int) h; if (ihy == width_+1) ihy--;
	int ihx2 = ihx+1; if (ihx2 == width_+1) ihx2--;
	int ihy2 = ihy+1; if (ihy2 == width_+1) ihy2--;

	float fhx = w - (float) ihx;
	float fhy = h - (float) ihy;

	Vector &normalA = getNormal(ihx, ihy);
	Vector &normalB = getNormal(ihx, ihy2);
	Vector &normalC = getNormal(ihx2, ihy);
	Vector &normalD = getNormal(ihx2, ihy2);

	static Vector normalDiffAB;
	normalDiffAB = normalB;
	normalDiffAB -= normalA;
	normalDiffAB *= fhy;
	static Vector normalDiffCD;
	normalDiffCD = normalD;
	normalDiffCD -= normalC;
	normalDiffCD *= fhy;

	static Vector normalE;
	normalE = normalA;
	normalE += normalDiffAB;
	static Vector normalF;
	normalF = normalC;
	normalF += normalDiffCD;

	static Vector normalDiffEF;
	normalDiffEF = normalF;
	normalDiffEF -= normalE;
	normalDiffEF *= fhx;

	normal = normalE;
	normal += normalDiffEF;
}
