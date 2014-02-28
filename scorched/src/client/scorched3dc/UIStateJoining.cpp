////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2013
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

#include <scorched3dc/ScorchedUI.h>
#include <scorched3dc/UIStateJoining.h>
#include <scorched3dc/UIProgressCounter.h>
#include <engine/ThreadCallback.h>
#include <client/ScorchedClient.h>
#include <client/ClientJoinGameThreadCallback.h>

UIStateJoining::UIStateJoining() : UIStateI(UIState::StateJoining)
{
}

UIStateJoining::~UIStateJoining()
{
}

void UIStateJoining::createState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

	{
		CEGUI::Window *join = wmgr.createWindow("OgreTray/Button", "CEGUIDemo/JoinButton");
		join->setText("Join");
		join->setPosition(CEGUI::UVector2(CEGUI::UDim(0.3f, 0.0f), CEGUI::UDim(0.00f, 0.0f)));
		join->setSize(CEGUI::UVector2(CEGUI::UDim(0.15f, 0.0f), CEGUI::UDim(0.05f, 0.0f)));
		join->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&UIStateJoining::join, this));
		sheet->addChildWindow(join);
	}

	CEGUI::System::getSingleton().setGUISheet(sheet);
}


bool UIStateJoining::join(const CEGUI::EventArgs &e)
{
	ScorchedUI::instance()->getUIState().setState(UIState::StateProgress);
	UIProgressCounter::instance()->setNewOp(LANG_STRING("Waiting for server..."));
	ScorchedClient::getClientThreadCallback().addCallback(new ClientJoinGameThreadCallback());
	return true;
}

void UIStateJoining::destroyState()
{
	CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
	wmgr.destroyAllWindows();
}
