////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#if !defined(__INCLUDE_GLWImageListh_INCLUDE__)
#define __INCLUDE_GLWImageListh_INCLUDE__

#include <GLW/GLWButton.h>
#include <GLW/GLWSelector.h>
#include <GLEXT/GLTexture.h>
#include <list>

class GLWImageList : 
	public GLWidget,
	public GLWSelectorI
{
public:
	GLWImageList(float x = 0.0f, float y = 0.0f, 
		const char *directory = 0);
	virtual ~GLWImageList();

	const char *getCurrent();
	bool setCurrent(const char *current);

	// GLWidget
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(float x, float y, bool &skipRest);
	virtual void mouseUp(float x, float y, bool &skipRest);
	virtual void mouseDrag(float mx, float my, float x, float y, bool &skipRest);

	// GLWSelectorI
	virtual void itemSelected(GLWSelectorEntry *entry, int position);

	REGISTER_CLASS_HEADER(GLWImageList);

protected:
	class GLWImageListEntry 
	{
	public:
		std::string shortFileName;
		GLTexture texture;
	};
	std::list<GLWImageListEntry*> entries_;
	GLWImageListEntry *current_;
		
};

#endif // __INCLUDE_GLWImageListh_INCLUDE__