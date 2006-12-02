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

#include <server/ScorchedServer.h>
#include <server/ServerMain.h>
#include <server/ServerParams.h>
#include <engine/ModDirs.h>
#include <common/OptionsGame.h>
#include <common/ARGParser.h>
#include <common/Defines.h>
#include <common/OptionsTransient.h>
#include <locale.h>
#include <math.h>
#include <signal.h>
#include <float.h>
#include <time.h>
#include <common/main.h>

int main(int argc, char *argv[])
{
	// From main.h
	run_main(argc, argv, *ServerParams::instance());

	// Start SDL
	unsigned int initFlags = 0;
	if (ServerParams::instance()->getAllowExceptions()) initFlags |= SDL_INIT_NOPARACHUTE;
	if (SDL_Init(initFlags) < 0)
	{
		dialogExit(
			scorched3dAppName,
			"The SDL library failed to initialize.");
	}

	// Load the server settings file
	if (!ServerParams::instance()->getServerFile()[0])
	{
		aParser.showArgs();
		exit(64);
	}
	if (!s3d_fileExists(ServerParams::instance()->getServerFile()))
	{
		dialogExit(scorched3dAppName, formatString(
			"Server file \"%s\" does not exist.",
			ServerParams::instance()->getServerFile()));
	}
	ScorchedServer::instance()->getOptionsGame().readOptionsFromFile(
		(char *) ServerParams::instance()->getServerFile());

	// Run Server
	consoleServer();

	SDL_Quit();
	return 0; // exit(0)
}