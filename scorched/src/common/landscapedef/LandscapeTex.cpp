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

#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDescriptions.h>
#include <engine/ScorchedContext.h>
#include <weapons/AccessoryStore.h>
#include <common/Defines.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

LandscapeTex::LandscapeTex(const char *name, const char *description, bool required) : 
	LandscapeInclude(name, description, required),
	waterHeight("The height of the landscape water", 0, -10)
{
	addChildXMLEntry("waterheight", &waterHeight);
}

LandscapeTex::~LandscapeTex()
{
}

LandscapeTextureFile::LandscapeTextureFile() :
		XMLEntryRoot<LandscapeTex>(S3D::eModLocation, "user_defined", 
			"tex",
			"LandscapeTexure", 
			"A landscape/scene definition, usualy related to the visual aspects of the landscape", true)
{
}

LandscapeTextureFile::~LandscapeTextureFile()
{
}
