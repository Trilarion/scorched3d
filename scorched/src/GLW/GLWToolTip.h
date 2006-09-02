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

#if !defined(__INCLUDE_GLWToolTiph_INCLUDE__)
#define __INCLUDE_GLWToolTiph_INCLUDE__

#include <engine/GameStateI.h>
#include <GLW/GLWTip.h>
#include <list>

class GLWToolTip : public GameStateI
{
public:
	static GLWToolTip *instance();

	bool addToolTip(const char *title, const char *text,
		float x, float y, float w, float h);
	bool addToolTip(GLWTip *tip, 
		float x, float y, float w, float h);
	void clearToolTip(float x, float y, float w, float h);

	// Inherited from GameStateI
	virtual void simulate(const unsigned state, float frameTime);
	virtual void draw(const unsigned state);

protected:
	static GLWToolTip *instance_;
	GLWTip *currentTip_;
	GLWTip *lastTip_;
	float timeDrawn_, timeSeen_;
	float refreshTime_;

	// GLWTip properties
	float currentX_, currentY_;
	float currentW_, currentH_;
	float tipX_, tipY_;
	float tipW_, tipH_;
	float tipTextWidth_;
	float tipTextHeight_;
	std::string tipTitle_, tipText_;
	std::list<char *> tipTexts_;

	void setupTip(GLWTip *tip);
	void calculateTip(GLWTip *tip);
private:
	GLWToolTip();
	virtual ~GLWToolTip();
};

#endif
