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


#ifndef _DISPLAY_H_
#define _DISPLAY_H_

// Display.h: interface for the Display class.
//
//////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <SDL/SDL.h>

class Display  
{
public:
	static Display *instance();

	bool changeSettings(int width,int height, bool fullscreen);
	void autoSettings(int &width, int &height, bool &full);

protected:
	static Display *instance_;
			
private:
	Display();
	virtual ~Display();

	bool init_;
	int videoFlags;
	const SDL_VideoInfo *videoInfo;
	SDL_Surface *surface;

};

#endif /* _DISPLAY_H_ */
