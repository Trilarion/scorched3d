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

#include <engine/PlayShots.h>
#include <engine/ScorchedContext.h>
#include <engine/ActionController.h>
#include <tank/TankScore.h>
#include <tank/TankState.h>
#include <tank/TankAccessories.h>
#include <tank/TankPosition.h>
#include <actions/TankFired.h>
#include <actions/TankResign.h>
#include <target/TargetLife.h>
#include <common/OptionsGame.h>
#include <common/StatsLogger.h>
#include <weapons/AccessoryStore.h>

PlayShots::PlayShots()
{
}

PlayShots::~PlayShots()
{
}

void PlayShots::createMessage(ComsPlayMovesMessage &message)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		message.getMoves()[(*itor).first] = (*itor).second;
	}
}

void PlayShots::readMessage(ComsPlayMovesMessage &message)
{
	clearShots();
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = message.getMoves().begin();
		itor != message.getMoves().end();
		itor++)
	{
		messages_[(*itor).first] = (*itor).second;
	}
}

void PlayShots::clearShots()
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		delete (*itor).second;
	}
	messages_.clear();
}

bool PlayShots::haveShot(unsigned int playerId)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor =
		messages_.find(playerId);
	return (itor != messages_.end());
}

void PlayShots::playShots(ScorchedContext &context, bool roundStart)
{
	std::map<unsigned int, ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages_.begin();
		itor != messages_.end();
		itor++)
	{
		unsigned int playerId = (*itor).first;
		ComsPlayedMoveMessage *message = (*itor).second;

		// Check the tank exists for this player
		// It may not if the player has left the game after firing.
		Tank *tank = context.tankContainer->getTankById(playerId);
		if (tank)
		{
			// This tank has now made a move, reset its missed move counter
			tank->getScore().setMissedMoves(0);

			// Actually play the move
			processPlayedMoveMessage(
				context, *message, tank, roundStart);
		}
	}
}

void PlayShots::processPlayedMoveMessage(ScorchedContext &context, 
	ComsPlayedMoveMessage &message, Tank *tank, bool roundStart)
{
	if (roundStart)
	{
		// All actions that are done at the very START of a new round
		switch (message.getType())
		{
			case ComsPlayedMoveMessage::eShot:
				processFiredMessage(context, message, tank);
				break;
			case ComsPlayedMoveMessage::eSkip:
				// Just do nothing as the player has requested
				// That they skip their move
				break;
			case ComsPlayedMoveMessage::eFinishedBuy:
				// Just used as a notification that the player
				// has finished buying, do nothing
				break;
			case ComsPlayedMoveMessage::eResign:
				if (context.optionsGame->getResignMode() == OptionsGame::ResignStart)
				{
					processResignMessage(context, message, tank);
				}
				else if (context.optionsGame->getResignMode() == OptionsGame::ResignDueToHealth)
				{
					if (tank->getLife().getMaxLife() / 2.0f <= tank->getLife().getLife())
					{
						processResignMessage(context, message, tank);
					}
				}
				break;
			default:
				// add other START round types
				break;
		}
	}
	else
	{
		// All actions that are done at the very END of a round
		switch (message.getType())
		{
			case ComsPlayedMoveMessage::eResign:
				if (context.optionsGame->getResignMode() == OptionsGame::ResignEnd ||
					context.optionsGame->getResignMode() == OptionsGame::ResignDueToHealth)
				{
					processResignMessage(context, message, tank);
				}
				break;
			default:
				// Add other END round types
				break;
		}
	}
}

void PlayShots::processResignMessage(ScorchedContext &context, 
	ComsPlayedMoveMessage &message, Tank *tank)
{
	// Check the tank is alive
	if (tank->getState().getState() == TankState::sNormal)
	{
		// Tank resign action
		TankResign *resign = new TankResign(tank->getPlayerId());
		context.actionController->addAction(resign);

		StatsLogger::instance()->tankResigned(tank);
	}
}

void PlayShots::processFiredMessage(ScorchedContext &context, 
	ComsPlayedMoveMessage &message, Tank *tank)
{
	// Check the tank is alive
	if (tank->getState().getState() != TankState::sNormal)
	{
		return;
	}

	// Check the weapon name exists and is a weapon
	Accessory *accessory = 
		context.accessoryStore->findByAccessoryId(message.getWeaponId());
	if (!accessory) return;

	if (accessory->getPositionSelect() != Accessory::ePositionSelectFuel)
	{
		// Actually use up one of the weapons
		// Fuel, is used up differently at the rate of one weapon per movement square
		// This is done sperately in the tank movement action
		tank->getAccessories().rm(accessory, 1);
	}

	// shot fired action
	Weapon *weapon = (Weapon *) accessory->getAction();
	TankFired *fired = new TankFired(tank->getPlayerId(), 
		weapon,
		message.getRotationXY(), message.getRotationYZ());
	context.actionController->addAction(fired);

	// Set the tank to have the correct rotation etc..
	tank->getPosition().rotateGunXY(
		message.getRotationXY(), false);
	tank->getPosition().rotateGunYZ(
		message.getRotationYZ(), false);
	tank->getPosition().changePower(
		message.getPower(), false);
	tank->getPosition().setSelectPosition(
		message.getSelectPositionX(), 
		message.getSelectPositionY());

	// Create the action for the weapon and
	// add it to the action controller
	Vector velocity = tank->getPosition().getVelocityVector() *
		(tank->getPosition().getPower() + 1.0f);
	Vector position = tank->getPosition().getTankGunPosition();

	weapon->fireWeapon(context, tank->getPlayerId(), position, velocity, 0);
	StatsLogger::instance()->tankFired(tank, weapon);
	StatsLogger::instance()->weaponFired(weapon, false);
}