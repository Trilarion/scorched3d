////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <target/TargetDamageCalc.h>
#include <tank/TankContainer.h>
#include <target/TargetLife.h>
#include <target/TargetSpace.h>
#include <common/Logger.h>
#include <engine/ActionController.h>
#include <actions/TankDamage.h>

void TargetDamageCalc::explosion(ScorchedContext &context,
							   Weapon *weapon,WeaponFireContext &weaponContext,
							   FixedVector &position, fixed radius,
							   fixed damageAmount, bool checkFall,
							   bool shieldOnlyDamage)
{
	std::map<unsigned int, Target *> collisionTargets;
	context.getTargetSpace().getCollisionSet(position, radius, collisionTargets, true);
	std::map<unsigned int, Target *>::iterator itor;
	for (itor = collisionTargets.begin();
		itor != collisionTargets.end();
		itor++)
	{
		Target *current = (*itor).second;
		if (!current->getAlive()) continue;
		
		// Check if the explosion causes damage
		fixed dist = current->getLife().collisionDistance(position);
		if (dist < radius)
		{
			// Direct hit by explosion
			fixed damage = 100;
			if (dist > radius / 3)
			{
				damage = ((dist - (radius / 3)) / (radius * fixed(true, 6600))) * 100;
				damage = fixed(100) - damage;
			}

			damageTarget(context, current, weapon, weaponContext, 
				damage * damageAmount, true, checkFall, shieldOnlyDamage);
		}
		else 
		{
			FixedVector &currentPosition = current->getLife().getCenterPosition();
			FixedVector direction = position - currentPosition;
			fixed dist2d = (direction[0] * direction[0] + 
				direction[1] * direction[1]).sqrt();
			if (dist2d < radius + 5)
			{
				// explosion under tank
				damageTarget(context, current, weapon, weaponContext, 
					0, true, checkFall, shieldOnlyDamage);
			}
		}
	}
}

void TargetDamageCalc::damageTarget(ScorchedContext &context,
								Target *target, Weapon *weapon, 
								WeaponFireContext &weaponContext, fixed damage,
								bool useShieldDamage, bool checkFall,
								bool shieldOnlyDamage)
{
	// Remove the correct damage from the tanks
	TankDamage *tankDamage = new TankDamage(
		weapon, target->getPlayerId(), weaponContext, 
		damage, useShieldDamage, checkFall, shieldOnlyDamage);
	context.getActionController().addAction(tankDamage);
}