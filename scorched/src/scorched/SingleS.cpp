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

#include "SingleS-def.cpp"

static void createControls(wxWindow *parent,
						   wxSizer *sizer)
{
	wxStaticBox *clientnameBox = 
		new wxStaticBox(parent, -1, "Client Settings");
	wxStaticBoxSizer *clientnameSizer = 
		new wxStaticBoxSizer(clientnameBox, wxVERTICAL);
	wxFlexGridSizer *clientnameSizer2 = new wxFlexGridSizer(4, 2, 5, 5);

	IDC_CLIENT_PLAYERS_CTRL_TEXT = 
		new wxStaticText(parent, -1,
		"Number Players :");
	clientnameSizer2->Add(IDC_CLIENT_PLAYERS_CTRL_TEXT, 0, wxALIGN_CENTER);
	IDC_CLIENT_PLAYERS_CTRL = 
		new wxComboBox(parent, -1,
		"",
		wxDefaultPosition, wxSize((int) 100, (int) 124.5),
		0, 0, wxCB_READONLY);
	clientnameSizer2->Add(IDC_CLIENT_PLAYERS_CTRL, 0, wxALIGN_CENTER);

	clientnameSizer->Add(clientnameSizer2);
	sizer->Add(clientnameSizer, 0, wxALL, 5);
	
	wxStaticBox *advBox = 
		new wxStaticBox(parent, -1, "Advanced Settings");
	wxStaticBoxSizer *advSizer = 
		new wxStaticBoxSizer(advBox, wxVERTICAL);
	IDC_BUTTON_SETTINGS_CTRL = 
		new wxButton(parent, IDC_BUTTON_SETTINGS,
		"Edit Advanced Settings");
	advSizer->Add(IDC_BUTTON_SETTINGS_CTRL, 0, wxALL, 5);
	sizer->Add(advSizer, 0, wxGROW | wxALL, 5);

	// Ok and cancel boxes
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	IDOK_CTRL = new wxButton(parent, wxID_OK, "Start Client");
	IDCANCEL_CTRL = new wxButton(parent, wxID_CANCEL, "Cancel");
	buttonSizer->Add(IDCANCEL_CTRL, 0, wxALL, 10);
	buttonSizer->Add(IDOK_CTRL, 0, wxALL, 10);
	sizer->Add(buttonSizer, 0, wxALIGN_RIGHT);
}

