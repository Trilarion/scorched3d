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

#include <engine/ModFileEntry.h>
#include <engine/ModFiles.h>
#include <zlib/zlib.h>
#include <wx/dir.h>
#include <wx/utils.h>

ModFileEntry::ModFileEntry() : 
	compressedcrc_(0)
{
}

ModFileEntry::~ModFileEntry()
{
}

bool ModFileEntry::writeModFile(const char *fileName, const char *modName)
{
	// Check the downloaded CRC matches the actual crc of the file
	unsigned int crc =  crc32(0L, Z_NULL, 0);
	crc = crc32(crc, (unsigned char *) 
		compressedfile_.getBuffer(), compressedfile_.getBufferUsed());
	if (crc != compressedcrc_)
	{
		dialogMessage("WriteModFile",
			"File crc missmatch error");
		return false;
	}

	// Decompress the actual file contents	
	NetBuffer fileContents;
	if (compressedfile_.getBufferUsed() > 0)
	{
		unsigned long destLen = uncompressedSize_ + 10;
		fileContents.allocate(destLen);
		unsigned uncompressResult = 
			uncompress((unsigned char *) fileContents.getBuffer(), &destLen, 
			(unsigned char *) compressedfile_.getBuffer(), compressedfile_.getBufferUsed());
		fileContents.setBufferUsed(destLen);

		if (uncompressResult == Z_MEM_ERROR) dialogMessage(
			"WriteModFile", "Memory error");
		else if (uncompressResult == Z_DATA_ERROR) dialogMessage(
			"WriteModFile", "Data error");
		else if (uncompressResult == Z_BUF_ERROR) dialogMessage(
			"WriteModFile", "Buffer error");

		bool result = (Z_OK == uncompressResult);
		if (!result) return false;
	}
	else
	{
		fileContents.allocate(1);
		fileContents.setBufferUsed(0);
	}

	// Create any needed directories
	char *dir = (char *) fileName;
	while (dir = strchr(dir, '/'))
	{
		*dir = '\0';
		const char *needdir = getModFile("%s/%s", 
			modName, fileName);
		if (!::wxDirExists(needdir)) ::wxMkdir(needdir);
		*dir = '/';
		dir++;
	}

	// Write the file 
	const char *needfile = getModFile("%s/%s", 
		modName, fileName);
	FILE *file = fopen(needfile, "wb");
	if (!file)
	{
		dialogMessage("WriteModFile",
			"Create file error");
		return false;
	}
	if (fwrite(fileContents.getBuffer(), sizeof(unsigned char), 
		fileContents.getBufferUsed(), file) != fileContents.getBufferUsed())
	{
		dialogMessage("WriteModFile",
			"Write file error");
		fclose(file);
		return false;
	}
	fclose(file);

	return true;
}

bool ModFileEntry::loadModFile(const char *filename)
{
	static NetBuffer fileContents;
	fileContents.reset();
	{
		// Do translation on ASCII files to prevent CVS
		// from being a biatch
		bool translate = false;
		if (strstr(filename, ".txt") != 0 ||
			strstr(filename, ".xml") != 0)
		{
			translate = true;
		}

		// Load the file into a coms buffer
		FILE *file = fopen(filename, "rb");
		if (!file) return false;
		int newSize = 0;
		unsigned char buffer[256];
		static NetBuffer tmpBuffer;
		tmpBuffer.reset();
		do
		{
			newSize = fread(buffer, 
				sizeof(unsigned char), 
				sizeof(buffer), 
				file);
			tmpBuffer.addDataToBuffer(buffer, newSize);
		}
		while (newSize > 0);
		fclose(file);

		for (unsigned i=0; i<tmpBuffer.getBufferUsed(); i++)
		{
			if (!translate ||
				i >= tmpBuffer.getBufferUsed() - 1 ||
				tmpBuffer.getBuffer()[i] != 13 ||
				tmpBuffer.getBuffer()[i+1] != 10)
			{	
				fileContents.addDataToBuffer(
					&tmpBuffer.getBuffer()[i] , 1);
			}
		}
	}

	uncompressedSize_ = fileContents.getBufferUsed();
	if (fileContents.getBufferUsed() > 0)
	{
		// Allocate the needed space for the compressed file
		unsigned long destLen = fileContents.getBufferUsed() + 100;
		compressedfile_.allocate(destLen);
		compressedfile_.reset();

		// Compress the file into the new buffer
		if (compress2(
				(unsigned char *) compressedfile_.getBuffer(), &destLen, 
				(unsigned char *) fileContents.getBuffer(), fileContents.getBufferUsed(), 
				6) != Z_OK)
		{
			return false;
		}
		compressedfile_.setBufferUsed(destLen);

		// Get the crc for the new file
		compressedcrc_ =  crc32(0L, Z_NULL, 0);
		compressedcrc_ = crc32(compressedcrc_, 
			(unsigned char *) compressedfile_.getBuffer(), compressedfile_.getBufferUsed());
	}
	else
	{
		compressedfile_.allocate(1);
		compressedfile_.setBufferUsed(0);
	}

	return true;
}