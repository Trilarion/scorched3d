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

#include <scorched/ServerDialog.h>
#include <scorched/MainDialog.h>
#include <scorched/SettingsDialog.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIAdder.h>
#include <common/OptionsGame.h>
#include <common/OptionsTransient.h>
#include <common/Logger.h>
#include <coms/NetBufferUtil.h>
#include <server/ServerState.h>
#include <server/ServerMain.h>
#include <server/ServerMessageHandler.h>
#include <server/ScorchedServer.h>
#include <server/ServerCommon.h>
#include <server/ServerTooFewPlayersStimulus.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/listctrl.h>

enum 
{
	IDC_PLAYER_LIST = 550,
	IDC_LOG_LIST,
	IDC_TIMER1,
	IDC_TIMER2,
	IDC_MENU_EXIT,
	IDC_MENU_SHOWOPTIONS,
	IDC_MENU_EDITOPTIONS,
	IDC_MENU_SAVEOPTIONS,
	IDC_MENU_LOADOPTIONS,
	IDC_MENU_PLAYERTALK,
	IDC_MENU_PLAYERTALKALL,
	IDC_MENU_PLAYERKICK,
	IDC_MENU_PLAYERKILLALL,
	IDC_MENU_PLAYERADD,
	IDC_MENU_PLAYERADD_1,
	IDC_MENU_PLAYERADD_2,
	IDC_MENU_PLAYERADD_3,
	IDC_MENU_PLAYERADD_4,
	IDC_MENU_PLAYERADD_5,
	IDC_MENU_PLAYERADD_6,
	IDC_MENU_PLAYERADD_7,
	IDC_MENU_PLAYERADD_8,
	IDC_MENU_PLAYERADD_9,
	IDC_MENU_STARTNEWGAME
};

class ServerPlayerListControl : public wxListCtrl
{
public:
	ServerPlayerListControl(wxWindow* parent, wxWindowID id, 
		const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~ServerPlayerListControl();

	virtual wxString OnGetItemText(long WXUNUSED(item), long WXUNUSED(col)) const;
	virtual int OnGetItemImage(long WXUNUSED(item)) const;
};

ServerPlayerListControl::ServerPlayerListControl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) :
	wxListCtrl(parent, id, pos, size, 
		wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_VIRTUAL | wxLC_SINGLE_SEL )
{
}

ServerPlayerListControl::~ServerPlayerListControl()
{
}

int ServerPlayerListControl::OnGetItemImage(long item) const
{
	return -1;
}

wxString ServerPlayerListControl::OnGetItemText(long item, long column) const
{
	if ((item != -1) && (item < ScorchedServer::instance()->getTankContainer().getNoOfTanks()))
	{
		Tank *tank = 
			ScorchedServer::instance()->getTankContainer().getTankByPos((unsigned int) item);
		switch (column)
		{
		case 0:
			return (char *) tank->getName();
			break;
		case 1:
			{
				const char *result = "Human";
				if (tank->getTankAI())
				{
					result = ((TankAIComputer *) tank->getTankAI())->getName();
				}

				return result;
			}
			break;
		case 2:
			{
				static char buffer[256];
				sprintf(buffer, "%u", tank->getPlayerId());
				return buffer;
			}
			break;
		case 3:
			return (char *) tank->getScore().getTimePlayedString();
			break;
		case 4:
			return (char *) tank->getScore().getScoreString();
			break;
		case 5:
			return (char *) tank->getState().getStateString();
			break;
		case 6:
			if (tank->getTeam() == 0) return "None";
			else if (tank->getTeam() == 1) return "Red";
			else if (tank->getTeam() == 2) return "Green";
			break;
		case 7:
			return tank->getHostDesc();
			break;
		}
	}
	return "";
}

class ServerFrame: public wxFrame
{
public:
	ServerFrame(const char *name);

	void OnSize(wxSizeEvent& event);
	void onTimer();
	void onTimerMain();
	void onMenuExit();
	void onShowOptions();
	void onEditOptions();
	void onLoadOptions();
	void onSaveOptions();
	void onPlayerTalk();
	void onPlayerTalkAll();
	void onPlayerKick();
	void onKillAll();
	void onStartNewGame();
	void onPlayerAdd(int i);
	void onPlayerAdd1();
	void onPlayerAdd2();
	void onPlayerAdd3();
	void onPlayerAdd4();
	void onPlayerAdd5();
	void onPlayerAdd6();
	void onPlayerAdd7();
	void onPlayerAdd8();
	void onPlayerAdd9();

	ServerPlayerListControl *playerList_;
	wxListCtrl *logList_;
	wxStatusBar *statusBar_;

protected:
	wxTimer timer_;
	wxTimer timerMain_;

private:
    DECLARE_EVENT_TABLE()
};

static ServerFrame *frame = 0;

BEGIN_EVENT_TABLE(ServerFrame, wxFrame)
    EVT_SIZE(ServerFrame::OnSize)
	EVT_TIMER(IDC_TIMER1, ServerFrame::onTimer)
	EVT_TIMER(IDC_TIMER2, ServerFrame::onTimerMain)
	EVT_MENU(IDC_MENU_EXIT, ServerFrame::onMenuExit)
	EVT_MENU(IDC_MENU_SHOWOPTIONS, ServerFrame::onShowOptions)
	EVT_MENU(IDC_MENU_EDITOPTIONS, ServerFrame::onEditOptions)
	EVT_MENU(IDC_MENU_LOADOPTIONS, ServerFrame::onLoadOptions)
	EVT_MENU(IDC_MENU_SAVEOPTIONS, ServerFrame::onSaveOptions)
	EVT_MENU(IDC_MENU_PLAYERTALK, ServerFrame::onPlayerTalk)
	EVT_MENU(IDC_MENU_PLAYERTALKALL, ServerFrame::onPlayerTalkAll)
	EVT_MENU(IDC_MENU_PLAYERKICK, ServerFrame::onPlayerKick)
	EVT_MENU(IDC_MENU_PLAYERADD_1, ServerFrame::onPlayerAdd1)
	EVT_MENU(IDC_MENU_PLAYERADD_2, ServerFrame::onPlayerAdd2)
	EVT_MENU(IDC_MENU_PLAYERADD_3, ServerFrame::onPlayerAdd3)
	EVT_MENU(IDC_MENU_PLAYERADD_4, ServerFrame::onPlayerAdd4)
	EVT_MENU(IDC_MENU_PLAYERADD_5, ServerFrame::onPlayerAdd5)
	EVT_MENU(IDC_MENU_PLAYERADD_6, ServerFrame::onPlayerAdd6)
	EVT_MENU(IDC_MENU_PLAYERADD_7, ServerFrame::onPlayerAdd7)
	EVT_MENU(IDC_MENU_PLAYERADD_8, ServerFrame::onPlayerAdd8)
	EVT_MENU(IDC_MENU_PLAYERADD_9, ServerFrame::onPlayerAdd9)
	EVT_MENU(IDC_MENU_PLAYERKILLALL, ServerFrame::onKillAll)
	EVT_MENU(IDC_MENU_STARTNEWGAME, ServerFrame::onStartNewGame)
END_EVENT_TABLE()

ServerFrame::ServerFrame(const char *name) :
	wxFrame(NULL, -1, name, wxPoint(0,0), wxSize(630, 470))
{
	CentreOnScreen();

	// Create timers
	timerMain_.SetOwner(this, IDC_TIMER2);
	timerMain_.Start(10, false);
	timer_.SetOwner(this, IDC_TIMER1);
	timer_.Start(3000, false);

	// Set the backbround color to be that of the default
	// panel colour
	wxPanel panel;
	SetBackgroundColour(panel.GetBackgroundColour());

#ifdef _WIN32
	// Set the frame's icon
	wxIcon icon(getDataFile("data/windows/tank2.ico"), wxBITMAP_TYPE_ICO);
	SetIcon(icon);
#endif

	// Create all the display controlls
	playerList_ = 
		new ServerPlayerListControl(this, IDC_PLAYER_LIST,
		wxDefaultPosition, wxDefaultSize);
	logList_ =
		new wxListCtrl(this, IDC_LOG_LIST, wxDefaultPosition, wxDefaultSize, 
		wxLC_REPORT | wxLC_HRULES | wxLC_VRULES | wxLC_SINGLE_SEL );

	// Setup list collumns
	// Setup the list control
	struct ListItem
	{
		char *name;
		int size;
	};
	int i;
	ListItem playerListItems[] =
		{
			{ "Player Name", 100 },
			{ "Player Type", 80 },
			{ "Player Id", 80 },
			{ "Played Time", 100 },
			{ "Player Score", 100 },
			{ "Player State", 140 },
			{ "Player Team", 100 },
			{ "Player OS", 140 }
		};
	for (i=0; i<sizeof(playerListItems)/sizeof(ListItem); i++)
	{
		playerList_->InsertColumn(
			i,
			playerListItems[i].name,
			wxLIST_FORMAT_LEFT,
			playerListItems[i].size);
	}
	ListItem mainListItems[] =
		{
			{ "Server Log Messages", 600 }
		};
	for (i=0; i<sizeof(mainListItems)/sizeof(ListItem); i++)
	{
		logList_->InsertColumn(
			i,
			mainListItems[i].name,
			wxLIST_FORMAT_LEFT,
			mainListItems[i].size);
	}

	// Add menu items
	wxMenu *menuFile = new wxMenu;
    menuFile->Append(IDC_MENU_SHOWOPTIONS, "&Display Options");
	menuFile->Append(IDC_MENU_EDITOPTIONS, "&Edit Options");
	menuFile->Append(IDC_MENU_LOADOPTIONS, "&Load Options");
	menuFile->Append(IDC_MENU_SAVEOPTIONS, "&Save Options");
    menuFile->AppendSeparator();
    menuFile->Append(IDC_MENU_EXIT, "E&xit");

	wxMenu *menuAddPlayer = new wxMenu;
	int aicount = 0;
	std::list<TankAIComputer *> &ais = TankAIStore::instance()->getAis();
	std::list<TankAIComputer *>::iterator aiitor;
	for (aiitor = ais.begin();
		aiitor != ais.end();
		aiitor++, aicount++)
	{
		TankAIComputer *ai = (*aiitor);
		char buffer[256];
		sprintf(buffer, "Add %s", ai->getName());
		menuAddPlayer->Append(IDC_MENU_PLAYERADD_1 + aicount, buffer);
	}

	wxMenu *menuPlayer = new wxMenu;
	menuPlayer->Append(IDC_MENU_PLAYERTALK, "Talk to selected players");
	menuPlayer->Append(IDC_MENU_PLAYERTALKALL, "Talk to all players");
	menuPlayer->Append(IDC_MENU_PLAYERKICK, "Kick selected players");
	menuPlayer->Append(IDC_MENU_PLAYERKILLALL, "Kill all players");
	menuPlayer->Append(IDC_MENU_PLAYERADD, "Add a new player", menuAddPlayer);

	wxMenu *menuGame = new wxMenu;
	menuGame->Append(IDC_MENU_STARTNEWGAME, "Start a new game");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
	menuBar->Append(menuPlayer, "&Players");
	menuBar->Append(menuGame, "&Game");
    SetMenuBar( menuBar );

#ifdef __WXMSW__
	SendSizeEvent();
#endif

	// Create the bottom status bar
	statusBar_ = CreateStatusBar(4);
}

void ServerFrame::onPlayerAdd(int i)
{
	if (ScorchedServer::instance()->getTankContainer().getNoOfTanks() <
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers())
	{
		int aicount = 1;
		std::list<TankAIComputer *> &ais = TankAIStore::instance()->getAis();
		std::list<TankAIComputer *>::iterator aiitor;
		for (aiitor = ais.begin();
			aiitor != ais.end();
			aiitor++, aicount++)
		{
			TankAIComputer *ai = (*aiitor);
			if (aicount == i)
			{
				TankAIAdder::addTankAI(ScorchedServer::instance()->getContext(),
					ai->getName(), "Random", "", true);
			}
		}
	}
}

void ServerFrame::onPlayerAdd1() { onPlayerAdd(1); }
void ServerFrame::onPlayerAdd2() { onPlayerAdd(2); }
void ServerFrame::onPlayerAdd3() { onPlayerAdd(3); }
void ServerFrame::onPlayerAdd4() { onPlayerAdd(4); }
void ServerFrame::onPlayerAdd5() { onPlayerAdd(5); }
void ServerFrame::onPlayerAdd6() { onPlayerAdd(6); }
void ServerFrame::onPlayerAdd7() { onPlayerAdd(7); }
void ServerFrame::onPlayerAdd8() { onPlayerAdd(8); }
void ServerFrame::onPlayerAdd9() { onPlayerAdd(9); }

void ServerFrame::onMenuExit()
{
	Close();
}

void ServerFrame::OnSize(wxSizeEvent& event)
{
	wxSize size = GetClientSize();
	wxCoord y = (2*size.y)/3;
	logList_->SetSize(0, 0, size.x, y);
	playerList_->SetSize(0, y + 1, size.x, size.y - y);
	event.Skip();
}

void ServerFrame::onTimerMain()
{
	serverLoop();
}

void ServerFrame::onTimer()
{
	// Update the player list
	if (frame->playerList_->GetItemCount() !=
		ScorchedServer::instance()->getTankContainer().getNoOfTanks())
	{
		frame->playerList_->SetItemCount(
			ScorchedServer::instance()->getTankContainer().getNoOfTanks());
	}
	frame->playerList_->Refresh();

	// Update the status bar
	char buffer[256];
	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	sprintf(buffer, "%i/%i Players", tanks.size(), 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers());
	frame->statusBar_->SetStatusText(buffer, 0);
	frame->statusBar_->SetStatusText(
		(ServerTooFewPlayersStimulus::instance()->acceptStateChange(0, 0, 0.0f)?"Not Playing":"Playing"), 1);
	sprintf(buffer, "Round %i/%i, %i/%i Moves",
		ScorchedServer::instance()->getOptionsGame().getNoRounds() - 
		ScorchedServer::instance()->getOptionsTransient().getNoRoundsLeft(),
		ScorchedServer::instance()->getOptionsGame().getNoRounds(),
		ScorchedServer::instance()->getOptionsTransient().getCurrentGameNo(),
		ScorchedServer::instance()->getOptionsGame().getNoMaxRoundTurns());
	frame->statusBar_->SetStatusText(buffer, 2);
	sprintf(buffer, "BI:%i BO:%i P:%i C:%i",
		NetInterface::getBytesIn(),
		NetInterface::getBytesOut(),
		NetInterface::getPings(),
		NetInterface::getConnects());
	frame->statusBar_->SetStatusText(buffer, 3);
}

void ServerFrame::onKillAll()
{
	ServerCommon::killAll();
}

void ServerFrame::onStartNewGame()
{
	onKillAll();
	ScorchedServer::instance()->getOptionsTransient().startNewGame();
}

void ServerFrame::onPlayerTalkAll()
{
	wxTextEntryDialog entryDialog(
		frame, "Message to players");
	if (entryDialog.ShowModal() == wxID_OK)
	{
		Logger::log(0, "Says \"%s\"", 
			entryDialog.GetValue().GetData());
		ServerCommon::sendString(0, entryDialog.GetValue());
	}
}

void ServerFrame::onPlayerTalk()
{
	wxTextEntryDialog entryDialog(
		frame, "Message to players");
	if (entryDialog.ShowModal() == wxID_OK)
	{
		Logger::log(0, "Says \"%s\"", 
			entryDialog.GetValue().GetData());

		long item = -1;
		while ((item = frame->playerList_->GetNextItem(
			item,
			wxLIST_NEXT_ALL,
			wxLIST_STATE_SELECTED)) != -1)
		{
			if ((item != -1) && 
				(item < ScorchedServer::instance()->getTankContainer().getNoOfTanks()))
			{
				Tank *tank = 
					ScorchedServer::instance()->getTankContainer().getTankByPos((unsigned int) item);
				if (!tank->getTankAI())
				{
					ServerCommon::sendString(tank->getDestinationId(), entryDialog.GetValue());
				}
			}
		}
	}
}

void ServerFrame::onPlayerKick()
{
	long item = -1;
    while ((item = frame->playerList_->GetNextItem(
		item,
		wxLIST_NEXT_ALL,
        wxLIST_STATE_SELECTED)) != -1)
    {
		if ((item != -1) && (item < 
			ScorchedServer::instance()->getTankContainer().getNoOfTanks()))
		{
			Tank *tank = 
				ScorchedServer::instance()->getTankContainer().getTankByPos((unsigned int) item);
			if (tank->getDestinationId() == 0)
			{
				ServerMessageHandler::instance()->destroyPlayer(tank->getPlayerId());
			}
			else
			{
				ServerCommon::kickDestination(tank->getDestinationId());
			}
		}		
    }
}

void ServerFrame::onEditOptions()
{
	if(showSettingsDialog(true, ScorchedServer::instance()->
		getOptionsGame().getChangedOptions()))
	{
		dialogMessage("Server",
			"These changes will only take place at the start of the next round (map).\n"
			"Be careful, some changes may not work without restarting server.");
	}
}

void ServerFrame::onLoadOptions()
{
	wxString file = ::wxFileSelector("Please choose the options file to open",
									 getDataFile("data"), // default path
									 "server.xml", // default filename
									 "", // default extension
									 "*.xml",
									 wxOPEN | wxFILE_MUST_EXIST);
	if (!file.empty())
	{
		if(ScorchedServer::instance()->getOptionsGame().getChangedOptions().
		   readOptionsFromFile((char *) file.c_str()))
		{
			dialogMessage("Server",
						  "These changes will only take place at the start of the next round (map).\n"
						  "Be careful, some changes may not work without restarting server.");
		}
	}
}

void ServerFrame::onSaveOptions()
{
	wxString file = ::wxFileSelector("Please choose the options file to save",
									 getDataFile("data"), // default path
									 "server.xml", // default filename
									 "", // default extension
									 "*.xml",
									 wxSAVE);
	if (!file.empty())
	{
		ScorchedServer::instance()->getOptionsGame().getChangedOptions().
			writeOptionsToFile((char *) file.c_str());
	}
}

void ServerFrame::onShowOptions()
{
	char allOptionsStr[2048];
	allOptionsStr[0] = '\0';
	std::list<OptionEntry *> &allOptions = 
		ScorchedServer::instance()->getOptionsGame().getOptions();
	std::list<OptionEntry *>::iterator itor;
	for (itor = allOptions.begin();
		itor != allOptions.end();
		itor++)
	{
		OptionEntry *entry = (*itor);
		strcat(allOptionsStr, entry->getName());
		strcat(allOptionsStr, " = ");
		strcat(allOptionsStr, entry->getValueAsString());
		strcat(allOptionsStr, "\n");
	}
	dialogMessage("Scorched 3D Server Options", allOptionsStr);
}

static class ServerLogger : public LoggerI
{
public:
	virtual void logMessage(
		const char *time,
		const char *message,
		unsigned int playerId)
	{
		// Make sure the list does not exceed 500 entries
		if (frame->logList_->GetItemCount() > 499)
		{
			// Remove the top 20 entries
			for (int j=0; j<20; j++) frame->logList_->DeleteItem(0);
		}

		// Add a new entry and ensure it is visible
		static char text[1024];
		sprintf(text, "%s - %s", time, message);
		long index = frame->logList_->InsertItem(
			frame->logList_->GetItemCount(), text);
		frame->logList_->EnsureVisible(index);
	};
} serverLogger;

void showServerDialog()
{
	char serverName[1024];
	sprintf(serverName, "Scorched 3D Dedicated Server [ %s : %i ] [%s]",
			ScorchedServer::instance()->getOptionsGame().getServerName(), 
			ScorchedServer::instance()->getOptionsGame().getPortNo(), 
			(ScorchedServer::instance()->getOptionsGame().getPublishServer()?"Published":"Not Published"));

	frame = new ServerFrame(serverName);
	frame->Show();

	Logger::addLogger(&serverLogger);
	ServerCommon::startFileLogger();
}
