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


// TankSort.h: interface for the TankSort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANKSORT_H__21F68DAD_9352_4673_9E24_B869AD5645AE__INCLUDED_)
#define AFX_TANKSORT_H__21F68DAD_9352_4673_9E24_B869AD5645AE__INCLUDED_

#include <list>
#include <engine/ScorchedContext.h>

class Tank;
namespace TankSort
{
	struct SortOnScore
	{
		bool operator()(const Tank *x, const Tank *y, ScorchedContext &context) const;
	};

	int compare(ScorchedContext &context,
		int kills1, int money1, int wins1, const char *name1,
		int kills2, int money2, int wins2, const char *name2);

	int getWinningTeam(ScorchedContext &context);
	void getSortedTanks(std::list<Tank *> &list, ScorchedContext &context);
	void getSortedTanksIds(ScorchedContext &context, 
		std::list<unsigned int> &list, bool allTanks = false);
};

#endif // !defined(AFX_TANKSORT_H__21F68DAD_9352_4673_9E24_B869AD5645AE__INCLUDED_)
