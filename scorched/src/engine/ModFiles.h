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

#if !defined(__INCLUDE_ModFilesh_INCLUDE__)
#define __INCLUDE_ModFilesh_INCLUDE__

#include <engine/ModFileEntry.h>
#include <common/ProgressCounter.h>
#include <map>

class ModFiles
{
public:
	ModFiles();
	virtual ~ModFiles();

	bool exportModFiles(const char *mod, const char *fileName);
	bool importModFiles(const char **mod, const char *fileName);
	bool importModFiles(const char **mod, NetBuffer &buffer);

	bool loadModFiles(const char *mod, bool createDir, 
		ProgressCounter *counter = 0);
	bool writeModFiles(const char *mod);
	void clearData();

	std::map<std::string, ModFileEntry *> &getFiles() { return files_; }
	static bool excludeFile(const char *file);
	static bool fileEnding(const char *file, const char *ext);

protected:
	std::map<std::string, ModFileEntry *> files_;

	bool loadModDir(const char *moddir, const char *mod,
		ProgressCounter *counter = 0);
	bool loadModFile(const char *fullFileName,
		const char *modDir, const char *mod);
	bool loadLocalModFile(const char *local, 
		const char *mod);

private:
	ModFiles(const ModFiles&other);
	ModFiles &operator=(ModFiles &other);

};

#endif

