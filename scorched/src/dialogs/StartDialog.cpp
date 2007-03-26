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

#include <dialogs/StartDialog.h>
#include <dialogs/ModSelectDialog.h>
#include <dialogs/ModSubSelectDialog.h>
#include <dialogs/SaveSelectDialog.h>
#include <dialogs/NetworkSelectDialog.h>
#include <dialogs/SettingsSelectDialog.h>
#include <dialogs/SettingsSubSelectDialog.h>
#include <client/ScorchedClient.h>
#include <client/ClientParams.h>
#include <client/ClientMain.h>
#include <engine/GameState.h>
#include <engine/MainLoop.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWWindowManager.h>

StartDialog *StartDialog::instance_ = 0;

StartDialog *StartDialog::instance()
{
	if (!instance_)
	{
		instance_ = new StartDialog;
	}
	return instance_;
}

StartDialog::StartDialog() : 
	GLWWindow("", 10.0f, 10.0f, 640.0f, 480.0f, eNoTitle),
	selected_(-1)
{
	OptionDefinition defs[] = 
	{
		"Tutorial", "- Start the tutorial to learn how to play.", 50.0f, 60.0f, 0.0f,
		"Local Game", "- Play a game against the computer or other local players.", 50.0f, 100.0f, 0.0f,
		"Network Game", "- Play an online game against remote players.", 50.0f, 140.0f, 0.0f,
		"Load Save", "- Continue playing a saved game.", 50.0f, 180.0f, 0.0f,
		"Help", "- View the online help.", 50.0f, 250.0f, 0.0f,
		"Donate", "- Show support for Scorched3D.", 50.0f, 290.0f, 0.0f,
		"Quit", "- Exit the game.", 50.0f, 360.0f, 0.0f
	};
	for (int i=0; i<sizeof(defs) / sizeof(OptionDefinition); i++)
	{
		definitions_.push_back(defs[i]);
	}
}

StartDialog::~StartDialog()
{
}

void StartDialog::draw()
{
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
	
	selected_ = -1;
	float size = 18.0f;
	float smallSize = 14.0f;
	Vector white(0.8f, 0.8f, 0.8f);
	Vector lightWhite(1.0f, 1.0f, 1.0f);
	for (int i=0; i<(int) definitions_.size(); i++)
	{
		OptionDefinition &definition = definitions_[i];

		// Check if option is selected
		Vector *color = &white;
		if (inBox(
			mouseX - GLWTranslate::getPosX(), 
			mouseY - GLWTranslate::getPosY(), 
			definition.x, (h_ - definition.y), 
			definition.width, 20.0f))
		{
			color = &lightWhite;
			selected_ = i;
		}

		// Draw Option
		GLWFont::instance()->getNormalFont()->draw(
			*color, size, 
			definition.x, h_ - definition.y, 0.0f,
			definition.option);
		definition.width = 
			GLWFont::instance()->getNormalFont()->getWidth(
			size, definition.option);

		// Draw Description
		if (selected_ == i)
		{
			GLWFont::instance()->getNormalFont()->draw(
				*color, smallSize, 
				definition.x + 200.0f, h_ - definition.y, 0.0f,
				definition.description);
		}
	}
}

void StartDialog::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (selected_ != -1)
	{
		skipRest = true;

		// Make sure all other options are hidden
		GLWWindowManager::instance()->hideWindow(
			ModSelectDialog::instance()->getId());
		GLWWindowManager::instance()->hideWindow(
			ModSubSelectDialog::instance()->getId());
		GLWWindowManager::instance()->hideWindow(
			SaveSelectDialog::instance()->getId());	
		GLWWindowManager::instance()->hideWindow(
			NetworkSelectDialog::instance()->getId());	
		GLWWindowManager::instance()->hideWindow(
			SettingsSelectDialog::instance()->getId());	
		GLWWindowManager::instance()->hideWindow(
			SettingsSubSelectDialog::instance()->getId());	
	}

	switch (selected_)
	{
	case 0:
		{
		const char *targetFilePath = getDataFile("data/singletutorial.xml");
		ClientParams::instance()->reset();
		ClientParams::instance()->setClientFile(targetFilePath);
		ClientMain::startClient();
		}
		break;
	case 1:
		GLWWindowManager::instance()->showWindow(
			ModSelectDialog::instance()->getId());
		break;
	case 2:
		GLWWindowManager::instance()->showWindow(
			NetworkSelectDialog::instance()->getId());
		break;
	case 3:
		GLWWindowManager::instance()->showWindow(
			SaveSelectDialog::instance()->getId());
		break;
	case 4:
		{
		showURL(formatString("file://%s", getDocFile("html/index.html")));
		}
		break;
	case 5:
		{
		const char *exec = 
			"\"https://www.paypal.com/xclick/business=donations%40"
			"scorched3d.co.uk&item_name=Scorched3D&no_note=1&tax=0&currency_code=GBP\"";
		showURL(exec);
		}
		break;
	case 6:
		ScorchedClient::instance()->getMainLoop().exitLoop();
		break;
	}
}
