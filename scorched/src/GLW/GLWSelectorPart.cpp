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

#include <GLW/GLWSelectorPart.h>
#include <GLW/GLWFont.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <client/ScorchedClient.h>
#include <common/Defines.h>

GLWSelectorPart::GLWSelectorPart(GLWSelectorI *user,
	int basePosition,
	float x, float y,
	std::list<GLWSelectorEntry> &entries,
	bool transparent) :
	user_(user),
	entries_(entries),
	transparent_(transparent),
	basePosition_(basePosition)
{
	calculateDimensions(x, y);
}

GLWSelectorPart::~GLWSelectorPart()
{
}

void GLWSelectorPart::calculateDimensions(float drawX, float drawY)
{
	// Get the height and width of the selector
	GLFont2d &font = *GLWFont::instance()->getLargePtFont();
	float selectedHeight = 10.0f; // Padding
	float selectedWidth = 0.0f;
	selected_ = false;
	icon_ = false;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor =	entries_.begin();
		itor != entries_.end();
		itor++)
	{
		// Get height
		GLWSelectorEntry &item = (*itor);
		if (item.getText()[0] == '-') selectedHeight += 8.0f;
		else selectedHeight += 18.0f;

		// Get width
		float currentwidth = 
			(float) font.getWidth(12, (char *) item.getText()) + 20.0f;
		if (item.getSelected()) selected_ = true;
		if (item.getIcon()) icon_ = true;
		if (currentwidth > selectedWidth) selectedWidth = currentwidth;
	}
	float indent = 0.0f;
	if (selected_) indent += 10.0f;
	if (icon_) indent += 16.0f;
	selectedWidth += indent;

	float selectedX = drawX;
	float selectedY = drawY;
	if (selectedX + selectedWidth > GLViewPort::getWidth())
	{
		selectedX -= (selectedX + selectedWidth) - GLViewPort::getWidth();
	}
	else if (selectedX < 0.0f) selectedX = 0.0f;
	if (selectedY - selectedHeight < 0)
	{
		selectedY -= (selectedY - selectedHeight);
	}

	selectedWidth_ = selectedWidth;
	selectedHeight_ = selectedHeight;
	selectedX_ = selectedX;
	selectedY_ = selectedY;
	selectedIndent_ = indent;
}

void GLWSelectorPart::draw()
{
	GLState currentStateBlend(GLState::TEXTURE_OFF | GLState::DEPTH_OFF | GLState::BLEND_ON);

	GLFont2d &font = *GLWFont::instance()->getLargePtFont();
	int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
	int mouseY = ScorchedClient::instance()->getGameState().getMouseY();

	// Draw the background
	{
		if (transparent_) glColor4f(0.4f, 0.6f, 0.8f, 0.6f);
		else glColor3f(0.8f, 0.8f, 1.0f);

		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(selectedX_ + 20.0f, 
				selectedY_ - 25.0f + 5.0f);
			glVertex2f(selectedX_ + 20.0f, 
				selectedY_ - selectedHeight_ + 5.0f);
			GLWidget::drawRoundBox(
				selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
				selectedWidth_, selectedHeight_, 10.0f);
			glVertex2f(selectedX_ + 20.0f, 
				selectedY_ - selectedHeight_ + 5.0f);
		glEnd();

		glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
		glLineWidth(2.0f);
		glBegin(GL_LINE_LOOP);
			if (transparent_)
			{
				GLWidget::drawRoundBox(
					selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
					selectedWidth_, selectedHeight_, 10.0f);
			}
			else
			{
				GLWidget::drawShadedRoundBox(
					selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
					selectedWidth_, selectedHeight_, 10.0f, true);
			}
		glEnd();
		glLineWidth(1.0f);
	}

	GLWToolTip::instance()->clearToolTip(
		selectedX_, selectedY_ - selectedHeight_ + 5.0f, 
		selectedWidth_, selectedHeight_);

	// Draw the menu items
	float currentTop = selectedY_;
	std::list<GLWSelectorEntry>::iterator itor;
	for (itor =	entries_.begin();
		itor != entries_.end();
		itor++)
	{
		GLWSelectorEntry &item = (*itor);

		// Check if the item is a seperator
		if (item.getText()[0] == '-')
		{
			// Draw a seperator
			glBegin(GL_LINES);
				glColor3f(0.0f, 0.0f, 0.0f);
				glVertex2f(selectedX_ + 5.0f, currentTop - 5.0f);
				glVertex2f(selectedX_ + selectedWidth_ - 5.0f, currentTop - 5.0f);
			glEnd();
			currentTop -= 8.0f;
		}
		else
		{
			// Draw the actual line
			bool selected = false;
			if (selectedX_ < mouseX && mouseX < selectedX_ + selectedWidth_ &&
				currentTop - 18.0f <= mouseY && mouseY < currentTop)
			{
				selected = true;
			}

			if (item.getToolTip())
			{
				GLWToolTip::instance()->addToolTip(item.getToolTip(),
					selectedX_, currentTop - 18.0f, selectedWidth_, 18.0f);
			}

			if (item.getIcon())
			{
				glColor3f(1.0f, 1.0f, 1.0f);
				GLState textureOn(GLState::TEXTURE_ON);
				item.getIcon()->draw();
				GLWSelector::instance()->drawIconBox(selectedX_ + (selected_?15.0f:5.0f), currentTop - 19.0f);
			}

			static Vector color(0.9f, 0.9f, 1.0f);
			static Vector itemcolor(0.1f, 0.1f, 0.4f);
			static Vector selectedColor(0.3f, 0.3f, 0.7f);
			Vector *c = 0;
			if (transparent_) c = (selected?&color:&itemcolor);
			else c = (selected?&selectedColor:&GLWFont::widgetFontColor);

			if (item.getSelected())
			{
				font.draw(*c, 12, selectedX_ + 5.0f, 
					currentTop - 16.0f, 0.0f, "x");
			}
			font.draw(*c, 12, selectedX_ + selectedIndent_ + 10.0f, 
				currentTop - 16.0f, 0.0f, (char *) item.getText());
			currentTop -= 18.0f;
		}
	}
}

void GLWSelectorPart::mouseDown(float mouseX, float mouseY, bool &hit)
{
	bool thisMenu = (mouseX > selectedX_ && mouseX < selectedX_ + selectedWidth_ && 
		mouseY < selectedY_ && mouseY > selectedY_ - selectedHeight_);
	if (thisMenu)
	{
		// Draw the menu items
		int position = 0;
		float currentTop = selectedY_;
		std::list<GLWSelectorEntry>::iterator itor;
		for (itor =	entries_.begin();
			 itor != entries_.end();
			 itor++)
		{
			GLWSelectorEntry &item = (*itor);
		
			// Check if the item is a seperator
			if (item.getText()[0] == '-')
			{
				position++;
				currentTop -= 8.0f;
			}
			else
			{
				// Check if the item is selected
				if (selectedX_ < mouseX && mouseX < selectedX_ + selectedWidth_ &&
					currentTop - 18.0f <= mouseY && mouseY < currentTop)
				{
					hit = true;
					if (user_) user_->itemSelected(&item, 
						basePosition_ + position);
				}

				position++;
				currentTop -= 18.0f;
			}
		}
	}
}