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

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

static ARGParser aParser;
char scorched3dAppName[128];

// Compilers from Borland report floating-point exceptions in a manner 
// that is incompatible with Microsoft Direct3D.
int _matherr(struct _exception  *e)
{
    e;               // Dummy reference to catch the warning.
    return 1;        // Error has been handled.
}

void _no_storage()
{
	printf("Failed to allocate memory!!");
	std::exit(1);
}

void run_main(int argc, char *argv[], OptionsParameters &params)
{
	std::set_new_handler(&_no_storage);

	// Set the path the executable was run with
	setExeName((const char *) argv[0]);

	// Generate the version
	snprintf(scorched3dAppName, 128, "Scorched3D - Version %s (%s)", 
		ScorchedVersion, ScorchedProtocolVersion);

	srand((unsigned)time(0));

	// Parse command line
	// Read options from command line
	if (!OptionEntryHelper::addToArgParser(
		params.getOptions(), aParser)) exit(64);
	if (!aParser.parse(argc, argv)) exit(64);
	setSettingsDir(params.getSettingsDir());

	// Check we are in the correct directory
	FILE *checkfile = fopen(getDataFile("data/autoexec.xml"), "r");
	if (!checkfile)
	{
		// Perhaps we can get the directory from the executables path name
		char path[1024];
		snprintf(path, sizeof(path), "%s", argv[0]);
		char *sep = strrchr(path, '/');
		if (sep)
		{
			// Change into this new direcotry
			*sep = '\0';
#ifdef _WIN32
			SetCurrentDirectory(path);
#else
			chdir(path);
#endif // _WIN32
		}

		// Now try again for the correct directory
		checkfile = fopen(getDataFile("data/autoexec.xml"), "r");
		if (!checkfile)
		{	
#ifdef _WIN32
			GetCurrentDirectory(sizeof(path), path);
#else
			getcwd(path, sizeof(path));
#endif // _WIN32
			dialogExit(
				scorched3dAppName, formatString(
				"Error: This game requires the Scorched3D data directory to run.\n"
				"Your machine does not appear to have the Scorched3D data directory in\n"
				"the required location.\n"
				"The data directory is set to \"%s\" which does not exist.\n"
				"(Current working directory %s)\n\n"
				"If Scorched3D does not run please re-install Scorched3D.",
				getDataFile("data"), path));
		}
	}
	else fclose(checkfile);

	// Check that the mods are uptodate with the current scorched3d
	// version
	ModDirs dirs;
	dirs.loadModDirs();

#ifndef _WIN32
	// Tells Linux not to issue a sig pipe when writting to a closed socket
	// Why does it have to be dificult!
	signal(SIGPIPE, SIG_IGN);
	signal(SIGFPE, SIG_IGN);
#endif

	if (setlocale(LC_ALL, "C") == 0)
	{
		dialogMessage(
			scorched3dAppName,
			"Warning: Failed to set client locale");
	}
}