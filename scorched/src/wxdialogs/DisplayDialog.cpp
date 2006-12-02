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

#include <wxdialogs/DisplayDialog.h>
#include <wxdialogs/MainDialog.h>
#include <wxdialogs/KeyDialog.h>
#include <net/NetInterface.h>
#include <engine/ModDirs.h>
#include <engine/ModFiles.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <common/Keyboard.h>
#include <common/KeyTranslateWx.h>
#include <net/UniqueIdStore.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/notebook.h>
#include <wx/grid.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <set>
#include <stdio.h>
#include "Display.cpp"

extern char scorched3dAppName[128];
extern char *displayOptions;

class DisplayFrame: public wxDialog
{
public:
	DisplayFrame();

	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();

	void onLoadDefaultsButton(wxCommandEvent &event);
	void onLoadFastestButton(wxCommandEvent &event);
	void onLoadSafeButton(wxCommandEvent &event);
	void onLoadDefaultKeysButton(wxCommandEvent &event);
	void onKeyButton(wxCommandEvent &event);
	void onImportMod(wxCommandEvent &event);
	void onExportMod(wxCommandEvent &event);
	void onMoreRes(wxCommandEvent &event);
	void onPageChange(wxNotebookEvent &event);
	void onKey();

	wxNotebook *book_;
	wxPanel *mainPanel_;
	wxPanel *troublePanel_;
	wxPanel *identPanel_;
	wxPanel *keysPanel_;
	wxPanel *modsPanel_;

protected:
	DECLARE_EVENT_TABLE()

	void refreshScreen();
	void refreshResolutions();
	void refreshKeysControls();
};

BEGIN_EVENT_TABLE(DisplayFrame, wxDialog)
    EVT_BUTTON(ID_LOADDEFAULTS,  DisplayFrame::onLoadDefaultsButton)
	EVT_BUTTON(ID_LOADFASTEST,  DisplayFrame::onLoadFastestButton)
	EVT_BUTTON(ID_KEYDEFAULTS,  DisplayFrame::onLoadDefaultKeysButton)
    EVT_BUTTON(ID_LOADSAFE,  DisplayFrame::onLoadSafeButton)
    EVT_BUTTON(ID_KEY,  DisplayFrame::onKeyButton)
	EVT_BUTTON(ID_IMPORT,  DisplayFrame::onImportMod)
	EVT_BUTTON(ID_EXPORT,  DisplayFrame::onExportMod)
	EVT_CHECKBOX(ID_MORERES, DisplayFrame::onMoreRes) 
	EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, DisplayFrame::onPageChange)
END_EVENT_TABLE()

DisplayFrame::DisplayFrame() :
	wxDialog(getMainDialog(), -1, wxString(scorched3dAppName, wxConvUTF8))
{
#ifdef _WIN32
	// Set the frame's icon
	wxIcon icon(getDataFile("data/windows/tank2.ico"), wxBITMAP_TYPE_ICO);
	SetIcon(icon);
#endif

	// Create the size for the layout
	wxBoxSizer *topsizer = new wxBoxSizer(wxVERTICAL);

	// Create all the display controls
	book_ = new wxNotebook(this, ID_NOTEBOOK);
	wxNotebookSizer *nbs = new wxNotebookSizer(book_);

	// Main Panel
	mainPanel_ = new wxPanel(book_, -1);
	book_->AddPage(mainPanel_, wxT("Main"));
	wxSizer *mainPanelSizer = new wxBoxSizer(wxVERTICAL);
	createMainControls(mainPanel_, mainPanelSizer);
	mainPanelSizer->Add(new wxStaticText(
		mainPanel_, -1,wxString(formatString("Settings Dir : %s", 
		getSettingsFile("")), wxConvUTF8)), 0, wxTOP, 10);
	mainPanel_->SetAutoLayout(TRUE);
	mainPanel_->SetSizer(mainPanelSizer);
	
	// Trouble Panel
	troublePanel_ = new wxPanel(book_, -1);
	wxSizer *troublePanelSizer = new wxBoxSizer(wxVERTICAL);
	createTroubleControls(troublePanel_, troublePanelSizer);
	book_->AddPage(troublePanel_, wxT("Troubleshooting / Detail"));
	troublePanel_->SetAutoLayout(TRUE);
	troublePanel_->SetSizer(troublePanelSizer);

	// Keys Panel
	keysPanel_ = new wxPanel(book_, -1);
	wxSizer *keysPanelSizer = new wxBoxSizer(wxVERTICAL);
	createKeysControls(keysPanel_, keysPanelSizer);
	book_->AddPage(keysPanel_, wxT("Keys"));
	keysPanel_->SetAutoLayout(TRUE);
#if wxCHECK_VERSION(2,6,0)
	keysPanel_->SetSizer(keysPanelSizer);
#else
#if wxCHECK_VERSION(2,5,0)
#else
	keysPanel_->SetSizer(keysPanelSizer);
#endif
#endif

	// Ident Panel
	identPanel_ = new wxPanel(book_, ID_PANEL_IDENT);
	wxSizer *identPanelSizer = new wxBoxSizer(wxVERTICAL);
	createIdentControls(identPanel_, identPanelSizer);
	book_->AddPage(identPanel_, wxT("Identity"));
	identPanel_->SetAutoLayout(TRUE);
	identPanel_->SetSizer(identPanelSizer);

	// Mods Panel
	modsPanel_ = new wxPanel(book_, -1);
    wxSizer *modsPanelSizer = new wxBoxSizer(wxVERTICAL);
    createModsControls(modsPanel_, modsPanelSizer);
    book_->AddPage(modsPanel_, wxT("Mods"));
    modsPanel_->SetAutoLayout(TRUE);
    modsPanel_->SetSizer(modsPanelSizer);

	// Notebook
	topsizer->Add(nbs, 0, wxALL, 10);

	// Ok and cancel boxes
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	IDOK_CTRL = new wxButton(this, wxID_OK, wxT("Ok"));
	IDCANCEL_CTRL = new wxButton(this, wxID_CANCEL, wxT("Cancel"));
	buttonSizer->Add(IDCANCEL_CTRL, 0, wxRIGHT, 5);
	buttonSizer->Add(IDOK_CTRL, 0, wxLEFT, 5);
	topsizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

	SetSizer(topsizer); // use the sizer for layout
	topsizer->SetSizeHints(this); // set size hints to honour minimum size

	CentreOnScreen();
}

void DisplayFrame::onLoadDefaultsButton(wxCommandEvent &event)
{
	OptionsDisplay::instance()->loadDefaultValues();
	refreshScreen();
}

void DisplayFrame::onLoadFastestButton(wxCommandEvent &event)
{
	OptionsDisplay::instance()->loadDefaultValues();
	OptionsDisplay::instance()->loadFastestValues();
	refreshScreen();
}

void DisplayFrame::onLoadSafeButton(wxCommandEvent &event)
{
	OptionsDisplay::instance()->loadDefaultValues();
	OptionsDisplay::instance()->loadSafeValues();
	refreshScreen();
}

void DisplayFrame::onLoadDefaultKeysButton(wxCommandEvent &event)
{
	Keyboard::instance()->loadKeyFile(true);
	refreshKeysControls();
}

void DisplayFrame::onMoreRes(wxCommandEvent &event)
{
	refreshResolutions();
}

void DisplayFrame::onPageChange(wxNotebookEvent &event)
{
	if (event.GetSelection() == 3)
	{
		refreshIdentControls();
	}
}

void DisplayFrame::onKeyButton(wxCommandEvent &event)
{
	// Find which button was pressed
	wxButton *chosenButton = (wxButton *) event.GetEventObject();
	KeyButtonData *chosenData = (KeyButtonData *) chosenButton->GetRefData();
	KeyboardKey *chosenKey = Keyboard::instance()->getKey(chosenData->key_.c_str());
	unsigned int chosenPosition = chosenData->position_;
	if (!chosenKey) return;

	// Ask the user for a key
	showKeyDialog(this);
	
	// Translate the key code from wx to SDL	
	unsigned int resultKey = getKeyDialogKey();
	unsigned int wxKey = (unsigned int) resultKey;
	unsigned int sdlKey = 0;

	for (int i=0; i<sizeof(KeyTranslationTableWx)/sizeof(KeyTranslationWx); i++)
	{
		if (KeyTranslationTableWx[i].wxKeySym == wxKey)
		{
			sdlKey = KeyTranslationTableWx[i].keySym;
			break;
		}
	}

	if (sdlKey == 0) return;

	unsigned int sdlState = 0;
	if (getKeyDialogAlt()) sdlState = KMOD_LALT;
	else if (getKeyDialogControl()) sdlState = KMOD_LCTRL;
	else if (getKeyDialogShift()) sdlState = KMOD_LSHIFT;

	// Remove any existing occurances of this key 
	// (except when it exists on the chosen key)
	bool found = false;
	std::list<wxButton *>::iterator itor;
	for (itor = keyboardKeyList.begin();
		itor != keyboardKeyList.end();
		itor++)
	{
		wxButton *button = (*itor);
		KeyButtonData *data = (KeyButtonData *) button->GetRefData();
		KeyboardKey *key = Keyboard::instance()->getKey(data->key_.c_str());
		unsigned int position = data->position_;

		if (key)
		{
			std::vector<KeyboardKey::KeyEntry> &keyEntries = key->getKeys();
			if (position < keyEntries.size())
			{
				if (keyEntries[position].key == sdlKey &&
					keyEntries[position].state == sdlState)
				{
					if (key == chosenKey) found = true;
					else key->removeKey(position);
				}
			}
		}
	}

	// Add this key to the chosen item (unless it already exists)
	if (!found)
	{
		chosenKey->addKey(chosenPosition, sdlKey, sdlState);
	}

	// Rerfresh the key view
	refreshKeysControls();
}

void DisplayFrame::refreshKeysControls()
{
	std::list<wxButton *>::iterator buttonitor;
	for (buttonitor = keyboardKeyList.begin();
		buttonitor != keyboardKeyList.end();
		buttonitor++)
	{
		wxButton *button = (*buttonitor);			

		KeyButtonData *data = (KeyButtonData *) button->GetRefData();
		KeyboardKey *key = Keyboard::instance()->getKey(data->key_.c_str());
		unsigned int position = data->position_;

		char buffer[256];
		buffer[0] = '\0';
		if (key && position < key->getKeys().size())
		{
			const char *keyName = "";
			const char *stateName = "";
			KeyboardKey::translateKeyNameValue(key->getKeys()[position].key, keyName);
			KeyboardKey::translateKeyStateValue(key->getKeys()[position].state, stateName);
			if (strcmp(stateName, "NONE") == 0) snprintf(buffer, 256, "%s", keyName);
			else snprintf(buffer, 256, "<%s> %s", stateName, keyName);
		}
		button->SetLabel(wxString(buffer, wxConvUTF8));
	}
}

bool DisplayFrame::TransferDataToWindow()
{
	// Read display options from a file
	OptionsDisplay::instance()->readOptionsFromFile();

	refreshScreen();
	return true;
}

void DisplayFrame::refreshScreen()
{
	IDC_FULLCLEAR_CTRL->SetValue(OptionsDisplay::instance()->getFullClear());
	IDC_FULLCLEAR_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getFullClearEntry().getDescription(), wxConvUTF8));
	IDC_NOEXT_CTRL->SetValue(OptionsDisplay::instance()->getNoGLExt());
	IDC_NOEXT_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLExtEntry().getDescription(), wxConvUTF8));
	IDC_NOLANDSCAPESCORCH_CTRL->SetValue(OptionsDisplay::instance()->getNoGLTexSubImage());
	IDC_NOLANDSCAPESCORCH_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLTexSubImageEntry().getDescription(), wxConvUTF8));
	IDC_NOMULTITEX_CTRL->SetValue(OptionsDisplay::instance()->getNoGLMultiTex());
	IDC_NOMULTITEX_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLMultiTexEntry().getDescription(), wxConvUTF8));
	IDC_NOCOMPILEDARRAYS_CTRL->SetValue(OptionsDisplay::instance()->getNoGLCompiledArrays());
	IDC_NOCOMPILEDARRAYS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLCompiledArraysEntry().getDescription(), wxConvUTF8));
	IDC_NOENVCOMBINE_CTRL->SetValue(OptionsDisplay::instance()->getNoGLEnvCombine());
	IDC_NOENVCOMBINE_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLEnvCombineEntry().getDescription(), wxConvUTF8));
	IDC_NOCUBEMAP_CTRL->SetValue(OptionsDisplay::instance()->getNoGLCubeMap());
	IDC_NOCUBEMAP_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLCubeMapEntry().getDescription(), wxConvUTF8));
	IDC_NOSPHEREMAP_CTRL->SetValue(OptionsDisplay::instance()->getNoGLSphereMap());
	IDC_NOSPHEREMAP_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLSphereMapEntry().getDescription(), wxConvUTF8));
	IDC_NOVBO_CTRL->SetValue(OptionsDisplay::instance()->getNoVBO());
	IDC_NOVBO_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoVBOEntry().getDescription(), wxConvUTF8));
	IDC_NOMIPMAPS_CTRL->SetValue(OptionsDisplay::instance()->getNoGLHardwareMipmaps());
	IDC_NOMIPMAPS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoGLHardwareMipmapsEntry().getDescription(), wxConvUTF8));
	IDC_NOSOUND_CTRL->SetValue(OptionsDisplay::instance()->getNoSound());
	IDC_NOSOUND_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoSoundEntry().getDescription(), wxConvUTF8));
	IDC_NOAMBIENTSOUND_CTRL->SetValue(OptionsDisplay::instance()->getNoAmbientSound());
	IDC_NOAMBIENTSOUND_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoAmbientSoundEntry().getDescription(), wxConvUTF8));
	IDC_NOBOIDSOUND_CTRL->SetValue(OptionsDisplay::instance()->getNoBoidSound());
	IDC_NOBOIDSOUND_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoBoidSoundEntry().getDescription(), wxConvUTF8));
	IDC_NOSKINS_CTRL->SetValue(OptionsDisplay::instance()->getNoSkins());
	IDC_NOSKINS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoSkinsEntry().getDescription(), wxConvUTF8));
	IDC_NODYNAMICLIGHT_CTRL->SetValue(OptionsDisplay::instance()->getNoModelLighting());
	IDC_NODYNAMICLIGHT_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoModelLightingEntry().getDescription(), wxConvUTF8));
	IDC_FULLSCREEN_CTRL->SetValue(OptionsDisplay::instance()->getFullScreen());
	IDC_FULLSCREEN_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getFullScreenEntry().getDescription(), wxConvUTF8));
	IDC_SINGLESKYLAYER_CTRL->SetValue(OptionsDisplay::instance()->getNoSkyLayers());
	IDC_SINGLESKYLAYER_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoSkyLayersEntry().getDescription(), wxConvUTF8));
	IDC_NOSKYANI_CTRL->SetValue(OptionsDisplay::instance()->getNoSkyMovement());
	IDC_NOSKYANI_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoSkyMovementEntry().getDescription(), wxConvUTF8));
	IDC_NOWATERANI_CTRL->SetValue(OptionsDisplay::instance()->getNoWaterMovement());
	IDC_NOWATERANI_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoWaterMovementEntry().getDescription(), wxConvUTF8));
	IDC_NOWATER_CTRL->SetValue(!OptionsDisplay::instance()->getDrawWater());
	IDC_NOWATER_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDrawWaterEntry().getDescription(), wxConvUTF8));
	IDC_NOSURROUND_CTRL->SetValue(!OptionsDisplay::instance()->getDrawSurround());
	IDC_NOSURROUND_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDrawSurroundEntry().getDescription(), wxConvUTF8));
	IDC_NOTREES_CTRL->SetValue(OptionsDisplay::instance()->getNoTrees());
	IDC_NOTREES_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoTreesEntry().getDescription(), wxConvUTF8));
	IDC_NOPRECIPITATION_CTRL->SetValue(OptionsDisplay::instance()->getNoPrecipitation());
	IDC_NOPRECIPITATION_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoPrecipitationEntry().getDescription(), wxConvUTF8));
	IDC_NOWAVES_CTRL->SetValue(OptionsDisplay::instance()->getNoWaves());
	IDC_NOWAVES_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoWavesEntry().getDescription(), wxConvUTF8));
	IDC_NODEPTHSORT_CTRL->SetValue(OptionsDisplay::instance()->getNoDepthSorting());
	IDC_NODEPTHSORT_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoDepthSortingEntry().getDescription(), wxConvUTF8));
	IDC_INVERT_CTRL->SetValue(OptionsDisplay::instance()->getInvertElevation());
	IDC_INVERT_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getInvertElevationEntry().getDescription(), wxConvUTF8));
	IDC_INVERTMOUSE_CTRL->SetValue(OptionsDisplay::instance()->getInvertMouse());
	IDC_INVERTMOUSE_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getInvertMouseEntry().getDescription(), wxConvUTF8));
	IDC_SMOUSE_CTRL->SetValue(OptionsDisplay::instance()->getSoftwareMouse());
	IDC_SMOUSE_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getSoftwareMouseEntry().getDescription(), wxConvUTF8));
	IDC_TOOLTIP_CTRL->SetValue(OptionsDisplay::instance()->getShowContextHelp());
	IDC_TOOLTIP_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getShowContextHelpEntry().getDescription(), wxConvUTF8));
	IDC_TIMER_CTRL->SetValue(OptionsDisplay::instance()->getFrameTimer());
	IDC_TIMER_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getFrameTimerEntry().getDescription(), wxConvUTF8));
	IDC_SIDESCROLL_CTRL->SetValue(OptionsDisplay::instance()->getSideScroll());
	IDC_SIDESCROLL_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getSideScrollEntry().getDescription(), wxConvUTF8));
	IDC_VALIDATESERVER_CTRL->SetValue(OptionsDisplay::instance()->getValidateServerIp());
	IDC_VALIDATESERVER_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getValidateServerIpEntry().getDescription(), wxConvUTF8));	
	IDC_SLIDER1_CTRL->SetRange(3, 40);
	IDC_SLIDER1_CTRL->SetValue(OptionsDisplay::instance()->getBrightness());
	IDC_SLIDER1_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getBrightnessEntry().getDescription(), wxConvUTF8));
	IDC_VOLUME_CTRL->SetRange(0, 128);
	IDC_VOLUME_CTRL->SetValue(OptionsDisplay::instance()->getSoundVolume());
	IDC_VOLUME_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getSoundVolumeEntry().getDescription(), wxConvUTF8));
	IDC_USERNAME_CTRL->SetValue(wxString(OptionsDisplay::instance()->getOnlineUserName(), wxConvUTF8));
	IDC_USERNAME_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getOnlineUserNameEntry().getDescription(), wxConvUTF8));
	IDC_TANKMODEL_CTRL->SetValue(wxString(OptionsDisplay::instance()->getOnlineTankModel(), wxConvUTF8));
	IDC_TANKMODEL_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getOnlineTankModelEntry().getDescription(), wxConvUTF8));
	IDC_HOSTDESC_CTRL->SetValue(wxString(OptionsDisplay::instance()->getHostDescription(), wxConvUTF8));
	IDC_HOSTDESC_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getHostDescriptionEntry().getDescription(), wxConvUTF8));
	IDC_NODETAILTEX_CTRL->SetValue(!OptionsDisplay::instance()->getDetailTexture());
	IDC_NODETAILTEX_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDetailTextureEntry().getDescription(), wxConvUTF8));
	IDC_NOBOIDS_CTRL->SetValue(OptionsDisplay::instance()->getNoBOIDS());
	IDC_NOBOIDS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoBOIDSEntry().getDescription(), wxConvUTF8));
	IDC_NOSHIPS_CTRL->SetValue(OptionsDisplay::instance()->getNoShips());
	IDC_NOSHIPS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getNoShipsEntry().getDescription(), wxConvUTF8));
	IDC_MORERES_CTRL->SetValue(OptionsDisplay::instance()->getMoreRes());
	IDC_MORERES_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getMoreResEntry().getDescription(), wxConvUTF8));
	//IDC_SWAPYAXIS_CTRL->SetValue(OptionsDisplay::instance()->getSwapYAxis());
	//IDC_SWAPYAXIS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getSwapYAxisEntry().getDescription(), wxConvUTF8));
	IDC_LOGGING_CTRL->SetValue(OptionsDisplay::instance()->getClientLogToFile());
	IDC_LOGGING_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getClientLogToFileEntry().getDescription(), wxConvUTF8));

	for (int i=2; i<=64; i+=2)
	{
		IDC_SOUNDCHANNELS_CTRL->Append(
			wxString(formatString("%i", i), wxConvUTF8));
	}
	IDC_SOUNDCHANNELS_CTRL->SetValue(
		wxString(formatString("%i", OptionsDisplay::instance()->getSoundChannels()),
			wxConvUTF8));

	refreshResolutions();

	IDC_TINYDIALOGS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDialogSizeEntry().getDescription(), wxConvUTF8));
	IDC_SMALLDIALOGS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDialogSizeEntry().getDescription(), wxConvUTF8));
	IDC_MEDIUMDIALOGS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDialogSizeEntry().getDescription(), wxConvUTF8));
	IDC_LARGEDIALOGS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDialogSizeEntry().getDescription(), wxConvUTF8));
	IDC_HUGEDIALOGS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getDialogSizeEntry().getDescription(), wxConvUTF8));
	switch (OptionsDisplay::instance()->getDialogSize())
	{
	case 0:
		IDC_TINYDIALOGS_CTRL->SetValue(true);
		break;
	case 1:
		IDC_SMALLDIALOGS_CTRL->SetValue(true);
		break;
	case 2:
		IDC_MEDIUMDIALOGS_CTRL->SetValue(true);
		break;
	case 3:
		IDC_LARGEDIALOGS_CTRL->SetValue(true);
		break;
	case 4:
		IDC_HUGEDIALOGS_CTRL->SetValue(true);
		break;
	}

	IDC_SMALLTEX_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getTexSizeEntry().getDescription(), wxConvUTF8));
	IDC_MEDIUMTEX_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getTexSizeEntry().getDescription(), wxConvUTF8));
	IDC_LARGETEX_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getTexSizeEntry().getDescription(), wxConvUTF8));
	switch (OptionsDisplay::instance()->getTexSize())
	{
	case 0:
		IDC_SMALLTEX_CTRL->SetValue(true);
		break;
	case 1:
		IDC_MEDIUMTEX_CTRL->SetValue(true);
		break;
	default:
		IDC_LARGETEX_CTRL->SetValue(true);
		break;
	};

	IDC_LOWEFFECTS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getEffectsDetailEntry().getDescription(), wxConvUTF8));
	IDC_MEDIUMEFFECTS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getEffectsDetailEntry().getDescription(), wxConvUTF8));
	IDC_HIGHEFFECTS_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getEffectsDetailEntry().getDescription(), wxConvUTF8));
	switch (OptionsDisplay::instance()->getEffectsDetail())
	{
	case 0:
		IDC_LOWEFFECTS_CTRL->SetValue(true);
		break;
	case 1:
		IDC_MEDIUMEFFECTS_CTRL->SetValue(true);
		break;
	default:
		IDC_HIGHEFFECTS_CTRL->SetValue(true);
		break;
	};

	IDC_LOWTANK_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getTankDetailEntry().getDescription(), wxConvUTF8));
	IDC_MEDIUMTANK_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getTankDetailEntry().getDescription(), wxConvUTF8));
	IDC_HIGHTANK_CTRL->SetToolTip(wxString(OptionsDisplay::instance()->getTankDetailEntry().getDescription(), wxConvUTF8));
	switch (OptionsDisplay::instance()->getTankDetail())
	{
	case 0:
		IDC_LOWTANK_CTRL->SetValue(true);
		break;
	case 1:
		IDC_MEDIUMTANK_CTRL->SetValue(true);
		break;
	default:
		IDC_HIGHTANK_CTRL->SetValue(true);
		break;
	};

	refreshKeysControls();

	IDOK_CTRL->SetDefault();
}

void DisplayFrame::refreshResolutions()
{
	IDC_DISPLAY_CTRL->Clear();

	std::set<std::string> displaySet;
	char string[256];
	SDL_Rect **modes = SDL_ListModes(NULL,SDL_FULLSCREEN|SDL_HWSURFACE);
	if((modes != (SDL_Rect **)0) && (modes != (SDL_Rect **)-1))
	{
		for(int i=0;modes[i];++i)
		{
			snprintf(string, 256, "%i x %i", 
				modes[i]->w, modes[i]->h);

			std::string newDisplay(string);
			if (displaySet.find(newDisplay) == displaySet.end())
			{
				IDC_DISPLAY_CTRL->Append(wxString(string, wxConvUTF8));
				displaySet.insert(newDisplay);
			}
		}
	}

	if (IDC_MORERES_CTRL->GetValue())
	{
		const char *extraModes[] = 
			{ "320 x 200", "320 x 240", "512 x 384", 
			"640 x 480", "800 x 600", "1024 x 768", "1024 x 384" };
		for (int i=0; i<sizeof(extraModes)/sizeof(const char *); i++)
		{
			std::string newDisplay(extraModes[i]);
			if (displaySet.find(newDisplay) == displaySet.end())
			{
				IDC_DISPLAY_CTRL->Append(wxString(newDisplay.c_str(), wxConvUTF8));
				displaySet.insert(newDisplay);
			}
		}
	}

	snprintf(string, 256, "%i x %i", 
		OptionsDisplay::instance()->getScreenWidth(),
		OptionsDisplay::instance()->getScreenHeight());
	IDC_DISPLAY_CTRL->SetValue(wxString(string, wxConvUTF8));
}

bool DisplayFrame::TransferDataFromWindow()
{
	OptionsDisplay::instance()->getFullClearEntry().setValue(IDC_FULLCLEAR_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLTexSubImageEntry().setValue(IDC_NOLANDSCAPESCORCH_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLExtEntry().setValue(IDC_NOEXT_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLMultiTexEntry().setValue(IDC_NOMULTITEX_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLCompiledArraysEntry().setValue(IDC_NOCOMPILEDARRAYS_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLEnvCombineEntry().setValue(IDC_NOENVCOMBINE_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLCubeMapEntry().setValue(IDC_NOCUBEMAP_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLSphereMapEntry().setValue(IDC_NOSPHEREMAP_CTRL->GetValue());
	OptionsDisplay::instance()->getNoVBOEntry().setValue(IDC_NOVBO_CTRL->GetValue());
	OptionsDisplay::instance()->getNoGLHardwareMipmapsEntry().setValue(IDC_NOMIPMAPS_CTRL->GetValue());
	OptionsDisplay::instance()->getNoSoundEntry().setValue(IDC_NOSOUND_CTRL->GetValue());
	OptionsDisplay::instance()->getNoAmbientSoundEntry().setValue(IDC_NOAMBIENTSOUND_CTRL->GetValue());
	OptionsDisplay::instance()->getNoBoidSoundEntry().setValue(IDC_NOBOIDSOUND_CTRL->GetValue());
	OptionsDisplay::instance()->getSoundChannelsEntry().setValue(atoi(IDC_SOUNDCHANNELS_CTRL->GetValue().mb_str(wxConvUTF8)));
	OptionsDisplay::instance()->getNoSkinsEntry().setValue(IDC_NOSKINS_CTRL->GetValue());
	OptionsDisplay::instance()->getNoModelLightingEntry().setValue(IDC_NODYNAMICLIGHT_CTRL->GetValue());
	OptionsDisplay::instance()->getNoTreesEntry().setValue(IDC_NOTREES_CTRL->GetValue());
	OptionsDisplay::instance()->getNoPrecipitationEntry().setValue(IDC_NOPRECIPITATION_CTRL->GetValue());
	OptionsDisplay::instance()->getNoWavesEntry().setValue(IDC_NOWAVES_CTRL->GetValue());
	OptionsDisplay::instance()->getNoDepthSortingEntry().setValue(IDC_NODEPTHSORT_CTRL->GetValue());
	OptionsDisplay::instance()->getFullScreenEntry().setValue(IDC_FULLSCREEN_CTRL->GetValue());
	OptionsDisplay::instance()->getNoSkyLayersEntry().setValue(IDC_SINGLESKYLAYER_CTRL->GetValue());
	OptionsDisplay::instance()->getNoSkyMovementEntry().setValue(IDC_NOSKYANI_CTRL->GetValue());
	OptionsDisplay::instance()->getNoWaterMovementEntry().setValue(IDC_NOWATERANI_CTRL->GetValue());
	OptionsDisplay::instance()->getBrightnessEntry().setValue(IDC_SLIDER1_CTRL->GetValue());
	OptionsDisplay::instance()->getSoundVolumeEntry().setValue(IDC_VOLUME_CTRL->GetValue());
	OptionsDisplay::instance()->getDrawWaterEntry().setValue(!IDC_NOWATER_CTRL->GetValue());
	OptionsDisplay::instance()->getDrawSurroundEntry().setValue(!IDC_NOSURROUND_CTRL->GetValue());
	OptionsDisplay::instance()->getInvertElevationEntry().setValue(IDC_INVERT_CTRL->GetValue());
	OptionsDisplay::instance()->getInvertMouseEntry().setValue(IDC_INVERTMOUSE_CTRL->GetValue());
	OptionsDisplay::instance()->getSoftwareMouseEntry().setValue(IDC_SMOUSE_CTRL->GetValue());
	OptionsDisplay::instance()->getShowContextHelpEntry().setValue(IDC_TOOLTIP_CTRL->GetValue());
	OptionsDisplay::instance()->getFrameTimerEntry().setValue(IDC_TIMER_CTRL->GetValue());
	OptionsDisplay::instance()->getSideScrollEntry().setValue(IDC_SIDESCROLL_CTRL->GetValue());
	OptionsDisplay::instance()->getValidateServerIpEntry().setValue(IDC_VALIDATESERVER_CTRL->GetValue());
	OptionsDisplay::instance()->getOnlineUserNameEntry().setValue(IDC_USERNAME_CTRL->GetValue().mb_str(wxConvUTF8));
	OptionsDisplay::instance()->getOnlineTankModelEntry().setValue(IDC_TANKMODEL_CTRL->GetValue().mb_str(wxConvUTF8));
	OptionsDisplay::instance()->getHostDescriptionEntry().setValue(IDC_HOSTDESC_CTRL->GetValue().mb_str(wxConvUTF8));
	OptionsDisplay::instance()->getDetailTextureEntry().setValue(!IDC_NODETAILTEX_CTRL->GetValue());
	OptionsDisplay::instance()->getNoBOIDSEntry().setValue(IDC_NOBOIDS_CTRL->GetValue());
	OptionsDisplay::instance()->getNoShipsEntry().setValue(IDC_NOSHIPS_CTRL->GetValue());
	OptionsDisplay::instance()->getMoreResEntry().setValue(IDC_MORERES_CTRL->GetValue());
	//OptionsDisplay::instance()->getSwapYAxisEntry().setValue(IDC_SWAPYAXIS_CTRL->GetValue());
	OptionsDisplay::instance()->getClientLogToFileEntry().setValue(IDC_LOGGING_CTRL->GetValue());

	wxString buffer = IDC_DISPLAY_CTRL->GetValue();
	int windowWidth, windowHeight;
	if (sscanf(buffer.mb_str(wxConvUTF8), 
		"%i x %i", 
		&windowWidth,
		&windowHeight) == 2)
	{
		OptionsDisplay::instance()->getScreenWidthEntry().setValue(windowWidth);
		OptionsDisplay::instance()->getScreenHeightEntry().setValue(windowHeight);
	}

	int dialogSize = 0;
	if (IDC_SMALLDIALOGS_CTRL->GetValue()) dialogSize = 1;
	if (IDC_MEDIUMDIALOGS_CTRL->GetValue()) dialogSize = 2;
	if (IDC_LARGEDIALOGS_CTRL->GetValue()) dialogSize = 3;
	if (IDC_HUGEDIALOGS_CTRL->GetValue()) dialogSize = 4;
	OptionsDisplay::instance()->getDialogSizeEntry().setValue(dialogSize);

	int texSize = 1;
	if (IDC_SMALLTEX_CTRL->GetValue()) texSize = 0;
	if (IDC_LARGETEX_CTRL->GetValue()) texSize = 2;
	OptionsDisplay::instance()->getTexSizeEntry().setValue(texSize);

	int effectsDetail = 1;
	if (IDC_LOWEFFECTS_CTRL->GetValue()) effectsDetail = 0;
	if (IDC_HIGHEFFECTS_CTRL->GetValue()) effectsDetail = 2;
	OptionsDisplay::instance()->getEffectsDetailEntry().setValue(effectsDetail);

	int tankDetail = 1;
	if (IDC_LOWTANK_CTRL->GetValue()) tankDetail = 0;
	if (IDC_HIGHTANK_CTRL->GetValue()) tankDetail = 2;
	OptionsDisplay::instance()->getTankDetailEntry().setValue(tankDetail);

	// Save display options to file
	OptionsDisplay::instance()->writeOptionsToFile();

	// Save keyboard keys to file
	Keyboard::instance()->saveKeyFile();

	return true;
}

void DisplayFrame::onExportMod(wxCommandEvent &event)
{
	int selectionNo = modbox->GetSelection();
	if (selectionNo < 0) return;
	wxString selection = modbox->GetString(selectionNo);
	if (selection.empty()) return;

	wxString file = ::wxFileSelector(wxT("Please choose the export file to save"),
		wxString(getSettingsFile(""), wxConvUTF8), // default path
		wxString(formatString("%s.s3m", (const char *) selection.mb_str(wxConvUTF8)), wxConvUTF8), // default filename
		wxT(""), // default extension
		wxT("*.s3m"),
		wxSAVE);
	if (file.empty()) return;
	ModFiles files;
	if (!files.loadModFiles(selection.mb_str(wxConvUTF8), false))
	{
		dialogMessage("Export Mod", "Failed to load mod files");
		return;
	}
	if (!files.exportModFiles(
		selection.mb_str(wxConvUTF8), 
		file.mb_str(wxConvUTF8)))
	{
		dialogMessage("Export Mod", "Failed to write mod export file");
		return;
	}
}

void DisplayFrame::onImportMod(wxCommandEvent &event)
{
	wxString file = ::wxFileSelector(wxT("Please choose the import file to open"),
		wxString(getSettingsFile(""),wxConvUTF8), // default path
		wxT(""), // default filename
		wxT(""), // default extension
		wxT("*.s3m"),
		wxOPEN | wxFILE_MUST_EXIST);
	if (file.empty()) return;
	ModFiles files;
	const char *mod = 0;
	if (!files.importModFiles(&mod, file.mb_str(wxConvUTF8)))
	{
		dialogMessage("Import Mod", "Failed to read mod export file");
		return;
	}
	if (!files.writeModFiles(mod))
	{
		dialogMessage("Import Mod", "Failed to write mod files");
		return;
	}
	updateModList();
}

void showDisplayDialog()
{
	DisplayFrame frame;
	frame.ShowModal();
}