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


#include <3dsparse/ASEStore.h>
#include <3dsparse/ASEFile.h>
#include <3dsparse/ASEArrayFact.h>

ASEStore *ASEStore::instance_ = 0;

ASEStore *ASEStore::instance()
{
	if (!instance_)
	{
		instance_ = new ASEStore;
	}
	return instance_;
}

ASEStore::ASEStore()
{
}

ASEStore::~ASEStore()
{
}


GLVertexArray *ASEStore::loadOrGetArray(const char *fileName)
{
	std::map<const char *, GLVertexArray *>::iterator findItor =
		fileMap_.find(fileName);
	if (findItor == fileMap_.end())
	{
		ASEFile file(fileName);
		if (file.getSuccess())
		{
			file.centre();
			GLVertexArray *array = 
				ASEArrayFact::getArray(file.getModels(), 1.0f);
			fileMap_[fileName] = array;
			return array;
		}
	}
	else
	{
		return (*findItor).second;
	}
	return 0;
}
