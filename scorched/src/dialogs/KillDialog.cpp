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
#include <scorched/ServerDialog.h>

KillDialog *KillDialog::instance_ = 0;

KillDialog *KillDialog::instance()
{
	if (!instance_)
	{
		instance_ = new KillDialog;
	}
	return instance_;
}

KillDialog::KillDialog() : GLWWindow("Kill Tanks", 210.0f, 75.0f, 0)
{
	killId_ = addWidget(new GLWTextButton("Mass tank kill", 10, 45, 190, this, false))->getId();
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
	else if (id == killId_)
	{
		killAll();
		WindowManager::instance()->hideWindow(id_);
	}
}
