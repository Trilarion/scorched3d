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

#if !defined(AFX_GLWIDGET_H__3F7BC394_576B_4ADF_8771_7D97EB3AF314__INCLUDED_)
#define AFX_GLWIDGET_H__3F7BC394_576B_4ADF_8771_7D97EB3AF314__INCLUDED_

#include <engine/MetaClass.h>
#include <common/KeyboardHistory.h>

class XMLNode;
class GLWTip;
class GLWPanel;

/**
A base class for al GL Widgets.
Defines the interface.
**/
class GLWidget : public MetaClass
{
public:
	GLWidget(float x = 0.0f, float y = 0.0f, 
		float w = 0.0f, float h = 0.0f);
	virtual ~GLWidget();

	// The widgets implementation
	virtual void draw();
	virtual void simulate(float frameTime);
	virtual void mouseDown(float x, float y, bool &skipRest);
	virtual void mouseUp(float x, float y, bool &skipRest);
	virtual void mouseDrag(float mx, float my, float x, float y, bool &skipRest);
	virtual void keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest);

	// Accessors
	unsigned int getId() { return id_; }
	virtual bool initFromXML(XMLNode *node);
	virtual void setToolTip(GLWTip *tooltip) { tooltip_ = tooltip; }
	virtual void setParent(GLWPanel *parent);

	// Width, height and position functions
	virtual float getX() { return x_; }
	virtual float getY() { return y_; }
	virtual float getW() { return w_; }
	virtual float getH() { return h_; }
	virtual void setX(float x) { x_ = x; }
	virtual void setY(float y) { y_ = y; }
	virtual void setW(float w) { w_ = w; }
	virtual void setH(float h) { h_ = h; }

	// Helper Functions
	static bool inBox(float posX, float posY, float x, float y, float w, float h);
	static void drawRoundBox(float x, float y, float w, float h, float size);
	static void drawShadedRoundBox(float x, float y, float w, float h, float size, bool depressed);
	static void drawCircle(int startA, int endA, float posX, float posY, float size);
	static void drawWholeCircle(bool cap = false);
	static void drawBox(float x, float y, float w, float h, bool depressed);

protected:
	static unsigned int nextId_;
	unsigned int id_;
	float x_, y_, w_, h_;
	GLWTip *tooltip_;
	bool tooltipTransparent_;
	GLWPanel *parent_;

};

/**
Class that defines a GL Widget Condition.
A condition is an expression that evaluates to true or false
and determines if the associated widget should be drawn.
**/
class GLWCondition : public MetaClass
{
public:
	GLWCondition();
	virtual ~GLWCondition();

	virtual bool getResult(GLWidget *widget) = 0;
	virtual bool initFromXML(XMLNode *node);
};

#endif // !defined(AFX_GLWIDGET_H__3F7BC394_576B_4ADF_8771_7D97EB3AF314__INCLUDED_)
