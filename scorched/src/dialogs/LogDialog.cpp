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

#include <dialogs/LogDialog.h>
#include <dialogs/QuitDialog.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>
#include <GLEXT/GLViewPort.h>
#include <common/OptionsDisplay.h>

LogDialog *LogDialog::instance_ = 0;

LogDialog *LogDialog::instance()
{
	if (!instance_)
	{
		instance_ = new LogDialog;
	}
	return instance_;
}

LogDialog::LogDialog() : 
	GLWWindow("Log", 10.0f, 10.0f, 300.0f, 240.0f, 0,
		"Log Window")
{
	needCentered_ = true;
	quit_ = (GLWTextButton *) 
		addWidget(new GLWTextButton("Quit", 205, 10, 85, this, 
		GLWButton::ButtonFlagCenterX));
	listView_ = (GLWListView *) addWidget(new GLWListView(10, 40, 280, 140, 100));
	serverName_ = (GLWLabel *) addWidget(new GLWLabel(5, 195, "Local"));

	// Add this class as a log handler
	Logger::addLogger(this);
}

LogDialog::~LogDialog()
{
}

void LogDialog::logMessage(
		const char *time,
		const char *message,
		unsigned int playerId)
{
	listView_->addLine(message);
}

void LogDialog::draw()
{
	float wWidth = (float) GLViewPort::getWidth();
	float width = wWidth - 40.0f;
	if (width < 320) width = 320;
	if (width > 640) width = 640;
	setW(width - 20);
	listView_->setW(width - 40);
	quit_->setX(width - 115);

	float wHeight = (float) GLViewPort::getHeight();
	float height = wHeight - 40.0f;
	if (height < 200) height = 200;
	if (height > 300) height = 300;
	serverName_->setY(height - 50);
	listView_->setH(height - 90);
	setH(height - 20);

	GLWWindow::draw();
}

void LogDialog::buttonDown(unsigned int id)
{
	if (id == quit_->getId())
	{
		GLWWindowManager::instance()->
			showWindow(QuitDialog::instance()->getId());
	}
}
