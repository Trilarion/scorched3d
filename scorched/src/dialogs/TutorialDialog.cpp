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

#include <dialogs/TutorialDialog.h>
#include <GLEXT/GLBitmap.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWLabel.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWListView.h>
#include <client/MessageDisplay.h>
#include <client/ScorchedClient.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <common/Defines.h>
#include <time.h>

TutorialDialog *TutorialDialog::instance_ = 0;

TutorialDialog *TutorialDialog::instance()
{
	if (!instance_)
	{
		instance_ = new TutorialDialog;
	}
	return instance_;
}

TutorialDialog::TutorialDialog() : 
	GLWWindow("Tutorial", 160.0f, -120.0f, 
		470.0f, 120.0f, eTransparent | eNoTitle,
		"The ingame tutorial."),
	triangleDir_(30.0f), triangleDist_(0.0f),
	currentHighlight_(false)
{
	Vector listColor(0.0f, 0.0f, 0.0f);
	listView_ = new GLWListView(0.0f, 0.0f, 470.0f, 95.0f, -1, 12.0f, 16.0f);
	listView_->setColor(listColor);
	listView_->setHandler(this);
	addWidget(listView_, 0, SpaceAll, 10.0f);
	setLayout(GLWPanel::LayoutVerticle);
	layout();

	if (file_.parseFile(getDataFile("data/tutorial.xml")))
	{
		showPage(file_.getStartText());
	}
}

TutorialDialog::~TutorialDialog()
{

}

void TutorialDialog::showPage(XMLNode *node)
{
	currentHighlight_ = false;
	listView_->clear();
	listView_->addXML(node);
}

void TutorialDialog::display()
{
	GLWWindow::display();
	listView_->resetPosition();
}

void TutorialDialog::draw()
{
	GLWWindow::draw();

	if (currentHighlight_)
	{
		drawHighlight(
			highlightX_, highlightY_, 
			highlightW_, highlightH_);
	}
}

void TutorialDialog::simulate(float frameTime)
{
	GLWWindow::simulate(frameTime);

	triangleDist_ += frameTime * triangleDir_;
	if (triangleDist_ < 0.0f)
	{
		triangleDist_ = -triangleDist_;
		triangleDir_ = -triangleDir_;
	}
	if (triangleDist_ > 10.0f) 
	{
		triangleDist_ = 10.0f;
		triangleDir_ = -triangleDir_;
	}
}

void TutorialDialog::url(const char *url)
{
	XMLNode *node = file_.getText(url);
	if (node) showPage(node);
}

void TutorialDialog::drawHighlight(float x, float y, float w, float h)
{
	if (!triangleTex_.textureValid())
	{
		std::string file = getDataFile("data/windows/triangle.bmp");
		GLBitmap maps(file.c_str(), file.c_str(), false);
		triangleTex_.create(maps, GL_RGBA, true);
	}

	GLState state(GLState::TEXTURE_ON | GLState::BLEND_ON);
	glColor3f(1.0f, 0.0f, 0.0f);

	triangleTex_.draw();

	const float size = 20.0f;
	drawTriangle(x - size - triangleDist_, y + (h - size) / 2.0f, size, 1);
	drawTriangle(x + w + triangleDist_, y + (h - size) / 2.0f, size, 3);
	drawTriangle(x + (w - size) / 2.0f, y + h + triangleDist_, size, 2);
	drawTriangle(x + (w - size) / 2.0f, y - size - triangleDist_, size, 0);
}

void TutorialDialog::drawTriangle(float x, float y, float size, int tex)
{
	glBegin(GL_QUADS);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(0.0f, 0.0f); break;
			case 1: glTexCoord2f(1.0f, 0.0f); break;
			case 2: glTexCoord2f(1.0f, 1.0f); break;
			case 3: glTexCoord2f(0.0f, 1.0f); break;
		}
		glVertex2f(x, y);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(1.0f, 0.0f); break;
			case 1: glTexCoord2f(1.0f, 1.0f); break;
			case 2: glTexCoord2f(0.0f, 1.0f); break;
			case 3: glTexCoord2f(0.0f, 0.0f); break;
		}
		glVertex2f(x + size, y);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(1.0f, 1.0f); break;
			case 1: glTexCoord2f(0.0f, 1.0f); break;
			case 2: glTexCoord2f(0.0f, 0.0f); break;
			case 3: glTexCoord2f(1.0f, 0.0f); break;
		}
		glVertex2f(x + size, y + size);
		switch (tex) 
		{ 
			case 0: glTexCoord2f(0.0f, 1.0f); break;
			case 1: glTexCoord2f(0.0f, 0.0f); break;
			case 2: glTexCoord2f(1.0f, 0.0f); break;
			case 3: glTexCoord2f(1.0f, 1.0f); break;
		}
		glVertex2f(x, y + size);
	glEnd();
}

void TutorialDialog::setHighlight(float x, float y, float w, float h)
{
	currentHighlight_ = true;
	highlightX_ = x;
	highlightY_ = y;
	highlightW_ = w;
	highlightH_ = h;
}

static const char *getValue(const char *name,
	std::map<std::string, std::string> &event)
{
	std::map<std::string, std::string>::iterator itor =
		event.find(name);
	if (itor == event.end()) return 0;
	return (*itor).second.c_str();
}

void TutorialDialog::event(std::map<std::string, std::string> &event)
{
	const char *action = getValue("action", event);
	if (!action)
	{
		dialogExit("TutorialDialog", "No action in event");
	}

	if (0 == strcmp(action, "highlight")) 
	{
		const char *windowName = getValue("window", event);
		const char *controlName = getValue("control", event);
		if (!windowName || !controlName)
		{
			dialogExit("TutorialDialog", "No window or control in event");
		}

		GLWWindow *window = GLWWindowManager::instance()->getWindowByName(windowName);
		if (!window)
		{
			dialogExit("TutorialDialog", 
				formatString("Cannot find window \"%s\"", windowName));
		}

		GLWidget *control = window->getWidgetByName(controlName);
		if (!control)
		{
			dialogExit("TutorialDialog", 
				formatString("Cannot find control \"%s\" in window \"%s\"", 
				controlName, windowName));
		}

		setHighlight(
			control->getX() + window->getX(), control->getY() + window->getY(),
			control->getW(), control->getH());
	}
}