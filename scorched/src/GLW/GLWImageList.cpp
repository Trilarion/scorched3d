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

#include <GLW/GLWImageList.h>
#include <GLW/GLWTranslate.h>
#include <GLEXT/GLGif.h>
#include <GLEXT/GLState.h>
#include <common/FileList.h>
#include <string.h>

REGISTER_CLASS_SOURCE(GLWImageList);

GLWImageList::GLWImageList(float x, float y, const char *directory) :
	GLWidget(x, y, 32.0f, 32.0f), current_(0)
{
	if (!directory) return;

	FileList filelist((char *) directory, "*.gif");
	std::list<std::string>::iterator itor;
	for (itor = filelist.getFiles().begin();
		itor != filelist.getFiles().end();
		itor++)
	{
		const char *filename = (*itor).c_str();
		GLGif gif;
		if (gif.loadFromFile(filename) &&
			gif.getWidth() == 32 &&
			gif.getHeight() == 32)
		{
			GLWImageListEntry *entry = new GLWImageListEntry;
			if (entry->texture.create(gif))
			{
				const char *sep = strrchr(filename, '/');
				if (sep) sep++;
				else sep = filename;
				entry->shortFileName = sep;
				entries_.push_back(entry);
			}
			else delete entry;
		}
	}

	setCurrent("");
}

GLWImageList::~GLWImageList()
{
	while (!entries_.empty())
	{
		GLWImageListEntry* entry = entries_.front();
		entries_.pop_front();
		delete entry;
	}
}

void GLWImageList::draw()
{
	GLWidget::draw();

	if (!current_) return;
	current_->texture.draw();

	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_T, GL_CLAMP);

	GLState currentStateOff(GLState::TEXTURE_OFF);
	glColor3f(0.4f, 0.4f, 0.6f);
	glBegin(GL_LINE_LOOP);
		glVertex2f(x_ - 4.0f, y_ - 3.0f);
		glVertex2f(x_ + w_ + 3.0f, y_ - 3.0f);
		glVertex2f(x_ + w_ + 3.0f, y_ + h_ + 3.0f);
		glVertex2f(x_ - 4.0f, y_ + h_ + 3.0f);
	glEnd();

	GLState currentStateOn(GLState::TEXTURE_ON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(x_, y_);
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(x_ + w_, y_);
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(x_ + w_, y_ + h_);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(x_, y_ + h_);
	glEnd();
}

bool GLWImageList::setCurrent(const char *current)
{
	std::list<GLWImageListEntry*>::iterator itor;
	for (itor = entries_.begin();
		itor != entries_.end();
		itor++)
	{
		GLWImageListEntry *entry = (*itor);
		if (0 == strcmp(entry->shortFileName.c_str(), current))
		{
			current_ = entry;
			return true;
		}
	}

	if (!current_ && !entries_.empty())
	{
		current_ = entries_.front();
	}
	return false;
}

const char *GLWImageList::getCurrent()
{
	if (current_) return current_->shortFileName.c_str();
	return "";
}

void GLWImageList::simulate(float frameTime)
{
	GLWidget::simulate(frameTime);
}

void GLWImageList::mouseDown(float x, float y, 
							 bool &skipRest)
{
	GLWidget::mouseDown(x, y, skipRest);

	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;
		std::list<GLWSelectorEntry> entries;
		std::list<GLWImageListEntry*>::iterator itor;
		for (itor = entries_.begin();
			itor != entries_.end();
			itor++)
		{
			GLWImageListEntry *entry = (*itor);
			entries.push_back(
				GLWSelectorEntry(entry->shortFileName.c_str(), 
				0, 0, &entry->texture));
		}

		GLWSelector::instance()->showSelector(
			this, 
			GLWTranslate::getPosX() + x, 
			GLWTranslate::getPosY() + y, 
			entries, 0, false);
	}
}

void GLWImageList::mouseUp(float x, float y, 
						   bool &skipRest)
{
	GLWidget::mouseUp(x, y, skipRest);
}

void GLWImageList::mouseDrag(float mx, float my, 
							 float x, float y, bool &skipRest)
{
	GLWidget::mouseDrag(mx, my, x, y, skipRest);
}

void GLWImageList::itemSelected(GLWSelectorEntry *entry, int position)
{
	setCurrent(entry->getText());
}
