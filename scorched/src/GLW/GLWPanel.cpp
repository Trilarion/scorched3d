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

#include <GLEXT/GLState.h>
#include <XML/XMLParser.h>
#include <common/Defines.h>
#include <GLW/GLWTranslate.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWTab.h>

GLWPanel::GLWPanelEntry::GLWPanelEntry(GLWidget *w, GLWCondition *c,
	unsigned int f, float wi) :
	widget(w), condition(c), flags(f),
	leftSpace(0.0f), rightSpace(0.0f),
	topSpace(0.0f), bottomSpace(0.0f)
{
	if (! (f & GLWPanel::AlignLeft) &&
		! (f & GLWPanel::AlignRight) &&
		! (f & GLWPanel::AlignCenterLeftRight)) flags |= GLWPanel::AlignLeft;

	if (! (f & GLWPanel::AlignTop) &&
		! (f & GLWPanel::AlignCenterTopBottom) &&
		! (f & GLWPanel::AlignBottom)) flags |= GLWPanel::AlignBottom;

	if ((f & GLWPanel::SpaceRight) || (f & GLWPanel::SpaceAll)) rightSpace = wi;
	if ((f & GLWPanel::SpaceLeft) || (f & GLWPanel::SpaceAll)) leftSpace = wi;
	if ((f & GLWPanel::SpaceTop) || (f & GLWPanel::SpaceAll)) topSpace = wi;
	if ((f & GLWPanel::SpaceBottom) || (f & GLWPanel::SpaceAll)) bottomSpace = wi;
}

REGISTER_CLASS_SOURCE(GLWPanel);

GLWPanel::GLWPanel(float x, float y, float w, float h, 
	bool depressed, bool visible, bool ridge) : 
	GLWidget(x, y, w, h), depressed_(depressed), 
	drawPanel_(visible), layout_(LayoutNone),
	gridWidth_(0), ridge_(ridge)
{

}

GLWPanel::~GLWPanel()
{
	clear();
}

GLWidget *GLWPanel::addWidget(GLWidget *widget, GLWCondition *condition, 
	unsigned int flags, float width)
{
	std::list<GLWPanelEntry>::iterator itor;
	for (itor = widgets_.begin();
		itor != widgets_.end();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		DIALOG_ASSERT(entry.widget != widget);
	}

	GLWPanelEntry entry(widget, condition, flags, width);
	widgets_.push_back(entry);
	widget->setParent(this);
	return widget;
}

void GLWPanel::clear()
{
	std::list<GLWPanelEntry>::iterator itor;
	for (itor = widgets_.begin();
		itor != widgets_.end();
		itor++)
	{
		delete (*itor).widget;
		delete (*itor).condition;
	}
	widgets_.clear();
}

void GLWPanel::simulate(float frameTime)
{
	std::list<GLWPanelEntry>::iterator itor;
	for (itor = widgets_.begin();
		itor != widgets_.end();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		{
			entry.widget->simulate(frameTime);
		}
	}
}

void GLWPanel::draw()
{
	GLState currentState(GLState::DEPTH_OFF | GLState::TEXTURE_OFF);

	GLWidget::draw();

	glPushMatrix();
		if (drawPanel_)
		{
			glColor3f(0.8f, 0.8f, 1.0f);
			glBegin(GL_QUADS);
				glVertex2f(x_, y_);
				glVertex2f(x_ + w_, y_);
				glVertex2f(x_ + w_, y_ + h_);
				glVertex2f(x_, y_ + h_);
			glEnd();

			glBegin(GL_LINE_LOOP);
				drawShadedRoundBox(x_, y_, w_, h_, 10.0f, !depressed_);
			glEnd();

			if (ridge_)
			{
				glBegin(GL_LINE_LOOP);
					drawShadedRoundBox(x_-2.0f, y_-2.0f, w_+4.0f, h_+4.0f, 10.0f, depressed_);
				glEnd();
			}
		}

		{
			GLWTranslate trans(x_, y_);
			glTranslatef(x_, y_, 0.0f);

			std::list<GLWPanelEntry>::iterator itor;
			for (itor = widgets_.begin();
				itor != widgets_.end();
				itor++)
			{
				GLWPanelEntry &entry = *itor;
				if (!entry.condition || 
					entry.condition->getResult(entry.widget))
				{
					glPushMatrix();
						(*itor).widget->draw();
					glPopMatrix();
				}
			}
		}
	glPopMatrix();
}

void GLWPanel::mouseDown(float x, float y, bool &skipRest)
{
	x -= x_;
	y -= y_;

	GLWTranslate trans(x_, y_);

	std::list<GLWPanelEntry>::reverse_iterator itor;
	for (itor = widgets_.rbegin();
		itor != widgets_.rend();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		{
			(*itor).widget->mouseDown(x, y, skipRest);
			if (skipRest) break;
		}
	}
}

void GLWPanel::mouseUp(float x, float y, bool &skipRest)
{
	x -= x_;
	y -= y_;

	GLWTranslate trans(x_, y_);

	std::list<GLWPanelEntry>::reverse_iterator itor;
	for (itor = widgets_.rbegin();
		itor != widgets_.rend();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		(*itor).widget->mouseUp(x, y, skipRest);
		if (skipRest) break;
	}
}

void GLWPanel::mouseDrag(float mx, float my, float x, float y, bool &skipRest)
{
	mx -= x_;
	my -= y_;

	std::list<GLWPanelEntry>::reverse_iterator itor;
	for (itor = widgets_.rbegin();
		itor != widgets_.rend();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		{
			(*itor).widget->mouseDrag(mx, my, x, y, skipRest);
			if (skipRest) break;
		}
	}
}

void GLWPanel::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	std::list<GLWPanelEntry>::reverse_iterator itor;
	for (itor = widgets_.rbegin();
		itor != widgets_.rend();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		{
			(*itor).widget->keyDown(buffer, keyState, history, hisCount, skipRest);
			if (skipRest) break;
		}
	}
}

void GLWPanel::display()
{
	std::list<GLWPanelEntry>::reverse_iterator itor;
	for (itor = widgets_.rbegin();
		itor != widgets_.rend();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		{
			(*itor).widget->display();
		}
	}
}

void GLWPanel::hide()
{
	std::list<GLWPanelEntry>::reverse_iterator itor;
	for (itor = widgets_.rbegin();
		itor != widgets_.rend();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		if (!entry.condition || 
			entry.condition->getResult(entry.widget))
		{
			(*itor).widget->hide();
		}
	}
}

bool GLWPanel::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;
	drawPanel_ = false;

	// Items
	XMLNode *itemsNode;
	if (!node->getNamedChild("items", itemsNode)) return false;

	// Itterate all of the items in the file
	std::list<XMLNode *>::iterator childrenItor;
		std::list<XMLNode *> &children = itemsNode->getChildren();
	for (childrenItor = children.begin();
		childrenItor != children.end();
		childrenItor++)
	{
		// For each node named items
		XMLNode *currentNode = (*childrenItor);

		// The Widget
		XMLNode *widgetNode, *widgetTypeNode;
		if (!currentNode->getNamedChild("widget", widgetNode)) return false;
		if (!widgetNode->getNamedParameter("type", widgetTypeNode)) return false;

		// Create new type
		GLWidget *widget = (GLWidget *)
			MetaClassRegistration::getNewClass(widgetTypeNode->getContent());
		if (!widget) 
		{
			dialogMessage("GLWPanel",
				"Unknown widget type \"%s\"",
				widgetTypeNode->getContent());
			return false;
		}
		if (!widget->initFromXML(widgetNode))
		{
			dialogMessage("GLWPanel",
				"Failed to parse \"%s\" widget type",
				widgetTypeNode->getContent());
			return false;			
		}

		// The condition (if any)
		GLWCondition *condition = 0;
		XMLNode *conditionNode = 0;
		if (currentNode->getNamedChild("condition", conditionNode, false))
		{
			// Get the type of this condition
			XMLNode *conditionTypeNode;
			if (!conditionNode->getNamedParameter("type", conditionTypeNode)) return false;

			// Create type
			condition = (GLWCondition *)
				MetaClassRegistration::getNewClass(conditionTypeNode->getContent());
			if (!condition)
			{
				dialogMessage("GLWPanel",
					"Unknown condition type \"%s\"",
					conditionTypeNode->getContent());
				return false;
			}
			if (!condition->initFromXML(conditionNode))
			{
				dialogMessage("GLWPanel",
					"Failed to parse \"%s\" condition type",
					conditionTypeNode->getContent());
				return false;
			}
		}

		addWidget(widget, condition);
	}
	return true;
}

void GLWPanel::layout()
{
	if (layout_ == LayoutNone) return;
	
	float w = 0.0f;
	float h = 0.0f;
	
	// Figure out how wide and high this frame should be
	std::list<GLWPanelEntry>::iterator itor;
	for (itor = widgets_.begin();
		itor != widgets_.end();
		itor++)
	{
		GLWPanelEntry &entry = *itor;
		
		// Size this widget
		entry.widget->layout();
		
		// Get the size
		float width = entry.widget->getW() +
			entry.leftSpace + entry.rightSpace;
		float height = entry.widget->getH() +
			entry.topSpace + entry.bottomSpace;		
		if (layout_ == LayoutHorizontal)
		{
			w += width;
			h = MAX(h, height);
		}
		else if (layout_ == LayoutVerticle)
		{
			w = MAX(w, width);
			h += height;
		}
		else if (layout_ == LayoutGrid)
		{
			w = MAX(w, width);
			h = MAX(h, height);
		}
		else DIALOG_ASSERT(0);
	}
	
	float prevw = w;
	float prevh = h;
	if (layout_ == LayoutGrid)
	{
		w *= gridWidth_;
		int rows = widgets_.size() / gridWidth_;
		if (widgets_.size() % gridWidth_ > 0) rows ++;
		h *= rows;
	}
	
	// Set the width and height
	setW(w);
	setH(h);
	
	// Set the position of all the widgets
	if (layout_ == LayoutHorizontal)
	{
		float width = 0.0f;
		for (itor = widgets_.begin();
			itor != widgets_.end();
			itor++)
		{
			GLWPanelEntry &entry = *itor;
			width += entry.leftSpace;
			entry.widget->setX(width);
			
			if (entry.flags & AlignBottom)
			{
				entry.widget->setY(entry.bottomSpace);
			}
			else if (entry.flags & AlignCenterTopBottom)
			{
				entry.widget->setY(prevh / 2 - 
					entry.widget->getH() / 2);
			}
			else if (entry.flags & AlignTop)
			{
				entry.widget->setY(prevh - entry.topSpace -
					entry.widget->getH());
			}
			else DIALOG_ASSERT(0);
			
			width += entry.widget->getW();
			width += entry.rightSpace;
		}
	}
	else if (layout_ == LayoutVerticle)
	{
		float height = getH();
		for (itor = widgets_.begin();
			itor != widgets_.end();
			itor++)
		{
			GLWPanelEntry &entry = *itor;
			height -= entry.topSpace;
			entry.widget->setY(height - entry.widget->getH());
			
			if (entry.flags & AlignLeft)
			{
				entry.widget->setX(entry.leftSpace);
			}
			else if (entry.flags & AlignCenterLeftRight)
			{
				entry.widget->setX(prevw / 2 - 
					entry.widget->getW() / 2);
			}
			else if (entry.flags & AlignRight)
			{
				entry.widget->setX(prevw - entry.rightSpace -
					entry.widget->getW());
			}
			else DIALOG_ASSERT(0);
			
			height -= entry.widget->getH();
			height -= entry.bottomSpace;
		}
	}
	else if (layout_ == LayoutGrid)
	{
		DIALOG_ASSERT(gridWidth_ != 0);
	
		unsigned int cell = 0;
		float width = 0.0f;
		float height = getH();
		for (itor = widgets_.begin();
			itor != widgets_.end();
			itor++)
		{
			GLWPanelEntry &entry = *itor;
			if (entry.flags & AlignLeft)
			{
				entry.widget->setX(width + entry.leftSpace);
			}
			else if (entry.flags & AlignCenterLeftRight)
			{
				entry.widget->setX(width + prevw / 2 - 
					entry.widget->getW() / 2);
			}
			else if (entry.flags & AlignRight)
			{
				entry.widget->setX(width + prevw - entry.rightSpace -
					entry.widget->getW());
			}
			else DIALOG_ASSERT(0);
			
			if (entry.flags & AlignBottom)
			{
				entry.widget->setY(height - prevh + entry.bottomSpace);
			}
			else if (entry.flags & AlignCenterTopBottom)
			{
				entry.widget->setY(height - prevh / 2 - 
					entry.widget->getH() / 2);
			}
			else if (entry.flags & AlignTop)
			{
				entry.widget->setY(height - entry.topSpace -
					entry.widget->getH());
			}
			else DIALOG_ASSERT(0);	
		
			if (++cell >= gridWidth_)
			{
				cell = 0;	
				width = 0.0f;
				height -= prevh;
			}
			else
			{
				width += prevw;
			}
		}		
	}
	else DIALOG_ASSERT(0);
}

void GLWPanel::setLayout(unsigned int layout)
{
	layout_ = layout;
}

unsigned int GLWPanel::getLayout()
{
	return layout_;
}

void GLWPanel::setGridWidth(unsigned int width)
{
	gridWidth_ = width;
}

unsigned int GLWPanel::getGridWidth()
{
	return gridWidth_;
}

