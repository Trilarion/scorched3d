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


// MainBanner.h: interface for the MainBanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINBANNER_H__112625BE_5176_49BA_A456_EEE04E22CD80__INCLUDED_)
#define AFX_MAINBANNER_H__112625BE_5176_49BA_A456_EEE04E22CD80__INCLUDED_

#include <common/Logger.h>
#include <engine/GameStateI.h>
#include <GLEXT/GLFontBanner.h>

class MainBanner : public GameStateI,
	public LoggerI
{
public:
	static MainBanner* instance();

	virtual void simulate(const unsigned state, float frameTime);
	virtual void draw(const unsigned state);

	virtual void logMessage(
		const char *time,
		const char *message,
		const LoggerInfo &info);

protected:
	static MainBanner* instance_;
	GLFontBanner *banner_;

private:
	MainBanner();
	virtual ~MainBanner();

};

#endif // !defined(AFX_MAINBANNER_H__112625BE_5176_49BA_A456_EEE04E22CD80__INCLUDED_)
