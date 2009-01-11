////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#if !defined(AFX_ImageFactory_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)
#define AFX_ImageFactory_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_

#include <image/ImageHandle.h>
#include <string>

class ImageFactory  
{
public:
	static Image *loadImage(
		const std::string &filename, 
		const std::string &alphafilename = "", 
		bool invert = true);
	static Image *loadAlphaImage(
		const std::string &filename);

	static ImageHandle loadImageHandle(
		const std::string &filename, 
		const std::string &alphafilename = "", 
		bool invert = true);
	static ImageHandle loadAlphaImageHandle(
		const std::string &filename);

	static ImageHandle createBlank(int width, int height, 
		bool alpha = false, unsigned char fill = 255);

#ifndef S3D_SERVER
	static ImageHandle grabScreen();
#endif

private:
	ImageFactory();
};

#endif // !defined(AFX_ImageFactory_H__0EBAA0E7_3103_43A4_90C0_5708ECE6DB43__INCLUDED_)