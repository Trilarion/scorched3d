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


// TankContainer.cpp: implementation of the TankContainer class.
//
//////////////////////////////////////////////////////////////////////

#include <tank/TankContainer.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TankContainer *TankContainer::instance_ = 0;

TankContainer *TankContainer::instance()
{
	if (!instance_)
	{
		instance_ = new TankContainer;
	}
	return instance_;
}

TankContainer::TankContainer() : playerId_(0)
{

}

TankContainer::~TankContainer()
{

}

void TankContainer::addTank(Tank *tank)
{
	playingTanks_[tank->getPlayerId()] = tank;
}

Tank *TankContainer::removeTank(unsigned int playerId)
{
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = playingTanks_.begin();
		itor != playingTanks_.end();
		itor++)
	{
		Tank *current = (*itor).second;
		if (current->getPlayerId() == playerId)
		{
			playingTanks_.erase(itor);
			return current;
		}
	}

	return 0;
}

Tank *TankContainer::getTankByPos(unsigned int pos)
{
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		mainitor != playingTanks_.end();
		mainitor++, pos--)
	{
		if (pos <=0) return (*mainitor).second;
	}
	return 0;
}

Tank *TankContainer::getTankById(unsigned int id)
{
	std::map<unsigned int, Tank *>::iterator mainitor =
		playingTanks_.find(id);
	if (mainitor != playingTanks_.end())
	{
		return (*mainitor).second;
	}
	return 0;
}

Tank *TankContainer::getTankByName(const char *name)
{
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		if (0 == strcmp((*mainitor).second->getName(),name)) return (*mainitor).second;
	}
	return 0;
}

Tank *TankContainer::getCurrentTank()
{
	if (!playingTanks_.empty())
	{
		if (playerId_)
		{
			static Tank *currentPlayer = 0;
			if (!currentPlayer || currentPlayer->getPlayerId() != playerId_)
			{
				currentPlayer = getTankById(playerId_);
			}
			return currentPlayer;
		}
	}

	return 0;
}

void TankContainer::nextRound()
{
	// Tell each tank a new round is starting
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		current->nextRound();
	}
}

void TankContainer::newGame()
{
	// Tell each tank a new game has started
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		current->newGame();
	}
}

void TankContainer::resetTanks()
{
	// Reset all tanks
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		current->reset();
	}
}

void TankContainer::removeAllTanks()
{
	// Remove all tanks
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		delete current;
	}
	playingTanks_.clear();
}

int TankContainer::aliveCount()
{
	int alive = 0;

	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;
		if (current->getState().getState() == TankState::sNormal)
		{
			alive++;
		}
	}
	return alive;
}

bool TankContainer::allReady()
{
	std::map<unsigned int, Tank *>::iterator mainitor;
	for (mainitor = playingTanks_.begin();
		 mainitor != playingTanks_.end();
		 mainitor++)
	{
		Tank *current = (*mainitor).second;

		// current check tanks that are not pending
		if (current->getState().getState() != TankState::sPending)
		{
			if (current->getState().getReadyState() == 
				TankState::SNotReady) return false;
		}
	}
	return true;
}
