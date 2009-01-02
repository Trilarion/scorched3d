////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2009
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

#include <GLW/GLWPlanView.h>
#include <GLW/GLWTranslate.h>
#include <GLEXT/GLState.h>
#include <GLEXT/GLViewPort.h>
#include <image/ImageFactory.h>
#include <tankgraph/TargetRendererImplTank.h>
#include <client/ScorchedClient.h>
#include <client/ClientState.h>
#include <graph/MainCamera.h>
#include <client/ClientLinesHandler.h>
#include <landscape/Landscape.h>
#include <landscape/LandscapePoints.h>
#include <landscapemap/LandscapeMaps.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankPosition.h>
#include <common/Vector.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsTransient.h>
#include <coms/ComsMessageSender.h>
#include <coms/ComsLinesMessage.h>
#include <graph/OptionsDisplay.h>
#include <lang/LangResource.h>
#include <math.h>

REGISTER_CLASS_SOURCE(GLWPlanView);

static const float degToRad = 3.14f / 180.0f;
static const float maxAnimationTime = 2.0f;

GLWPlanView::GLWPlanView(float x, float y, float w, float h) :
	GLWidget(x, y, w, h),
	animationTime_(0.0f), flashTime_(0.0f), totalTime_(0.0f), pointTime_(0.0f),
	flash_(true), dragging_(false), firstTime_(true),
	planColor_(1.0f)
{
	setToolTip(new ToolTip(ToolTip::ToolTipHelp,
		LANG_RESOURCE("PLAN_VIEW", "Plan View"),
		LANG_RESOURCE("PLAN_VIEW_TOOLTIP", "Shows the position of the the tanks\n"
		"on a overhead map of the island.\n"
		"Flashing tanks are still to make a move.\n"
		"Clicking on the plan will move the camera\n"
		"to look at that point.")));
}

GLWPlanView::~GLWPlanView()
{
}

void GLWPlanView::simulate(float frameTime)
{
	planColor_ += frameTime * 0.2f;
	if (planColor_ > 1.0f) planColor_ = 1.0f;

	totalTime_ += frameTime;
	flashTime_ += frameTime;
	if (flashTime_ > 0.3f)
	{
		flashTime_ = 0.0f;
		flash_ = !flash_;
	}

	animationTime_ += frameTime;
	if (animationTime_ > maxAnimationTime)
	{
		animationTime_ = 0.0f;
	}

	// Send messages about lines that have been drawn once every
	// 2 seconds
	pointTime_ += frameTime;
	if (pointTime_ > 2.0f)
	{
		pointTime_ = 0.0f;

		if (!sendPoints.empty())
		{
			Vector last = sendPoints.back();
			last[2] = 0.0f;

			ComsLinesMessage message(
				ScorchedClient::instance()->getTankContainer().getCurrentPlayerId());
			message.getLines() = sendPoints;
			if (dragging_) message.getLines().push_back(Vector::getNullVector());
			sendPoints.clear();
			ComsMessageSender::sendToServer(message);

			if (dragging_) sendPoints.push_back(last);
		}
	}

	// Simulate all points
	simulateLine(localPoints_); // Local
	std::list<PlayerDrawnInfo>::iterator playeritor; // Remote
	for (playeritor = dragPoints_.begin();
		playeritor != dragPoints_.end();
		)
	{
		PlayerDrawnInfo &info = (*playeritor);
		std::list<Vector>::iterator recieveitor;
		for (recieveitor = info.recievepoints.begin();
			recieveitor != info.recievepoints.end();
			recieveitor++)
		{
			Vector &v = (*recieveitor);
			if (v[2] > 3.0f) v[2] = 3.0f;
			v[2] -= frameTime;
		}

		if (!simulateLine(info))
		{
			playeritor = dragPoints_.erase(playeritor);
		}
		else
		{
			playeritor++;
		}
	}
}

void GLWPlanView::draw()
{
	landscapeWidth_ = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getLandscapeWidth();
	landscapeHeight_ = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getLandscapeHeight();
	arenaX_ = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getArenaX();
	arenaY_ = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getArenaY();
	arenaWidth_ = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getArenaWidth();
	arenaHeight_ = (float) ScorchedClient::instance()->
		getLandscapeMaps().getGroundMaps().getArenaHeight();

	if (firstTime_)
	{
		firstTime_ = false;
		if (!OptionsDisplay::instance()->getNoPlanDraw())
		{
			ClientLinesHandler::instance()->registerCallback(this);
		}
	}

	GLWidget::draw();
	drawMap();
}

void GLWPlanView::drawMap()
{
	GLState currentState(GLState::DEPTH_OFF | GLState::BLEND_ON | GLState::TEXTURE_ON);

	float maxWidth = MAX(arenaWidth_, arenaHeight_);

	glPushMatrix();
		glTranslatef(x_ + 10.0f, y_ + 10.0f, 0.0f);
		glScalef((w_ - 20.0f) / maxWidth, (h_ - 20.0f) / maxWidth, 1.0f);
		glTranslatef((maxWidth - arenaWidth_) / 2.0f - arenaX_,
			(maxWidth - arenaHeight_) / 2.0f - arenaY_, 0.0f);

		drawTexture();

		{
			GLState currentState2(GLState::TEXTURE_OFF);
			drawTanks();
			drawBuoys();
		}
		{
			drawCameraPointer();
		}
	glPopMatrix();

	drawLines();
}

void GLWPlanView::drawLines()
{
	if (localPoints_.points.empty() &&
			dragPoints_.empty()) return;

	GLState currentState2(GLState::TEXTURE_OFF);
	glPushMatrix();
	glTranslatef(x_ + 2.0f, y_ + 2.0f, 0.0f);
	glScalef(w_ - 4.0f, h_ - 4.0f, 1.0f);

	glLineWidth(2.0f);
	if (!localPoints_.points.empty())
	{
		localPoints_.playerId = ScorchedClient::instance()->
			getTankContainer().getCurrentPlayerId();
		drawLine(localPoints_);
	}

	if (!dragPoints_.empty())
	{
		std::list<PlayerDrawnInfo>::iterator playeritor;
		for (playeritor = dragPoints_.begin();
			playeritor != dragPoints_.end();
			playeritor++)
		{
			PlayerDrawnInfo &info = (*playeritor);
			drawLine(info);
		}
	}
	glLineWidth(1.0f);

	glPopMatrix();
}

bool GLWPlanView::simulateLine(PlayerDrawnInfo &info)
{
	while (!info.recievepoints.empty())
	{
		Vector &first = info.recievepoints.front();
		if (first[2] > 0.0f) break;
		first[2] = totalTime_;
		info.points.push_back(first);
		info.recievepoints.pop_front();
	}

	while (!info.points.empty())
	{
		Vector &first = info.points.front();
		float time = totalTime_ - first[2];
		if (time < 3.0f) break;
		info.points.pop_front();
	}

	return !(info.points.empty() && info.recievepoints.empty());
}

void GLWPlanView::drawLine(PlayerDrawnInfo &info)
{
	Tank *current =
		ScorchedClient::instance()->getTankContainer().getTankById(
			info.playerId);
	if (!current)
	{
		info.points.clear();
		info.recievepoints.clear();
		return;
	}

	if (info.points.empty()) return;

	glBegin(GL_LINE_STRIP);
	std::list<Vector>::iterator itor;
	for (itor = info.points.begin();
		itor != info.points.end();
		itor++)
	{
		Vector &v = (*itor);

		if (v[0] == 0.0f && v[1] == 0.0f)
		{
			glEnd();
			glBegin(GL_LINE_STRIP);
		}
		else
		{
			if (v[0] >= 0.0f && v[1] >= 0.0f &&
				v[0] <= 1.0f && v[1] <= 1.0f)
			{
				float time = totalTime_ - v[2];
				time = 1.0f - (time / 3.0f);
				glColor4f(
					current->getColor()[0],
					current->getColor()[1],
					current->getColor()[2],
					time);
				glVertex2f(v[0], v[1]);
			}
		}
	}
	glEnd();
}

void GLWPlanView::drawTexture()
{
	float leftScale = (arenaX_) / landscapeWidth_;
	float rightScale = (arenaX_ + arenaWidth_) / landscapeWidth_;
	float bottomScale = (arenaY_) / landscapeHeight_;
	float topScale = (arenaY_ + arenaHeight_) / landscapeHeight_;

	// Draw the square of land
	glColor3f(planColor_, planColor_, planColor_);
	Landscape::instance()->getPlanATexture().draw(true);
	glBegin(GL_QUADS);
		glTexCoord2f(rightScale, bottomScale);
		glVertex2f(arenaX_ + arenaWidth_, arenaY_);
		glTexCoord2f(rightScale, topScale);
		glVertex2f(arenaX_ + arenaWidth_, arenaY_ + arenaHeight_);
		glTexCoord2f(leftScale, topScale);
		glVertex2f(arenaX_, arenaY_ + arenaHeight_);
		glTexCoord2f(leftScale, bottomScale);
		glVertex2f(arenaX_, arenaY_);
	glEnd();
}

void GLWPlanView::drawCameraPointer()
{
	float maxWidth = MAX(arenaWidth_, arenaHeight_);
	float width = maxWidth / w_ * 10.0f;
	float height = maxWidth / h_ * 10.0f;

	static bool createdTexture = false;
	if (!createdTexture)
	{
		createdTexture = true;
		ImageHandle logoMap = ImageFactory::loadAlphaImageHandle(
			S3D::getDataFile("data/windows/arrow_s.png"));
		arrowTex_.create(logoMap);
	}

	arrowTex_.draw();

	// Get camera positions
	Vector currentPos = MainCamera::instance()->getCamera().getCurrentPos();
	Vector lookAt = MainCamera::instance()->getCamera().getLookAt();
	Vector direction = (currentPos - lookAt).Normalize2D();
	Vector directionPerp = direction.get2DPerp().Normalize2D();

	direction[0] *= width;
	direction[1] *= height;
	directionPerp[0] *= width;
	directionPerp[1] *= height;

	glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
	glPushMatrix();
		glTranslatef(lookAt[0] + direction[0], lookAt[1] + direction[1], 0.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(-direction - directionPerp);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(-direction + directionPerp);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3fv(direction + directionPerp);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3fv(direction - directionPerp);
		glEnd();
	glPopMatrix();
}

void GLWPlanView::drawBuoys()
{
	//Get the wall type and set the colour accordingly
	switch(ScorchedClient::instance()->getOptionsTransient().getWallType())
	{
	case OptionsTransient::wallWrapAround:
		glColor3f(0.5f, 0.5f, 0.0f);	// Keep the colours dark on the outside
		break;				// to try to give a look of shape
	case OptionsTransient::wallBouncy:
		glColor3f(0.0f, 0.0f, 0.5f);
		break;
	case OptionsTransient::wallConcrete:
		glColor3f(0.2f, 0.2f, 0.2f);
		break;
	case OptionsTransient::wallNone:
		return;
	default:
		break;	// should never happen....
	}

	std::vector<Vector> &points = Landscape::instance()->getPoints().getPoints();

	// Plot the dots, scaling for non-square maps
	// Draw the dots!
	//glEnable(GL_POINT_SMOOTH);
	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (int a=0; a<2; a++)
	{
		for (int i=0; i<(int) points.size(); i++)
		{
			glVertex3f(points[i][0], points[i][1], 0.0f);
		}

		if (a == 0)
		{
			glEnd();

			switch(ScorchedClient::instance()->getOptionsTransient().getWallType())
			{
			case OptionsTransient::wallWrapAround:
				glColor3f(0.9f, 0.9f, 0.4f);	// Keep the colours dark on the outside
				break;				// to try to give a look of shape
			case OptionsTransient::wallBouncy:
				glColor3f(0.4f, 0.4f, 0.4f);
				break;
			case OptionsTransient::wallConcrete:
				glColor3f(0.6f, 0.6f, 0.6f);
				break;
			default:
				break;	// should never happen....
			}

			glPointSize(2.0f);
			glBegin(GL_POINTS);
		}
	}
	glEnd();

	//glDisable(GL_POINT_SMOOTH);
	glPointSize(1.0f);
}
void GLWPlanView::drawTanks()
{
	float maxWidth = MAX(arenaWidth_, arenaHeight_);

	std::map<unsigned int, Tank *> &currentTanks =
		ScorchedClient::instance()->getTankContainer().getPlayingTanks();
	if (currentTanks.empty()) return;

	Vector position;
	//glEnable(GL_POINT_SMOOTH);

	Tank *currentTank = ScorchedClient::instance()->getTankContainer().getCurrentTank();
	if (currentTank)
	{
		currentTank->getPosition().getTankPosition().asVector(position);

		glColor3f(0.0f, 0.0f, 0.0f);
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		glVertex3fv(position);
		glEnd();
	}

	glPointSize(7.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 0.0f);
	std::map<unsigned int, Tank *>::iterator itor;
	for (itor = currentTanks.begin();
		itor != currentTanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getState() == TankState::sNormal &&
			!tank->getState().getSpectator())
		{
			tank->getPosition().getTankPosition().asVector(position);
			glVertex3fv(position);
		}
	}
	glEnd();

	glPointSize(5.0f);
	glBegin(GL_POINTS);
	for (itor = currentTanks.begin();
		itor != currentTanks.end();
		itor++)
	{
		Tank *tank = (*itor).second;
		if (tank->getState().getState() == TankState::sNormal &&
			!tank->getState().getSpectator())
		{
			glColor3fv(tank->getColor());
			tank->getPosition().getTankPosition().asVector(position);

			if ((flash_ && tank->getState().getReadyState() == TankState::SNotReady) ||
				tank->getState().getReadyState() == TankState::sReady)
			{
				glVertex3fv(position);
			}

			TargetRendererImplTank *renderer = (TargetRendererImplTank *)
				tank->getRenderer();
			if (renderer)
			{
				GLWToolTip::instance()->addToolTip(&renderer->getTips()->tankTip,
					GLWTranslate::getPosX() + x_ + 10.0f +
						(position[0] - arenaX_ + (maxWidth - arenaWidth_)/2.0f) / maxWidth * (w_ - 20.0f) - 4.0f,
					GLWTranslate::getPosY() + y_ + 10.0f +
						(position[1] - arenaY_ + (maxWidth - arenaHeight_)/2.0f) / maxWidth * (h_ - 20.0f) - 4.0f,
					8.0f, 8.0f);
			}
		}
	}
	glEnd();
	//glDisable(GL_POINT_SMOOTH);
}

void GLWPlanView::mouseDown(int button, float x, float y, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (button == GameState::MouseButtonLeft)
		{
			int arenaX = ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getArenaX();
			int arenaY = ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getArenaY();
			int arenaWidth = ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getArenaWidth();
			int arenaHeight = ScorchedClient::instance()->
				getLandscapeMaps().getGroundMaps().getArenaHeight();

			float maxWidth = (float) MAX(arenaWidth, arenaHeight);
			float mapX = (((x - x_ - 10.0f) / (w_ - 20.0f)) * maxWidth) + arenaX_ -
				(maxWidth - arenaWidth_) / 2.0f;
			float mapY = (((y - y_ - 10.0f) / (h_ - 20.0f)) * maxWidth) + arenaY_ -
				(maxWidth - arenaHeight_) / 2.0f;

			if (mapX > arenaX && mapY > arenaY &&
				mapX < arenaX + arenaWidth &&
				mapY < arenaY + arenaHeight)
			{
				Vector lookAt(mapX, mapY, ScorchedClient::instance()->
					getLandscapeMaps().getGroundMaps().getInterpHeight(
						fixed::fromFloat(mapX), fixed::fromFloat(mapY)).asFloat() + 5.0f);
				MainCamera::instance()->getTarget().setCameraType(TargetCamera::CamFree);
				MainCamera::instance()->getCamera().setLookAt(lookAt);
			}
		}
		else if (button == GameState::MouseButtonRight)
		{
			dragging_ = true;

			dragLastX_ = x;
			dragLastY_ = y;

			float mapX = ((x - x_) / w_);
			float mapY = ((y - y_) / h_);
			localPoints_.points.push_back(Vector(mapX, mapY, totalTime_));
			sendPoints.push_back(Vector(mapX, mapY, pointTime_));
		}
	}
}

void GLWPlanView::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		skipRest = true;
		if (inBox(mx, my, x_, y_, w_, h_))
		{
			if (fabsf(mx - dragLastX_) > 5.0f ||
				fabsf(my - dragLastY_) > 5.0f)
			{
				dragLastX_ = mx;
				dragLastY_ = my;

				float mapX = ((mx - x_) / w_);
				float mapY = ((my - y_) / h_);
				localPoints_.points.push_back(Vector(mapX, mapY, totalTime_));
				sendPoints.push_back(Vector(mapX, mapY, pointTime_));
			}
		}
	}
}

void GLWPlanView::mouseUp(int button, float x, float y, bool &skipRest)
{
	if (dragging_)
	{
		dragging_ = false;
		localPoints_.points.push_back(Vector::getNullVector());
		sendPoints.push_back(Vector::getNullVector());
	}
}

void GLWPlanView::addRecievePoints(unsigned int playerId,
	std::list<Vector> &recievepoints)
{
	PlayerDrawnInfo *foundInfo = 0;
	std::list<PlayerDrawnInfo>::iterator dragItor;
	for (dragItor = dragPoints_.begin();
		dragItor != dragPoints_.end();
		dragItor++)
	{
		PlayerDrawnInfo &info = (*dragItor);
		if (info.playerId == playerId)
		{
			foundInfo = &info;
		}
	}
	if (!foundInfo)
	{
		planColor_ = 0.2f;

		dragPoints_.push_back(PlayerDrawnInfo());
		foundInfo = &dragPoints_.back();
		foundInfo->playerId = playerId;
	}

	foundInfo->recievepoints.insert(foundInfo->recievepoints.end(),
		recievepoints.begin(), recievepoints.end());
}
