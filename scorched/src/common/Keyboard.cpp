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

#include <common/Keyboard.h>
#include <common/Defines.h>
#include <XML/XMLFile.h>

bool Keyboard::dvorak_ = false;

Keyboard *Keyboard::instance_ = 0;

Keyboard *Keyboard::instance()
{
	if (!instance_)
	{
		instance_ = new Keyboard;
	}

	return instance_;
}

Keyboard::Keyboard() : keybHistCnt_(0)
{

}

Keyboard::~Keyboard()
{

}

bool Keyboard::init()
{
 	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(250, 100);
	
	return true;
}

char *Keyboard::getkeyboardbuffer(unsigned int &bufCnt)
{
 	bufCnt = SDLK_LAST;
 	return (char *) SDL_GetKeyState(NULL);
}
 
KeyboardHistory::HistoryElement *Keyboard::getkeyboardhistory(unsigned int &histCnt)
{
 	histCnt = keybHistCnt_;
 	keybHistCnt_=0;

 	return keybHist_;
}
 
void Keyboard::processKeyboardEvent(SDL_Event &event)
{
	// Check we have enough space in the array
	if (keybHistCnt_ >= MAX_KEYBDHIST) return;

	// Check we are adding the correct key type
	if (event.type != SDL_KEYDOWN) return;

	// Check we don't have an international character
	if ( (event.key.keysym.unicode & 0xFF80) == 0 )
	{
		char ch = event.key.keysym.unicode & 0x7F;

		KeyboardHistory::HistoryElement newElement =
		{
			event.key.keysym.sym,
			ch
		};

		// Add characater and symbol to history
		keybHist_[keybHistCnt_++] = newElement;
	}
}

bool &Keyboard::getDvorak()
{
	return dvorak_;
}

bool Keyboard::parseKeyFile(const char *fileName)
{
	// Load key definition file
	XMLFile file;
    if (!file.readFile(fileName))
	{
		dialogMessage("Keyboard", 
					  "Failed to parse \"%s\"\n%s", 
					  fileName,
					  file.getParserError());
		return false;
	}

	// Check file exists
	if (!file.getRootNode())
	{
		dialogMessage("Keyboard",
					  "Failed to find key definition file \"%s\"",
					  fileName);
		return false;		
	}

	// Itterate all of the keys in the file
    std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> &children = file.getRootNode()->getChildren();
    for (childrenItor = children.begin();
		 childrenItor != children.end();
		 childrenItor++)
    {
		// Parse the tank entry
        XMLNode *currentNode = (*childrenItor);
		if (strcmp(currentNode->getName(), "key"))
		{
			dialogMessage("Keyboard",
						  "Failed to find key node");
			return false;
		}

		// Get the name of the key
		XMLNode *nameNode = currentNode->getNamedChild("name");
		if (!nameNode)
		{
			dialogMessage("Keyboard",
						  "Failed to find name node");
			return false;
		}
		const char *keyName = nameNode->getContent();
		
		// Get the description for the key
		XMLNode *descNode = currentNode->getNamedChild("description");
		if (!descNode)
		{
			dialogMessage("Keyboard",
						  "Failed to find description node");
			return false;
		}
		const char *keyDesc = descNode->getContent();

		// Create the key
		KeyboardKey *newKey = new KeyboardKey(keyName, keyDesc);

		// Add all the key names
		std::list<std::string> keyNames;
		std::list<XMLNode *>::iterator keyItor;
		std::list<XMLNode *> &keys = currentNode->getChildren();
		for (keyItor = keys.begin();
			 keyItor != keys.end();
			 keyItor++)
		{
			XMLNode *currentKey = (*keyItor);
			if (strcmp(currentKey->getName(), "key")==0)
			{
				// Add a key
				keyNames.push_back(currentKey->getContent());

				// Check the key is not already in use
				std::map<std::string, KeyboardKey *, std::less<std::string> >::iterator useditor =
					usedKeyMap_.find(currentKey->getContent());
				if (useditor != usedKeyMap_.end())
				{
					KeyboardKey *usedKey = (*useditor).second;
					dialogMessage("Keyboard",
								  "Key \"%s\" defined for \"%s\" was already defined for \"%s\"",
								  currentKey->getContent(), keyName, usedKey->getName());
					return false;					
				}
				usedKeyMap_[currentKey->getContent()] = newKey;
			}
		}

		// Actually add the key
		if (!newKey->addKeys(keyNames)) return false;
		keyMap_[keyName] = newKey;
	}

	return true;
}

KeyboardKey *Keyboard::getKey(const char *name)
{
	static KeyboardKey defaultKey("None", "None");

	std::map<std::string, KeyboardKey *, std::less<std::string> >::iterator itor =
		keyMap_.find(name);
	if (itor != keyMap_.end()) return (*itor).second;

	dialogMessage("Keyboard",
				  "Failed to find key for key name \"%s\"",
				  name);	
	return &defaultKey;
}
