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

#include <wx/wx.h>
#include <wx/utils.h>
#include <wx/image.h>
#include <wx/filedlg.h>
#include <wx/dir.h>
#include <scorched/MainDialog.h>
#include <scorched/SingleSDialog.h>
#include <scorched/SingleGames.h>
#include <scorched/SingleChoiceDialog.h>
#include <common/Defines.h>
#include <common/OptionsGame.h>
#include <common/OptionsParam.h>
#include <set>
#include <string>
#include <string.h>

extern char scorched3dAppName[128];

enum
{
	ID_BUTTON_EASY = 1,
	ID_BUTTON_CUSTOM,
	ID_BUTTON_LOAD,
	ID_BUTTON_GAME = 100
};

class SingleFrameData : public wxObjectRefData
{
public:
	SingleFrameData(const char *m) : mod(m) { }
	virtual ~SingleFrameData() { }

	std::string mod;
};

class SingleFrame: public wxDialog
{
public:
	SingleFrame();

	void onGameButton(wxCommandEvent &event);
	void onCustomButton();
	void onLoadButton();

protected:
	void addModButton(
		int &count,
		std::set<std::string> &mods, 
		const char *mod, wxSizer *sizer);

private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(SingleFrame, wxDialog)
	EVT_BUTTON(ID_BUTTON_CUSTOM,  SingleFrame::onCustomButton)
	EVT_BUTTON(ID_BUTTON_LOAD, SingleFrame::onLoadButton)
	EVT_BUTTON(ID_BUTTON_GAME,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 1,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 2,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 3,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 4,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 5,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 6,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 7,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 8,  SingleFrame::onGameButton)
	EVT_BUTTON(ID_BUTTON_GAME + 9,  SingleFrame::onGameButton)
END_EVENT_TABLE()

SingleFrame::SingleFrame() :
	wxDialog(getMainDialog(), -1, scorched3dAppName, 
		wxDefaultPosition, wxDefaultSize)
{
	// Create the positioning sizer
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

#ifdef _WIN32
	// Set the frame's icon
	wxIcon icon(getDataFile("data/windows/tank2.ico"), wxBITMAP_TYPE_ICO);
	SetIcon(icon);
#endif

	wxFlexGridSizer *gridsizer = new wxFlexGridSizer(4, 2, 5, 5);

	std::set<std::string> loadedMods;
	int count = ID_BUTTON_GAME;
	addModButton(count, loadedMods, "", gridsizer);

	{
		wxDir dir(getModFile(""));
		if (dir.IsOpened())
		{
			wxString filename;
			bool cont = dir.GetFirst(&filename, "", wxDIR_DIRS);
			while (cont)
			{
				addModButton(count, loadedMods, 
					filename.c_str(), gridsizer);
				cont = dir.GetNext(&filename);
			}
		}
	}
	{
		wxDir dir(getGlobalModFile(""));
		if (dir.IsOpened())
		{
			wxString filename;
			bool cont = dir.GetFirst(&filename, "", wxDIR_DIRS);
			while (cont)
			{
				addModButton(count, loadedMods, 
					filename.c_str(), gridsizer);
				cont = dir.GetNext(&filename);
			}
		}
	}

	{
		addButtonToWindow(ID_BUTTON_CUSTOM,
			"Start an custom single or multi-player game.\n"
			"Choose the opponents to play against,\n"
			"the settings and the mod.",
			"data/windows/tank-cus.bmp", this, gridsizer);
	}

	{
		addButtonToWindow(ID_BUTTON_LOAD,
			"Load a previously saved single or multi-player game.", 
			"data/windows/save.bmp", this, gridsizer);
	}
	topsizer->Add(gridsizer, 0, wxALIGN_CENTER | wxALL, 5);

	// Quit button
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Close"), 0, wxALL, 5);
	topsizer->Add(buttonSizer, 0, wxALIGN_RIGHT);

	// use the sizer for layout
	SetSizer(topsizer); 
	topsizer->SetSizeHints(this); // set size hints to honour minimum size

	CentreOnScreen();
}

void SingleFrame::addModButton(
	int &count,
	std::set<std::string> &mods, 
	const char *mod,
	wxSizer *sizer)
{
	_strlwr((char *) mod);
	if (mods.find(mod) != mods.end()) return;

	std::string noModGamesFile = getDataFile("data/singlegames.xml");
	setDataFileMod(mod);
	std::string modGamesFile = getDataFile("data/singlegames.xml");
	if (mod[0] && noModGamesFile == modGamesFile) return;
	mods.insert(mod);

	SingleGames games;
	if (!games.parse(modGamesFile.c_str())) 
		dialogExit("SingleFrame", "Failed to load \"%s\" games", mod);
	wxObjectRefData *refData = new SingleFrameData(mod);
	addButtonToWindow(count++, 
		(char *) games.description.c_str(), 
		(char *) games.icon.c_str(), 
		this, sizer, refData);

	setDataFileMod("");
}

void SingleFrame::onLoadButton()
{
	wxString file = ::wxFileSelector("Please choose the saved game to load",
									 getSaveFile(""), // default path
									 "", // default filename
									 "", // default extension
									 "*.s3d",
									 wxOPEN | wxFILE_MUST_EXIST);
	if (!file.empty())
	{
		EndModal(wxID_OK);
		runScorched3D("-loadsave \"%s\"", file.c_str());
	}
}

void SingleFrame::onGameButton(wxCommandEvent &event)
{
	wxObject *object = event.GetEventObject();
	if (!object) return;
	SingleFrameData *data = 
		(SingleFrameData *) object->GetRefData();
	if (!data) return;
	const char *mod = data->mod.c_str();

	if (showSingleChoiceDialog(mod))
	{
		EndModal(wxID_OK);
	}
}

void SingleFrame::onCustomButton()
{
	if (showSingleSDialog())
	{
		EndModal(wxID_OK);
	}
}

bool showSingleDialog()
{
	SingleFrame frame;
	return (frame.ShowModal() == wxID_OK);
}
