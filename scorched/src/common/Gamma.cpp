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


// Gamma.cpp: implementation of the Gamma class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <common/Gamma.h>
#include <common/Defines.h>
#include <math.h>
#include <SDL/SDL.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Gamma *Gamma::instance_ = 0;

Gamma *Gamma::instance()
{
	if (!instance_)
	{
		instance_ = new Gamma;
	}

	return instance_;
}

Gamma::Gamma() : changedGamma_(false)
{

}

Gamma::~Gamma()
{
	reset();
}

bool Gamma::set(float gamma)
{
	if (!changedGamma_)
	{
		if (!save()) return false;
		changedGamma_ = true;
	}

	for (int n=0; n<256; n++) 
	{
		float i = (float)n / 256.0f;
		float c = powf(i,1.0f/gamma);

		int value =  (int)(c*65535.0 + 0.5); 
		if ( value > 65535 ) { value = 65535; } 

		tmpSettings_.Red  [n] = (Uint16)value;
		tmpSettings_.Green[n] = (Uint16)value;
		tmpSettings_.Blue [n] = (Uint16)value;
	}

	if (SDL_SetGammaRamp(tmpSettings_.Red,tmpSettings_.Green,tmpSettings_.Blue) <0)
	{
		return false;
	}

	return true;
}

bool Gamma::save()
{
	if (SDL_GetGammaRamp(savedSettings_.Red,  savedSettings_.Green, savedSettings_.Blue) <0) 
		return false;
	return true;
}


void Gamma::reset()
{
	if (!changedGamma_) return;
	SDL_SetGammaRamp(savedSettings_.Red,  savedSettings_.Green, savedSettings_.Blue);
}
