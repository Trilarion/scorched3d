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

#include <GLEXT/GLConsoleLines.h>

unsigned GLConsoleLine::nextLineNumber_ = 0;

GLConsoleLine::GLConsoleLine() :
	showPointer_(false), lineNumber_(0)
{

}

GLConsoleLine::~GLConsoleLine()
{

}

void GLConsoleLine::set(const char *line, bool showPointer)
{
	line_ = std::string(line);
	showPointer_ = showPointer;
	if (showPointer_)
	{
		lineNumber_ = ++nextLineNumber_;
	}
}

void GLConsoleLine::drawLine(float x, float y, GLFont2d *font)
{
	static Vector color(0.9f, 0.9f, 0.9f);
	if (showPointer_)
	{
		// We show a line number of those lines with commands
		// on them
		font->draw(color, 12, x, y, 0.0f, "%4i : %s", lineNumber_, line_.c_str());
	}
	else
	{
		font->draw(color, 12, x, y, 0.0f,"      %s", line_.c_str());
	}
}

GLConsoleLines::GLConsoleLines(int maxLines) :
	maxLines_(maxLines), currentLine_(0)
{
}

GLConsoleLines::~GLConsoleLines()
{
}

void GLConsoleLines::clear()
{
	currentLine_ = 0;
	while (!lines_.empty())
	{
		GLConsoleLine *line = lines_.front();
		lines_.pop_front();
		delete line;
	}
}

void GLConsoleLines::scroll(int lines)
{
	currentLine_ -= lines;
	if (currentLine_ < 0) currentLine_ = 0;
	else if (currentLine_ >= (int) lines_.size()) 
		currentLine_ = (int) lines_.size();
}

const char *GLConsoleLines::getItem(int linecount)
{
	int linesSize = (int) lines_.size();
	if (linecount < 0)
	{
		for (int line = currentLine_ + 1; line < linesSize; line++)
		{
			GLConsoleLine *consoleline = lines_[line];
			if (consoleline->getShowPointer())
			{
				linecount++;
				currentLine_ = line;
				if (linecount == 0) break;
			}
		}
	}
	else
	{
		for (int line = currentLine_ - 1; line >= 0 && line < linesSize; line--)
		{
			GLConsoleLine *consoleline = lines_[line];
			if (consoleline->getShowPointer())
			{
				linecount--;
				currentLine_ = line;
				if (linecount == 0) break;
			}
		}
	}

	// Check the command is valid and return the line
	if (currentLine_ >= 0 && currentLine_ < linesSize)
	{
		GLConsoleLine *line = lines_[currentLine_];
		if (line->getShowPointer()) return line->getLine();
	}
	return "";
}

void GLConsoleLines::addLine(const char *text, bool showPointer)
{
	if (showPointer) reset();

	const int bufferSize = 80;
	int len = (int) strlen(text);
	if (len < bufferSize)
	{
		addSmallLine(text, showPointer);
	}
	else
	{
		static char buffer[bufferSize + 2];
		int c = 0;
		const char *a;
		for (a=text, c=0; *a; a++, c++)
		{
			buffer[c] = *a;
			if ((c==bufferSize) || (c>bufferSize-15 && *a ==' '))
			{
				buffer[c+1] = '\0';
				c=-1;
				addSmallLine(buffer, showPointer);
			}
		}
		if (buffer[0]) 
		{
			buffer[c+1] = '\0';
			addSmallLine(buffer, showPointer);
		}
	}
}

void GLConsoleLines::addSmallLine(const char *text, bool showPointer)
{
	if (lines_.size() == maxLines_)
	{
		// We need to reuse the lines
		// reuse the first line
		GLConsoleLine *line = lines_.back();
		lines_.pop_back();
		line->set(text, showPointer);
		lines_.push_front(line);
	}
	else
	{
		// Add a new line
		GLConsoleLine *line = new GLConsoleLine;
		line->set(text, showPointer);
		lines_.push_front(line);
	}

	// Set the lookat and cursor to point at the new line
	if (currentLine_ != 0)
	{
		currentLine_ ++;
		if (currentLine_ > (int) lines_.size()) 
			currentLine_ = (int) lines_.size();
	}
}

void GLConsoleLines::drawLines(GLFont2d *font, float startHeight, float totalHeight, float totalWidth)
{
	if (currentLine_ != 0)
	{
		// Draw arrows at the bottom of the screen if stuff off bottom
		glColor3f(0.7f, 0.7f, 0.7f);
		glBegin(GL_TRIANGLES);
			for (float a=100.0f; a<totalWidth - 100.0f; a+=totalWidth / 25.0f)
			{
				glVertex2f(a + 20.0f, startHeight - 4.0f);
				glVertex2f(a + 24.0f, startHeight - 8.0f);
				glVertex2f(a + 28.0f, startHeight - 4.0f);
			}
		glEnd();
	}

	bool offTop = false;
	{
		// Draw all of the text
		// Stops each drawLine() from changing state
		GLState currentState(GLState::TEXTURE_ON | GLState::BLEND_ON);

		float position = startHeight + 20.0f;
		for (int i=currentLine_; i<(int) lines_.size(); i++)
		{
			GLConsoleLine *line = lines_[i];
			line->drawLine(20.0f, position, font);

			position += 15.0f;
			if (position > totalHeight - 20.0f)
			{
				offTop = true;
				break;
			}
		}
	}

	if (offTop)
	{
		// Draw arrows at top of screen if stuff off top
		glColor3f(0.7f, 0.7f, 0.7f);
		glBegin(GL_TRIANGLES);
			for (float a=100.0f; a<totalWidth - 100.0f; a+=totalWidth / 25.0f)
			{
				glVertex2f(a + 24.0f, totalHeight - 4.0f);
				glVertex2f(a + 20.0f, totalHeight - 8.0f);
				glVertex2f(a + 28.0f, totalHeight - 8.0f);
			}
		glEnd();
	}
}
