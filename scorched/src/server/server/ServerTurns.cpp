////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2011
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
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <server/ServerTurns.h>
#include <server/ScorchedServer.h>
#include <server/ServerSimulator.h>
#include <server/ServerChannelManager.h>
#include <server/ServerDestinations.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tank/TankTeamScore.h>
#include <common/OptionsScorched.h>
#include <common/OptionsTransient.h>
#include <simactions/TankStartMoveSimAction.h>
#include <simactions/TankStopMoveSimAction.h>
#include <simactions/PlayMovesSimAction.h>
#include <simactions/NextTurnSimAction.h>
#include <coms/ComsPlayedMoveMessage.h>

ServerTurns::ServerTurns(bool waitForShots) : 
	shotsState_(eShotsNone),
	waitForShots_(waitForShots)
{
	shotsStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::shotsStarted);
	moveStarted_ = new SimulatorIAdapter<ServerTurns>(this,  &ServerTurns::moveStarted);
}

ServerTurns::~ServerTurns()
{
	delete shotsStarted_;
	delete moveStarted_;
	{
		std::map<unsigned int, PlayingPlayer*>::iterator itor;
		for (itor = playingPlayers_.begin();
			itor != playingPlayers_.end();
			itor++)
		{
			delete itor->second;
		}
		playingPlayers_.clear();
	}
	{
		std::map<unsigned int, WaitingPlayer*>::iterator itor;
		for (itor = waitingPlayers_.begin();
			itor != waitingPlayers_.end();
			itor++)
		{
			delete itor->second;
		}
		waitingPlayers_.clear();
	}
}

void ServerTurns::enterState()
{
	shotsState_ = eShotsNone;
	std::map<unsigned int, PlayingPlayer*>::iterator playingItor;
	for (playingItor = playingPlayers_.begin();
		playingItor != playingPlayers_.end();
		playingItor++)
	{
		delete playingItor->second;
	}
	playingPlayers_.clear();
	std::map<unsigned int, WaitingPlayer*>::iterator waitingItor;
	for (waitingItor = waitingPlayers_.begin();
		waitingItor != waitingPlayers_.end();
		waitingItor++)
	{
		delete waitingItor->second;
	}
	waitingPlayers_.clear();

	internalEnterState();
}

void ServerTurns::internalShotsFinished()
{
}

void ServerTurns::simulate(fixed frameTime)
{
	// Check what we are allowed to do
	if (shotsState_ == eShotsWaitingStart) return;
	if (shotsState_ == eShotsWaitingEnd)
	{
		if (ScorchedServer::instance()->getActionController().noReferencedActions())
		{
			internalShotsFinished();
			shotsState_ = eShotsNone;
		}
		else
		{
			return;
		}
	}

	std::list<unsigned int> processPlayers;
	std::list<unsigned int>::iterator processPlayersItor;

	// Remove time from playing players
	processPlayers.clear();
	std::map<unsigned int, PlayingPlayer*>::iterator playingItor;
	for (playingItor = playingPlayers_.begin();
		playingItor != playingPlayers_.end();
		playingItor++)
	{
		unsigned int playerId = playingItor->first;
		PlayingPlayer *playing = playingItor->second;
		if (playing->startedMove_)
		{
			playing->moveTime_ -= frameTime;
			if (playing->moveTime_ < 0) processPlayers.push_back(playerId);
		}
	}
	for (processPlayersItor = processPlayers.begin();
		processPlayersItor != processPlayers.end();
		processPlayersItor++)
	{
		unsigned int playerId = *processPlayersItor;
		PlayingPlayer *playing = playingPlayers_[playerId];

		// Player has timed out
		ComsPlayedMoveMessage timedOutMessage(
			playerId, playing->moveId_, ComsPlayedMoveMessage::eTimeout);
		moveFinished(timedOutMessage);
	}

	// Remove time from waiting players
	processPlayers.clear();
	std::map<unsigned int, WaitingPlayer*>::iterator waitingItor;
	for (waitingItor = waitingPlayers_.begin();
		waitingItor != waitingPlayers_.end();
		waitingItor++)
	{
		unsigned int playerId = waitingItor->first;
		WaitingPlayer *waiting = waitingItor->second;

		waiting->waitTime_ -= frameTime;
		if (waiting->waitTime_ < 0) processPlayers.push_back(playerId);
	}
	for (processPlayersItor = processPlayers.begin();
		processPlayersItor != processPlayers.end();
		processPlayersItor++)
	{
		unsigned int playerId = *processPlayersItor;
		WaitingPlayer *waiting = waitingPlayers_[playerId];

		ScorchedServer::instance()->getServerSimulator().
			addSimulatorAction(waiting->action_, waiting->callback_);
		delete waiting;
		waitingPlayers_.erase(playerId);
	}

	// Do other stuff
	internalSimulate(frameTime);
}

void ServerTurns::shotsStarted(fixed simulationTime, SimAction *action)
{
	shotsState_ = eShotsWaitingEnd;
}

void ServerTurns::moveStarted(fixed simulationTime, SimAction *action)
{
	TankStartMoveSimAction *startMove = (TankStartMoveSimAction *) action;
	
	std::map<unsigned int, PlayingPlayer*>::iterator findItor =
		playingPlayers_.find(startMove->getPlayerId());
	if (findItor != playingPlayers_.end())
	{
		findItor->second->startedMove_ = true;
	}
}

void ServerTurns::moveFinished(ComsPlayedMoveMessage &playedMessage)
{
	internalMoveFinished(playedMessage);
}

bool ServerTurns::finished()
{
	return internalFinished();
}

bool ServerTurns::internalFinished()
{
	return showScore();
}

bool ServerTurns::showScore()
{
	// Check why this round has finished
	int teamWonGame = 
		ScorchedServer::instance()->getContext().getTankTeamScore().getWonGame();
	if (teamWonGame > 0)
	{
		// A team has won
		ChannelText text("info",
			"ROUND_FINISHED_TEAM_WON",
			"Round finished due to team obtaining an objective.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}

	if (ScorchedServer::instance()->getOptionsGame().getTeams() > 1 &&
		ScorchedServer::instance()->getTankContainer().teamCount() == 1)
	{
		// Only one team left
		ChannelText text("info",
			"ROUND_FINISHED_TEAM",
			"Round finished due to last team standing.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}
	else if (ScorchedServer::instance()->getTankContainer().aliveCount() == 0)
	{
		// Only one person left
		ChannelText text("info",
			"ROUND_FINISHED_DEAD",
			"Round finished due to annihilation.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}
	else if (ScorchedServer::instance()->getTankContainer().aliveCount() <= 1)
	{
		// Only one person left
		ChannelText text("info",
			"ROUND_FINISHED_PLAYER",
			"Round finished due to last man standing.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}

	if (ScorchedServer::instance()->getOptionsGame().getNoTurns() > 0)
	{
		if (ScorchedServer::instance()->getOptionsTransient().getCurrentTurnNo() >
			ScorchedServer::instance()->getOptionsGame().getNoTurns())
		{
			// Max turns exceeded
			ChannelText text("info",
				"ROUND_FINISHED_TURNS",
				"Round finished due to maximum moves reached.");
			ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
			return true;
		}
	}

	// Check for tanks skiping turns
	bool allSkipped = true;
	std::map<unsigned int, Tank *> &tanks =
		ScorchedServer::instance()->getTankContainer().getAllTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getScore().getWonGame())
		{
			ChannelText text("info",
				"ROUND_FINISHED_TANK_WON",
				"Round finished due to player obtaining an objective.");
			ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
			return true;
		}

		if (tank->getState().getLives() > 0 &&
			tank->getState().getTankPlaying() &&
			tank->getState().getSkippedShots() < 3)
		{
			allSkipped = false;
		}
	}
	if (allSkipped) 
	{
		ChannelText text("info",
			"ROUND_FINISHED_SKIP",
			"Round finished due to all players skipping moves.");
		ScorchedServer::instance()->getServerChannelManager().sendText(text, true);
		return true;
	}

	return false;
}

void ServerTurns::playMove(Tank *tank, unsigned int moveId, fixed maximumShotTime, fixed delayStartMoveTime)
{
	// If there is a maximum shot time make sure the AIs dont think longer
	// than the allowed time
	if (maximumShotTime > 0)
	{
		if (delayStartMoveTime > maximumShotTime - 5) delayStartMoveTime = maximumShotTime - 5;
		if (delayStartMoveTime < 0) delayStartMoveTime = 0;
	}

	// Get the ping
	fixed ping = 0;
	ServerDestination *destination = 
		ScorchedServer::instance()->getServerDestinations().getDestination(tank->getDestinationId());
	if (destination) ping = destination->getPing().getAverage();

	// Create the move action
	tank->getState().setMoveId(moveId);
	TankStartMoveSimAction *tankSimAction = new TankStartMoveSimAction(
		tank->getPlayerId(), tank->getState().getMoveId(), maximumShotTime, false, ping);

	// If shotTime > 0 then add to the list of playing players so they can be timed out
	SimulatorI *callback = 0;
	if (maximumShotTime > 0)
	{
		fixed simulationTime(ScorchedServer::instance()->getServerSimulator().getSendStepSize());
		PlayingPlayer *playingPlayer = new PlayingPlayer(moveId, maximumShotTime + simulationTime * 2);
		playingPlayers_[tank->getPlayerId()] = playingPlayer;
		callback = moveStarted_;
	}

	// Delay players that need delayed
	if (delayStartMoveTime > 0)
	{
		WaitingPlayer *waitingPlayer = new WaitingPlayer(delayStartMoveTime, tankSimAction, callback);
		waitingPlayers_[tank->getPlayerId()] = waitingPlayer;
	}
	else
	{
		ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction, callback);
	}
}

void ServerTurns::playMoveFinished(Tank *tank)
{	
	// Remove timeout check for this player
	std::map<unsigned int, PlayingPlayer*>::iterator findItor =
		playingPlayers_.find(tank->getPlayerId());
	if (findItor != playingPlayers_.end())
	{
		delete findItor->second;
		playingPlayers_.erase(findItor);
	}

	// Send stop move action
	TankStopMoveSimAction *tankSimAction = 
		new TankStopMoveSimAction(tank->getPlayerId());
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(tankSimAction);	
	tank->getState().setMoveId(0);
}

void ServerTurns::playShots(std::list<ComsPlayedMoveMessage *> messages, unsigned int moveId, bool timeOutPlayers)
{
	PlayMovesSimAction *movesAction = 
		new PlayMovesSimAction(moveId, timeOutPlayers);
	std::list<ComsPlayedMoveMessage *>::iterator itor;
	for (itor = messages.begin();
		itor != messages.end();
		itor++)
	{
		movesAction->addMove(*itor);
	}
	
	SimulatorI *callback = 0;
	if (waitForShots_)
	{
		callback = shotsStarted_;
		shotsState_ = eShotsWaitingStart;
	}
	ScorchedServer::instance()->getServerSimulator().
		addSimulatorAction(movesAction, callback);
}

void ServerTurns::incrementTurn()
{
	NextTurnSimAction *turnAction = new NextTurnSimAction();
	ScorchedServer::instance()->getServerSimulator().addSimulatorAction(turnAction);
}
