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


// WARNING: This was autogenerated from a windows .rc file!!
// Do not edit directly

#include "ServerS-def.cpp"

static void createControls(wxWindow *parent)
{
	IDC_SERVER_NAME_CTRL = 
		new wxTextCtrl(parent, IDC_SERVER_NAME,
		"",
		wxPoint((int) 111, (int) 25.5), wxSize((int) 226.5, (int) 21));
	IDC_SERVER_PORT_CTRL = 
		new wxTextCtrl(parent, IDC_SERVER_PORT,
		"",
		wxPoint((int) 111, (int) 49.5), wxSize((int) 72, (int) 21));
	IDC_SERVER_PASSWORD_CTRL = 
		new wxTextCtrl(parent, IDC_SERVER_PASSWORD,
		"",
		wxPoint((int) 111, (int) 73.5), wxSize((int) 226.5, (int) 21));
	new wxStaticText(parent, -1,
		"Port Number :",
		wxPoint((int) 19.5, (int) 54));
	new wxStaticBox(parent, -1,
		"Server Name",
		wxPoint((int) 10.5, (int) 10.5), wxSize((int) 339, (int) 147));
	new wxStaticText(parent, -1,
		"Server Name :",
		wxPoint((int) 19.5, (int) 30));
	new wxStaticText(parent, -1,
		"Password :",
		wxPoint((int) 19.5, (int) 78));
	IDC_PUBLISH_CTRL = 
		new wxCheckBox(parent, IDC_PUBLISH,
		"Allow ALL other internet users to see and use this server",
		wxPoint((int) 21, (int) 100));
	IDC_PUBLISHIP_CTRL = 
		new wxTextCtrl(parent, IDC_PUBLISHIP,
		"",
		wxPoint((int) 111, (int) 126), wxSize((int) 228, (int) 21));
	IDC_PUBLISHIP_CTRL_TEXT = new wxStaticText(parent, -1,
		"Published IP :",
		wxPoint((int) 21, (int) 126), wxSize((int) 84, (int) 12));
	IDOK_CTRL = 
		new wxButton(parent, wxID_OK,
		"Start Server",
		wxPoint((int) 273, (int) 226.5), wxSize((int) 75, (int) 21));
	IDC_BUTTON_SETTINGS_CTRL = 
		new wxButton(parent, IDC_BUTTON_SETTINGS,
		"Edit Advanced Settings",
		wxPoint((int) 19.5, (int) 180), wxSize((int) 150, (int) 21));
	IDCANCEL_CTRL = 
		new wxButton(parent, wxID_CANCEL,
		"Cancel",
		wxPoint((int) 189, (int) 226.5), wxSize((int) 75, (int) 21));
	new wxStaticBox(parent, -1,
		"Advanced Settings",
		wxPoint((int) 10.5, (int) 158), wxSize((int) 339, (int) 58));
}

