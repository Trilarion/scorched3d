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


#include <tank/TankLib.h>
#include <tank/TankContainer.h>
#include <common/OptionsTransient.h>
#include <common/Defines.h>
#include <math.h>

float TankLib::getDistanceToTank(Vector &position, Tank *targetTank)
{
	Vector currentdirection = 
			targetTank->getPhysics().getTankPosition() - position;
	float maxdistance2D = sqrtf(currentdirection[0] * 
		currentdirection[0] + currentdirection[1] * currentdirection[1]);
	return maxdistance2D;
}

void TankLib::getTanksSortedByDistance(Vector &position, 
									  std::list<std::pair<float, Tank *> > &result,
									  float maxDistance)
{
	std::list<std::pair<float, Tank *> > tankDistList;
	std::map<unsigned int, Tank *> &allCurrentTanks = 
		TankContainer::instance()->getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = allCurrentTanks.begin();
		itor != allCurrentTanks.end();
		itor++)
	{
		Tank *targetTank = (*itor).second;
		if (targetTank->getState().getState() == TankState::sNormal)
		{
			float maxdistance2D = getDistanceToTank(position, targetTank);
			tankDistList.push_back(std::pair<float, Tank*>(maxdistance2D, targetTank));
		}
	}

	while (!tankDistList.empty())
	{
		std::list<std::pair<float, Tank *> >::iterator removeItor = tankDistList.begin();
		std::list<std::pair<float, Tank *> >::iterator itor = tankDistList.begin(); itor++;
		for (;itor != tankDistList.end(); itor++)
		{
			if ((*itor).first <  (*removeItor).first) 
			{
				removeItor = itor;
			}
		}

		if ((*removeItor).first <= maxDistance || maxDistance == -1) result.push_back(*removeItor);
		tankDistList.erase(removeItor);
	}
}

void TankLib::getShotTowardsPosition(Vector &position, Vector &shootAt, float distForSniper, 
									float &angleXYDegs, float &angleYZDegs, float &power)
{
	// Calculate direction
	Vector direction = shootAt - position;
	float angleXYRads = atan2f(direction[1], direction[0]);
	angleXYDegs = (angleXYRads / 3.14f) * 180.0f - 90.0f;
	angleYZDegs = 45.0f;

	float distance2D = sqrtf(direction[0] * direction[0] + direction[1] * direction[1]);

	// Special case
	// If we are less than a certain distance and under the position we
	// will use a direct shot on full power
	if ((distance2D < distForSniper) && (shootAt[2] >= position[2]))
	{
		power = 1000.0f;
		float angleYZRads = atan2f(distance2D, direction[2]);
		angleYZDegs = 90.0f - ((angleYZRads / 3.14f) * 180.0f);
	}
	else
	{
		// Calculate power (very roughly)
		power = 1000.0f;
		if (distance2D < 200.0f)
		{
			power = 750;
			if (distance2D < 100.0f)
			{
				power = 550;
				if (distance2D < 50.0f)
				{
					power = 350;
				}
			}
		}

		// Add some randomness to the power
		power += (RAND * 200.0f) - 100.0f;
		if (power < 100) power = 100;

		if (OptionsTransient::instance()->getWindOn())
		{
			// Make less adjustments for less wind
			float windMag = OptionsTransient::instance()->getWindSpeed() / 5.0f;

			// Try to account for the wind direction
			Vector ndirection = direction;
			ndirection[2] = 0.0f;
			ndirection = ndirection.Normalize();
			ndirection = ndirection.get2DPerp();
			float windoffsetLR = OptionsTransient::instance()->getWindDirection().dotP(ndirection);
			angleXYDegs += windoffsetLR * distance2D * (0.12f + RAND * 0.04f) * windMag;

			float windoffsetFB = OptionsTransient::instance()->getWindDirection().dotP(direction.Normalize());
			windoffsetFB /= 10.0f;
			windoffsetFB *= windMag;
			windoffsetFB += 1.0f; // windowoffset FB 0.9 > 1.1

			power *= windoffsetFB;
		}
	}
}

Vector &TankLib::getVelocityVector(float xy, float yz)
{
	static Vector diff;
	diff = Vector((float) sinf(-xy / 180.0f * 3.14f) *
				cosf(yz / 180.0f * 3.14f),
				(float) cosf(-xy / 180.0f * 3.14f) * 
				cosf(yz / 180.0f * 3.14f),
				(float) sinf(yz/ 180.0f * 3.14f));	
	diff /= 20.0f;
	return diff;
}