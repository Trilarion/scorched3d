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


#include <dialogs/KillDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <common/WindowManager.h>
#include <engine/MainLoop.h>
#include <tank/TankContainer.h>

KillDialog *KillDialog::instance_ = 0;

KillDialog *KillDialog::instance()
{
	if (!instance_)
	{
		instance_ = new KillDialog;
	}
	return instance_;
}

KillDialog::KillDialog() : GLWWindow("Kill Tanks", 210.0f, 110.0f, 0)
{
	killId_ = addWidget(new GLWTextButton("Mass tank kill", 10, 45, 190, this, false))->getId();
	quitId_ = addWidget(new GLWTextButton("Quit Game", 10, 80, 190, this, false))->getId();
	okId_ = addWidget(new GLWTextButton(" Cancel ", 95, 10, 105, this, true))->getId();
}

KillDialog::~KillDialog()
{
}

void KillDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		WindowManager::instance()->hideWindow(id_);
	}
	else if (id == quitId_)
	{
		MainLoop::instance()->exitLoop();
	}
	else if (id == killId_)
	{
		std::map<unsigned int, Tank *>::iterator itor;
		std::map<unsigned int, Tank *> &tanks = 
			TankContainer::instance()->getPlayingTanks();
		for (itor = tanks.begin();
			itor != tanks.end();
			itor++)
		{
			Tank *current = (*itor).second;
			current->getState().setState(TankState::sDead);
		}
		WindowManager::instance()->hideWindow(id_);
	}
}
