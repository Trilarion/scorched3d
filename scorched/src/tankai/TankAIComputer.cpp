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

#include <tankai/TankAIComputer.h>
#include <tankai/TankAIStrings.h>
#include <tankai/TankAILogic.h>
#include <server/ServerShotHolder.h>
#include <server/ScorchedServer.h>
#include <common/OptionsGame.h>
#include <common/OptionsTransient.h>
#include <actions/TankSay.h>
#include <engine/ScorchedContext.h>
#include <coms/ComsMessageSender.h>

TankAIComputer::TankAIComputer() : 
	TankAI(0, 0), primaryShot_(true), name_("<NoName>")
{
	
}

TankAIComputer::~TankAIComputer()
{

}

void TankAIComputer::setContext(ScorchedContext *context)
{
	context_ = context;
}

void TankAIComputer::setTank(Tank *tank)
{
	currentTank_ = tank;
	tankBuyer_.setTank(currentTank_);
}

bool TankAIComputer::parseConfig(XMLNode *node)
{
	XMLNode *nameNode = node->getNamedChild("name");
	if (!nameNode)
	{
		dialogMessage("TankAIComputer",
			"Failed to find name node in tank ai");
		return false;
	}
	name_ = nameNode->getContent();

	std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = node->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 childrenItor++)
    {
		XMLNode *child = (*childrenItor);
		if (strcmp(child->getName(), "weapon")==0)
		{
			XMLNode *wname = child->getNamedChild("name");
			XMLNode *wlevel = child->getNamedChild("level");
			if (!wname || !wlevel) 
			{
				dialogMessage("TankAIComputer",
					"Failed to parse weapon node in tank ai \"%s\"",
					name_.c_str());
				return false;
			}

			if (!tankBuyer_.addAccessory(
				wname->getContent(), 
				atoi(wlevel->getContent()))) return false;
		}
	}

	return true;
}

void TankAIComputer::newGame()
{

}

void TankAIComputer::nextShot()
{

}

void TankAIComputer::tankHurt(Weapon *weapon, unsigned int firer)
{
	if (currentTank_->getState().getLife() == 0.0f &&
		currentTank_->getState().getState() == TankState::sNormal)
	{
		const char *line = TankAIStrings::instance()->getDeathLine();
		if (line) say(line);
	}
}

void TankAIComputer::shotLanded(Weapon *weapon, unsigned int firer, Vector &position)
{
	if (primaryShot_ && firer == currentTank_->getPlayerId())
	{
		ourShotLanded(weapon, position);
		primaryShot_ = false;
	}
}

void TankAIComputer::ourShotLanded(Weapon *weapon, Vector &position)
{

}

void TankAIComputer::autoDefense()
{

}

void TankAIComputer::buyAccessories()
{
	ComsPlayedMoveMessage *message = 
		new ComsPlayedMoveMessage(currentTank_->getPlayerId(), ComsPlayedMoveMessage::eFinishedBuy);
	ServerShotHolder::instance()->addShot(currentTank_->getPlayerId(), message);

	tankBuyer_.buyAccessories(3);
}

void TankAIComputer::selectFirstShield()
{
	if (currentTank_->getAccessories().getShields().getAllShields().size())
	{
		std::map<Shield *, int>::iterator itor = 
			currentTank_->getAccessories().getShields().getAllShields().begin();
		Shield *shield = (*itor).first;

		if (!currentTank_->getAccessories().getShields().getCurrentShield())
		{
			shieldsUpDown(shield->getName());
		}
	}
}

void TankAIComputer::say(const char *text)
{
	std::string newText(currentTank_->getName());
	newText += ":";
	newText += text;

	context_->actionController.addAction(
		new TankSay(currentTank_->getPlayerId(), newText.c_str()));
}

void TankAIComputer::fireShot()
{
	primaryShot_ = true;
	Weapon *currentWeapon = 
		currentTank_->getAccessories().getWeapons().getCurrent();
	if (currentWeapon)
	{
		ComsPlayedMoveMessage *message = 
			new ComsPlayedMoveMessage(currentTank_->getPlayerId(), ComsPlayedMoveMessage::eShot);
		message->setShot(
			currentTank_->getAccessories().getWeapons().getCurrent()->getName(),
			currentTank_->getPhysics().getRotationGunXY(),
			currentTank_->getPhysics().getRotationGunYZ(),
			currentTank_->getState().getPower());
	
		ServerShotHolder::instance()->addShot(currentTank_->getPlayerId(), message);
	}
}

void TankAIComputer::skipShot()
{
	ComsPlayedMoveMessage *message = 
		new ComsPlayedMoveMessage(currentTank_->getPlayerId(), ComsPlayedMoveMessage::eSkip);
	ServerShotHolder::instance()->addShot(currentTank_->getPlayerId(), message);
}

void TankAIComputer::resign()
{
	ComsPlayedMoveMessage *message = 
		new ComsPlayedMoveMessage(currentTank_->getPlayerId(), ComsPlayedMoveMessage::eResign);
	ServerShotHolder::instance()->addShot(currentTank_->getPlayerId(), message);
}

void TankAIComputer::move(int x, int y)
{
	ComsPlayedMoveMessage *message = 
		new ComsPlayedMoveMessage(currentTank_->getPlayerId(), ComsPlayedMoveMessage::eMove);
	message->setPosition(x, y);
	ServerShotHolder::instance()->addShot(currentTank_->getPlayerId(), message);
}

void TankAIComputer::parachutesUpDown(bool on)
{
	ComsDefenseMessage defenseMessage(
		currentTank_->getPlayerId(),
		on?ComsDefenseMessage::eParachutesUp:ComsDefenseMessage::eParachutesDown);

	if (TankAILogic::processDefenseMessage(
		ScorchedServer::instance()->getContext(), defenseMessage, currentTank_))
	{
		ComsMessageSender::sendToAllPlayingClients(defenseMessage);
	}
}

void TankAIComputer::shieldsUpDown(const char *name)
{
	ComsDefenseMessage defenseMessage(
		currentTank_->getPlayerId(),
		name?ComsDefenseMessage::eShieldUp:ComsDefenseMessage::eShieldDown,
		name?name:"");

	if (TankAILogic::processDefenseMessage(
		ScorchedServer::instance()->getContext(), defenseMessage, currentTank_))
	{
		ComsMessageSender::sendToAllPlayingClients(defenseMessage);
	}
}

void TankAIComputer::useBattery()
{
	ComsDefenseMessage defenseMessage(
		currentTank_->getPlayerId(),
		ComsDefenseMessage::eBatteryUse);

	if (TankAILogic::processDefenseMessage(
		ScorchedServer::instance()->getContext(), defenseMessage, currentTank_))
	{
		ComsMessageSender::sendToAllPlayingClients(defenseMessage);
	}
}
