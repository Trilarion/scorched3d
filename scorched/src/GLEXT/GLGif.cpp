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

#include <stdio.h>
#include <math.h>
#include <common/Defines.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLGif.h>
#include <wx/utils.h>
#include <wx/gifdecod.h>
#include <wx/wfstream.h>
#include <wx/mstream.h>

GLGif::GLGif() :
	width_(0), height_(0), bits_(0)
{

}

GLGif::GLGif(const char * filename) :
	bits_(0), width_(0), height_(0)
{
	loadFromFile(filename);
}

GLGif::~GLGif()
{
	delete [] bits_;
}

bool GLGif::loadFromFile(const char * filename)
{
	wxFileInputStream ifStream(filename);
	if (!ifStream.Ok()) return false;
	return loadFromStream(&ifStream);
}

bool GLGif::loadFromBuffer(NetBuffer &buffer)
{
	wxMemoryInputStream memStream(buffer.getBuffer(),
		buffer.getBufferUsed());
	return loadFromStream(&memStream);
}

bool GLGif::loadFromStream(wxInputStream *ifStream)
{
	wxGIFDecoder decoder(ifStream);
	if (decoder.ReadGIF() != wxGIF_OK) return false;

	bits_ = new unsigned char[decoder.GetWidth() * decoder.GetHeight() * 3];
	width_ = (int) decoder.GetWidth();
	height_ = (int) decoder.GetHeight();
	unsigned char* pal = decoder.GetPalette();
	unsigned char* src = decoder.GetData() + 
		(decoder.GetHeight() * decoder.GetWidth()) - decoder.GetWidth();
	unsigned char* dst = bits_;

    for (unsigned int y = 0; 
		y < decoder.GetHeight();
		y++, src-=decoder.GetWidth()*2)
    {
		for (unsigned int x = 0;
			x < decoder.GetWidth();
			x++, src++)
		{
			DIALOG_ASSERT(src >= decoder.GetData() &&
				src < decoder.GetData() + decoder.GetWidth() * decoder.GetHeight());

			*(dst++) = pal[3 * (*src) + 0];
			*(dst++) = pal[3 * (*src) + 1];
			*(dst++) = pal[3 * (*src) + 2];
		}
    }

	return true;
}