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


// SmokeChain.h: interface for the SmokeChain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMOKECHAIN_H__23478194_10E3_48AB_A956_ED2B21618665__INCLUDED_)
#define AFX_SMOKECHAIN_H__23478194_10E3_48AB_A956_ED2B21618665__INCLUDED_

#include <common/Vector.h>

class SmokeChainEntry
{
public:
	void set(Vector &position, Vector &velocity, float maxS, float maxT);
	void draw(Vector &bilX, Vector &bilY);
	bool move(float frameTime);

	Vector position_; // Smoke position
	Vector velocity_; // Smoke velocity
	float a, t; // Alpha and time
	float maxTime, maxSize; // Max time, max size
	int texCoordType; // Which orientation of tex coord to use

};

class SmokeChain  
{
public:
	SmokeChain(int numberOfEntries);
	virtual ~SmokeChain();

	SmokeChainEntry *getNextEntry();
	void removeEntry();

	SmokeChainEntry *getFirst();
	SmokeChainEntry *getNext();

	int getNoEntries() { return noEntries_; }

protected:
	int noEntries_, maxNoEntries_;
	SmokeChainEntry *nextEntry_, *lastEntry_, *firstEntry_;
	SmokeChainEntry *entries_;

	SmokeChainEntry *iter_;
};

#endif // !defined(AFX_SMOKECHAIN_H__23478194_10E3_48AB_A956_ED2B21618665__INCLUDED_)
