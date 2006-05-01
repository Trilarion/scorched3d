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


#if !defined(__INCLUDE_GLWListViewh_INCLUDE__)
#define __INCLUDE_GLWListViewh_INCLUDE__

#include <GLW/GLWidget.h>
#include <GLW/GLWScrollW.h>
#include <XML/XMLNode.h>
#include <common/Vector.h>
#include <vector>
#include <string>

class GLWListViewI
{
public:
	virtual void url(const char *url) = 0;
};

class GLWListView :
	public GLWidget
{
public:
	GLWListView(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f, 
		int maxLen = -1, float textSize = 10.0f,
		float scrollSpeed = 0.0f);
	virtual ~GLWListView();

	void setHandler(GLWListViewI *handler) { handler_ = handler; }
	void setColor(Vector &color) { color_ = color; }

	bool addXML(XMLNode *node);
	void addLine(const char *text);
	void clear();

	void resetPosition() { currentPosition_ = 0.0f; }

	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(float x, float y, bool &skipRest);
	virtual void mouseUp(float x, float y, bool &skipRest);
	virtual void mouseDrag(float mx, float my, float x, float y, bool &skipRest);

	virtual void setX(float x) { x_ = x; scroll_.setX(x_ + w_ - 17); }
	virtual void setY(float y) { y_ = y; scroll_.setY(y_); }
	virtual void setW(float w) { w_ = w; scroll_.setX(x_ + w_ - 17); }
	virtual void setH(float h) { h_ = h; scroll_.setH(h_ - 1); }

	REGISTER_CLASS_HEADER(GLWListView);

protected:
	struct WordEntry
	{
		WordEntry(const char *word, Vector &color) : 
			word_(word), color_(color) { }

		Vector color_;
		std::string href_;
		std::string word_;
	};
	struct LineEntry
	{
		std::vector<WordEntry> words_;
	};
	struct UrlEntry
	{
		float x_, y_;
		float w_, h_;
		WordEntry *entry_;
	};

	GLWListViewI *handler_;
	float currentPosition_;
	float scrollSpeed_;
	float textSize_;
	int maxLen_;
	Vector color_;
	GLWScrollW scroll_;
	std::vector<LineEntry> lines_;
	std::vector<UrlEntry> urls_;

	void setScroll();
	bool addWordEntry(std::list<WordEntry> &words, 
		std::string &word, XMLNode *parentNode);
	bool getLines(std::list<WordEntry> &words, float &lineLen);
	bool getWords(XMLNode *node, std::list<WordEntry> &words);
	void drawUrl(WordEntry &entry, int drawChars, float x, float y);
};

#endif
