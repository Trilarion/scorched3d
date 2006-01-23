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

#include <dialogs/ScoreDialog.h>
#include <tank/TankSort.h>
#include <tank/TankContainer.h>
#include <GLW/GLWFont.h>
#include <common/OptionsParam.h>
#include <common/OptionsTransient.h>
#include <common/OptionsGame.h>
#include <common/Defines.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <server/ScorchedServer.h>
#include <stdio.h>

static const float rankLeft = 15.0f;
static const float iconLeft = 5.0f;
static const float nameLeft = 35.0f;
static const float killsLeft = 215.0f;
static const float moneyLeft = 270.0f;
static const float winsLeft = 350.0f;
static const float readyLeft = 380.0f;
static const float statsLeft = 405.0f;
static const float lineSpacer = 10.0f;

ScoreDialog *ScoreDialog::instance_ = 0;
ScoreDialog *ScoreDialog::instance2_ = 0;

ScoreDialog *ScoreDialog::instance()
{
	if (!instance_)
	{
		instance_ = new ScoreDialog;
	}

	return instance_;
}

ScoreDialog *ScoreDialog::instance2()
{
	if (!instance2_)
	{
		instance2_ = new ScoreDialog;
	}

	return instance2_;
}

ScoreDialog::ScoreDialog() :
	GLWWindow("Score", 10.0f, 10.0f, 447.0f, 310.0f, eTransparent |eSmallTitle,
		"Shows the current score for all players."),
	lastScoreValue_(0), lastWinsValue_(0), lastNoPlayers_(0)
{

}

ScoreDialog::~ScoreDialog()
{

}

void ScoreDialog::display()
{
	GLWWindow::display();
	calculateScores();
}

void ScoreDialog::calculateScores()
{
	lastScoreValue_ = lastWinsValue_ = 0;
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = tanks.begin();
		itor != tanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		lastScoreValue_ += tank->getScore().getMoney();
		lastWinsValue_ += tank->getScore().getWins();
	}

	lastNoPlayers_ = tanks.size();
	sortedTanks_.clear();
	TankSort::getSortedTanksIds(
		ScorchedClient::instance()->getContext(), sortedTanks_);
}

void ScoreDialog::windowInit(const unsigned state)
{
	needCentered_ = true;
}

void ScoreDialog::draw()
{
	float h = sortedTanks_.size() * lineSpacer + 80.0f;
	if (ScorchedClient::instance()->getOptionsGame().getTeams() > 1)
	{
		h += (lineSpacer * 2.0f) * (ScorchedClient::instance()->getOptionsGame().getTeams() - 1) +
			lineSpacer * 2.0f;
	}
	setH(h);

	static size_t noTanks = 0;
	if (noTanks != sortedTanks_.size())
	{
		needCentered_ = true;
		noTanks = sortedTanks_.size();
	}

	GLWWindow::draw();
	GLState newState(GLState::TEXTURE_OFF | GLState::DEPTH_OFF);

	bool finished = (ScorchedClient::instance()->getGameState().getState() == 
		ClientState::StateScore);

	Vector white(0.9f, 0.9f, 1.0f);
	bool server = (OptionsParam::instance()->getConnectedToServer());
	{
		const char *text = "Current Rankings";
		if (ScorchedClient::instance()->getGameState().getState() == 
			ClientState::StateScore)
		{
			text = "Final Rankings";
		}
		else if (ScorchedClient::instance()->getGameState().getState() == 
			ClientState::StateGetPlayers)
		{
			finished = true;
			if (ScorchedClient::instance()->getTankContainer().getNoOfNonSpectatorTanks() <
				ScorchedClient::instance()->getOptionsGame().getNoMinPlayers())
			{
				text = "Waiting for more players";
			}
			else
			{
				text = "Waiting to join game";
			}
		}
		else if (server)
		{
			text = ScorchedClient::instance()->getOptionsGame().getServerName();
		}

		GLWFont::instance()->getLargePtFont()->draw(
				white,
				20,
				x_ + 8.0f, y_ + h_ - 21.0f, 0.0f,
				text);
	}

	if (!finished) 
	{
		char moves[256];
		moves[0] = '\0';
		if (ScorchedClient::instance()->getOptionsGame().getNoMaxRoundTurns() > 0)
		{
			snprintf(moves, 256, ",Move %i of %i", 
				ScorchedClient::instance()->getOptionsTransient().getCurrentGameNo(),
				ScorchedClient::instance()->getOptionsGame().getNoMaxRoundTurns());
		}
		const char *rounds = formatString("Round %i of %i%s",
				ScorchedClient::instance()->getOptionsTransient().getCurrentRoundNo(),
				ScorchedClient::instance()->getOptionsGame().getNoRounds(),
				moves);

		int roundsWidth = GLWFont::instance()->getSmallPtFont()->getWidth(
			10, rounds);
		GLWFont::instance()->getSmallPtFont()->draw(
			white,
			10,
			x_ + 430 - roundsWidth, y_ + h_ - 40.0f, 0.0f,
			rounds);
	}

	float y = lineSpacer + 10.0f;
	GLWFont::instance()->getSmallPtFont()->draw(
			white,
			12,
			x_ + nameLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Name");
	GLWFont::instance()->getSmallPtFont()->draw(
			white,
			12,
			x_ + killsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Kills");
	GLWFont::instance()->getSmallPtFont()->draw(
			white,
			12,
			x_ + moneyLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Money");
	GLWFont::instance()->getSmallPtFont()->draw(
			white,
			12,
			x_ + winsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Wins");
	GLWFont::instance()->getSmallPtFont()->draw(
			white,
			12,
			x_ + statsLeft, y_ + h_ - y - lineSpacer - 26.0f, 0.0f,
			"Stat");
	y+= lineSpacer + lineSpacer;

	int tmpLastScoreValue = 0;
	int tmpLastWinsValue = 0;
	if (ScorchedClient::instance()->getOptionsGame().getTeams() > 1)
	{
		int winningTeam = TankSort::getWinningTeam(
			ScorchedClient::instance()->getContext());

		struct Score
		{
			Score() : wins(0), kills(0), money(0), team(false) {}
	
			int wins;
			int kills;
			int money;
			bool team;
		};
		Score scores[4];

		for (int i=0; i<ScorchedClient::instance()->getOptionsGame().getTeams(); i++)
		{
			std::list<unsigned int>::iterator itor;
			for (itor = sortedTanks_.begin();
				itor != sortedTanks_.end();
				itor ++)
			{
				unsigned int playerId = (*itor);
				Tank *current = ScorchedClient::instance()->getTankContainer().getTankById(playerId);
				if (current && current->getTeam() == (i + 1) && !current->getState().getSpectator()) 
				{
					scores[i].team = true;
					addLine(current, y, (char *)((winningTeam==(i+1))?"1":"2"), finished);
					
					scores[i].wins += current->getScore().getWins();
					scores[i].kills += current->getScore().getKills();
					scores[i].money += current->getScore().getMoney();
					tmpLastScoreValue += current->getScore().getMoney();
					tmpLastWinsValue += current->getScore().getWins();
					y+= lineSpacer;
				}
			}
			if (scores[i].team)
			{
				addScoreLine(y, scores[i].kills, scores[i].money, scores[i].wins);
				y+= lineSpacer;
				y+= lineSpacer;
			}
		}
	}
	else
	{
		// No Team
		int rank = 1;
		char strrank[10];
		std::list<unsigned int>::iterator itor;
		for (itor = sortedTanks_.begin();
			itor != sortedTanks_.end();
			itor ++, rank++)
		{
			unsigned int playerId = (*itor);
			Tank *current = ScorchedClient::instance()->getTankContainer().getTankById(playerId);
			if (current && !current->getState().getSpectator()) 
			{
				snprintf(strrank, 10, "%i", rank);

				addLine(current, y, strrank, finished);
				tmpLastScoreValue += current->getScore().getMoney();
				tmpLastWinsValue += current->getScore().getWins();
				y+= lineSpacer;
			}
		}	
	}
	y+= lineSpacer / 1.5f;
	// Spectators
	std::list<unsigned int>::iterator itor;
	for (itor = sortedTanks_.begin();
		itor != sortedTanks_.end();
		itor ++)
	{
		unsigned int playerId = (*itor);
		Tank *current = ScorchedClient::instance()->
			getTankContainer().getTankById(playerId);
		if (current && current->getState().getSpectator()) 
		{
			addLine(current, y, " ", false);
			y+= lineSpacer;
		}
	}	

	std::map<unsigned int, Tank *> &realTanks = 
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	if (tmpLastScoreValue != lastScoreValue_ ||
		tmpLastWinsValue != lastWinsValue_ ||
		realTanks.size() != lastNoPlayers_)
	{
		calculateScores();
	}
}

void ScoreDialog::addScoreLine(float y, int kills, int money, int wins)
{
	float textX = x_;
	float textY  = y_ + h_ - y - lineSpacer - 30.0f;
	Vector white(0.9f, 0.9f, 1.0f);

	// Print the name on the screen
	GLWFont::instance()->getSmallPtFont()->draw(
		white,
		10,
		textX + killsLeft, textY, 0.0f,
		"%i",
		kills);	
	GLWFont::instance()->getSmallPtFont()->draw(
		white,
		10,
		textX + moneyLeft, textY, 0.0f,
		"$%i",
		money);	
	GLWFont::instance()->getSmallPtFont()->draw(
		white,
		10,
		textX + winsLeft, textY, 0.0f,
		"%i",
		wins);	
}

void ScoreDialog::addLine(Tank *current, float y, char *rank, bool finished)
{
	float textX = x_;
	float textY  = y_ + h_ - y - lineSpacer - 25.0f;
	bool currentPlayer = false;
	
	// Print a highlight behind the current clients player
	if (!current->getState().getSpectator() &&
		current->getDestinationId() == 
		ScorchedClient::instance()->getTankContainer().getCurrentDestinationId())
	{
		GLState state(GLState::BLEND_ON); 

		glColor4f(0.0f, 0.0f, 0.0f, 0.4f);
		glBegin(GL_QUADS);
			glVertex2f(x_ + w_ - 3.0f, textY + lineSpacer - 1.0f);
			glVertex2f(x_ + 3.0f, textY + lineSpacer - 1.0f);
			glVertex2f(x_ + 3.0f, textY - 1.0f);
			glVertex2f(x_ + w_ - 3.0f, textY - 1.0f);
		glEnd();
	}

	// Form the name
	static char name[256];
	strcpy(name, current->getName());
	if (finished && ! OptionsParam::instance()->getConnectedToServer())
	{
		strcat(name, " (");
		Tank *serverTank = 
			ScorchedServer::instance()->getTankContainer().getTankById(
			current->getPlayerId());
		TankAI *tankAI = serverTank->getTankAI();
		if (tankAI) strcat(name, tankAI->getName());
		else strcat(name, "Human");
		strcat(name, ")");
	}
	else if (current->getState().getState() != TankState::sNormal)
	{
		strcat(name, " (");
		strcat(name, current->getState().getSmallStateString());
		strcat(name, ")");
	}
	name[26] = '\0'; // Limit length

	if (current->getState().getSpectator())
	{
		// Print the name on the screen
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + nameLeft, textY, 0.0f,
			name);
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + readyLeft, textY, 0.0f,
			"%2s",
			((current->getState().getReadyState() == TankState::SNotReady)?"*":" "));
	}
	else
	{
		GLState state(GLState::TEXTURE_ON);
		glColor3f(1.0f, 1.0f, 1.0f);
		current->getAvatar().getTexture()->draw();
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(textX + iconLeft,
				textY + 2.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(textX + iconLeft + 8.0f, 
				textY + 2.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(textX + iconLeft + 8.0f,
				textY + 10.0f);
			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(textX + iconLeft,
				textY + 10.0f);
        glEnd();

		// Print the name on the screen
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + rankLeft, textY, 0.0f,
			"%s",
			rank);
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + nameLeft, textY, 0.0f,
			"%s",
			name);
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + killsLeft, textY, 0.0f,
			"%i",
			current->getScore().getKills());
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + moneyLeft, textY, 0.0f,
			"$%i",
			current->getScore().getMoney());
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + winsLeft, textY, 0.0f,
			"%i",
			current->getScore().getWins());
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + readyLeft, textY, 0.0f,
			"%s",
			((current->getState().getReadyState() == TankState::SNotReady)?"*":" "));
		GLWFont::instance()->getSmallPtFont()->draw(
			current->getColor(),
			10,
			textX + statsLeft, textY, 0.0f,
			"%s",
			current->getScore().getStatsRank());
	}
}
