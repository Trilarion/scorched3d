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

#include <dialogs/MainMenuDialog.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/SoundDialog.h>
#include <common/Defines.h>
#include <GLW/GLWWindowManager.h>
#include <GLEXT/GLBitmap.h>
#include <GLEXT/GLMenu.h>

HelpButtonDialog *HelpButtonDialog::instance_ = 0;

HelpButtonDialog *HelpButtonDialog::instance()
{
	if (!instance_)
	{
		instance_ = new HelpButtonDialog();
	}
	return instance_;
}

HelpButtonDialog::HelpButtonDialog()
{
}

HelpButtonDialog::~HelpButtonDialog()
{
}

HelpButtonDialog::HelpMenu::HelpMenu()
{
	GLBitmap *map = new GLBitmap(
		formatString(getDataFile("data/windows/help.bmp")),
		formatString(getDataFile("data/windows/helpa.bmp")),
		false);
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu("Help", 32.0f, 0, this, map,
		GLMenu::eMenuAlignRight);
}

GLTexture &HelpButtonDialog::HelpMenu::getHelpTexture()
{
	if (!helpTexture_.textureValid())
	{
		GLBitmap map(
			formatString(getDataFile("data/windows/help.bmp")),
			formatString(getDataFile("data/windows/helpa.bmp")),
			false);
		helpTexture_.create(map, GL_RGBA, false);
	}
	return helpTexture_;
}

bool HelpButtonDialog::HelpMenu::getMenuItems(const char* menuName, std::list<GLMenuItem> &result)
{
	result.push_back(GLMenuItem("Show Online Help"));
	return true;
}

void HelpButtonDialog::HelpMenu::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	showURL(formatString("file://%s", getDocFile("html/index.html")));
}

HelpButtonDialog::VolumeMenu::VolumeMenu()
{
	GLBitmap *map = new GLBitmap(
		formatString(getDataFile("data/windows/sound.bmp")),
		formatString(getDataFile("data/windows/sounda.bmp")),
		false);
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->
		addMenu("Volume", 32.0f, 0, this, map,
		GLMenu::eMenuAlignRight);
}

bool HelpButtonDialog::VolumeMenu::menuOpened(const char* menuName)
{
	GLWWindowManager::instance()->showWindow(
		SoundDialog::instance()->getId());
	return false;
}
