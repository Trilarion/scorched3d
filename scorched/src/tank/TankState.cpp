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

#include <tank/Tank.h>
#include <tank/TankAdmin.h>
#include <common/OptionsDisplay.h>
#include <common/OptionsGame.h>
#include <engine/ScorchedContext.h>
#include <stdio.h>

TankState::TankState(ScorchedContext &context, unsigned int playerId) : 
	state_(sPending), life_(100.0f), tank_(0),
	readyState_(sReady), admin_(0),
	context_(context), spectator_(false), loading_(false),
	muted_(false), adminTries_(0),
	skipshots_(false)
{

}

TankState::~TankState()
{
}

void TankState::reset()
{
	setState(sDead);
	readyState_ = sReady;
	life_ = 100.0f;
}

void TankState::newGame()
{
	setState(sDead);
	life_ = 100.0f;
}

void TankState::setState(State s)
{
	if (loading_) return;

	state_ = s;
	if (state_ == sNormal) tank_->getPhysics().enablePhysics();
	else tank_->getPhysics().disablePhysics();
}

void TankState::setAdmin(TankAdmin *admin)
{ 
	delete admin_; 
	admin_ = admin; 
}

void TankState::setLife(float life)
{
	life_ = life;

	if (life_ >= 100) life_ = 100;
	if (life_ <= 0) life_ = 0;
	if (context_.optionsGame->getLimitPowerByHealth())
	{
		if (tank_->getPhysics().getPower() > life_ * 10.0f) 
			tank_->getPhysics().changePower(life_ * 10.0f, false);
	}
}

const char *TankState::getStateString()
{
	static char string[1024];
	sprintf(string, "%s - %s %s(%i hp)",
		((readyState_==sReady)?"Rdy":"Wait"),
		getSmallStateString(),
		(muted_?"muted ":""),
		(int) life_);
	return string;
}

const char *TankState::getSmallStateString()
{
	if (loading_) return "Loading";
	if (spectator_) return "Spectator";
	return ((state_==sDead)?"Dead":((state_==sNormal)?"Alive":"Pending"));
}

bool TankState::writeMessage(NetBuffer &buffer)
{
	buffer.addToBuffer((int) state_);
	buffer.addToBuffer(life_);
	buffer.addToBuffer(spectator_);
	buffer.addToBuffer(loading_);
	return true;
}

bool TankState::readMessage(NetBufferReader &reader)
{
	int s;
	if (!reader.getFromBuffer(s)) return false;
	setState((TankState::State) s);
	if (!reader.getFromBuffer(life_)) return false;
	if (!reader.getFromBuffer(spectator_)) return false;
	if (!reader.getFromBuffer(loading_)) return false;
	return true;
}

