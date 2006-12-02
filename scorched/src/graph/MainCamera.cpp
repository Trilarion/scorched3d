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

#include <graph/MainCamera.h>
#include <client/ClientState.h>
#include <client/ScorchedClient.h>
#include <graph/Main2DCamera.h>
#include <landscapemap/LandscapeMaps.h>
#include <landscapedef/LandscapeDefn.h>
#include <engine/ViewPoints.h>
#include <GLEXT/GLBitmap.h>
#include <dialogs/MainMenuDialog.h>
#include <sound/Sound.h>
#include <sound/SoundUtils.h>
#include <common/Keyboard.h>
#include <graph/OptionsDisplay.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <math.h>
#include <time.h>

MainCamera *MainCamera::instance_ = 0;

MainCamera *MainCamera::instance()
{
	if (!instance_)
	{
		instance_ = new MainCamera;
	}

	return instance_;
}

MainCamera::MainCamera() : 
	GameStateI("MainCamera"),
	scrollTime_(0.0f)
{
	GLBitmap *map = new GLBitmap(
		formatString(getDataFile("data/windows/camera.bmp")),
		formatString(getDataFile("data/windows/cameraa.bmp")),
		false);
	DIALOG_ASSERT(map->getBits());
	MainMenuDialog::instance()->addMenu(
		"Camera", 32, 0, this, map);
}

MainCamera::~MainCamera()
{

}

bool MainCamera::getMenuItems(const char* menuName, 
	std::list<GLMenuItem> &result)
{
	for (int i=0; i<TargetCamera::getNoCameraNames(); i++)
	{
		result.push_back(GLMenuItem(TargetCamera::getCameraNames()[i], 
			&TargetCamera::getCameraToolTips()[i],
			(targetCam_.getCameraType() == 
			(TargetCamera::CamType) i)));
	}
	return true;
}

void MainCamera::menuSelection(const char* menuName, 
	const int position, GLMenuItem &item)
{
	targetCam_.setCameraType((TargetCamera::CamType) position);
}

void MainCamera::simulate(const unsigned state, float frameTime)
{
	if (OptionsDisplay::instance()->getSideScroll())
	{
		int mouseX = ScorchedClient::instance()->getGameState().getMouseX();
		int mouseY = ScorchedClient::instance()->getGameState().getMouseY();
		int windowX = Main2DCamera::instance()->getViewPort().getWidth();
		int windowY = Main2DCamera::instance()->getViewPort().getHeight();
		float mapWidth = (float) ScorchedClient::instance()->getLandscapeMaps().
			getDefinitions().getDefn()->landscapewidth;
		float mapHeight = (float) ScorchedClient::instance()->getLandscapeMaps().
			getDefinitions().getDefn()->landscapeheight;

		scrollTime_ += frameTime;
		while (scrollTime_ > 0.0f)
		{
			scrollTime_ -= 0.1f;

			const int scrollWindow = 5;
			if (mouseX < scrollWindow)
			{
				targetCam_.setCameraType(TargetCamera::CamFree);
				if (Keyboard::instance()->getKeyboardState() & KMOD_LSHIFT)
				{
					targetCam_.getCamera().movePositionDelta(-0.3f, 0.0f, 0.0f);
				}
				else
				{
					targetCam_.getCamera().scroll(GLCamera::eScrollLeft, mapWidth, mapHeight);
				}
			}
			else if (mouseX > windowX - scrollWindow)
			{
				targetCam_.setCameraType(TargetCamera::CamFree);
				if (Keyboard::instance()->getKeyboardState() & KMOD_LSHIFT)
				{
					targetCam_.getCamera().movePositionDelta(+0.3f, 0.0f, 0.0f);
				}
				else
				{
					targetCam_.getCamera().scroll(GLCamera::eScrollRight, mapWidth, mapHeight);
				}
			}
		
			if (mouseY < scrollWindow)
			{
				targetCam_.setCameraType(TargetCamera::CamFree);
				if (Keyboard::instance()->getKeyboardState() & KMOD_LSHIFT)
				{
					targetCam_.getCamera().movePositionDelta(0.0f, 0.3f, 0.0f);
				}
				else if (Keyboard::instance()->getKeyboardState() & KMOD_LCTRL)
				{
					targetCam_.getCamera().movePositionDelta(0.0f, 0.0f, +5.0f);
				}
				else
				{
					targetCam_.getCamera().scroll(GLCamera::eScrollDown, mapWidth, mapHeight);
				}
			}
			else if (mouseY > windowY - scrollWindow)
			{
				targetCam_.setCameraType(TargetCamera::CamFree);
				if (Keyboard::instance()->getKeyboardState() & KMOD_LSHIFT)
				{
					targetCam_.getCamera().movePositionDelta(0.0f, -0.3f, 0.0f);
				}
				else if (Keyboard::instance()->getKeyboardState() & KMOD_LCTRL)
				{
					targetCam_.getCamera().movePositionDelta(0.0f, 0.0f, -5.0f);
				}
				else
				{
					targetCam_.getCamera().scroll(GLCamera::eScrollUp, mapWidth, mapHeight);
				}
			}
		}
	}

	ScorchedClient::instance()->getContext().viewPoints->simulate(frameTime);
	targetCam_.simulate(frameTime, (state == ClientState::StatePlaying));

	Sound::instance()->getDefaultListener()->setPosition(
		targetCam_.getCamera().getCurrentPos());
	Sound::instance()->getDefaultListener()->setVelocity(
		targetCam_.getCamera().getVelocity());
	Vector direction = 
		targetCam_.getCamera().getLookAt() -
		targetCam_.getCamera().getCurrentPos();
	Sound::instance()->getDefaultListener()->setOrientation(
		direction);
}

void MainCamera::draw(const unsigned state)
{
	targetCam_.draw();
}

void MainCamera::mouseDrag(const unsigned state, 
	GameState::MouseButton button, 
	int mx, int my, int x, int y, bool &skipRest)
{
	targetCam_.mouseDrag(button, mx, my, x, y, skipRest);
}

void MainCamera::mouseWheel(const unsigned state, int x, int y, int z, bool &skipRest)
{
	targetCam_.mouseWheel(x, y, z, skipRest);
}

void MainCamera::mouseDown(const unsigned state, GameState::MouseButton button, 
	int x, int y, bool &skipRest)
{
	targetCam_.mouseDown(button, x, y, skipRest);
}

void MainCamera::keyboardCheck(const unsigned state, float frameTime, 
							   char *buffer, unsigned int keyState,
							   KeyboardHistory::HistoryElement *history, 
							   int hisCount, 
							   bool &skipRest)
{
	targetCam_.keyboardCheck(frameTime, buffer,
		keyState, history, hisCount, skipRest);

	KEYBOARDKEY("SAVE_SCREEN", saveScreenKey);
	if (saveScreenKey->keyDown(buffer, keyState, false))
	{
		saveScreen_.saveScreen_ = true;
	}

	KEYBOARDKEY("CAMERA_SCROLL_UP", scrollUp);
	KEYBOARDKEY("CAMERA_SCROLL_DOWN", scrollDown);
	KEYBOARDKEY("CAMERA_SCROLL_LEFT", scrollLeft);
	KEYBOARDKEY("CAMERA_SCROLL_RIGHT", scrollRight);
	float mapWidth = (float) ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getDefn()->landscapewidth;
	float mapHeight = (float) ScorchedClient::instance()->getLandscapeMaps().
		getDefinitions().getDefn()->landscapeheight;
	if (scrollUp->keyDown(buffer, keyState)) 
		targetCam_.getCamera().scroll(GLCamera::eScrollUp, mapWidth, mapHeight);
	else if (scrollDown->keyDown(buffer, keyState)) 
		targetCam_.getCamera().scroll(GLCamera::eScrollDown, mapWidth, mapHeight);
	else if (scrollLeft->keyDown(buffer, keyState)) 
		targetCam_.getCamera().scroll(GLCamera::eScrollLeft, mapWidth, mapHeight);
	else if (scrollRight->keyDown(buffer, keyState)) 
		targetCam_.getCamera().scroll(GLCamera::eScrollRight, mapWidth, mapHeight);

	KEYBOARDKEY("CAMERA_SET_QUICK_SLOT_1", setQuick1);
	KEYBOARDKEY("CAMERA_SET_QUICK_SLOT_2", setQuick2);
	KEYBOARDKEY("CAMERA_SET_QUICK_SLOT_3", setQuick3);
	KEYBOARDKEY("CAMERA_SET_QUICK_SLOT_4", setQuick4);
	KEYBOARDKEY("CAMERA_SET_QUICK_SLOT_5", setQuick5);
	KEYBOARDKEY("CAMERA_USE_QUICK_SLOT_1", useQuick1);
	KEYBOARDKEY("CAMERA_USE_QUICK_SLOT_2", useQuick2);
	KEYBOARDKEY("CAMERA_USE_QUICK_SLOT_3", useQuick3);
	KEYBOARDKEY("CAMERA_USE_QUICK_SLOT_4", useQuick4);
	KEYBOARDKEY("CAMERA_USE_QUICK_SLOT_5", useQuick5);
	if (setQuick1->keyDown(buffer, keyState, false)) 
		setQuick(1);
	else if (setQuick2->keyDown(buffer, keyState, false)) 
		setQuick(2);
	else if (setQuick3->keyDown(buffer, keyState, false)) 
		setQuick(3);
	else if (setQuick4->keyDown(buffer, keyState, false)) 
		setQuick(4);
	else if (setQuick5->keyDown(buffer, keyState, false)) 
		setQuick(5);
	else if (useQuick1->keyDown(buffer, keyState, false)) 
		useQuick(1);
	else if (useQuick2->keyDown(buffer, keyState, false)) 
		useQuick(2);
	else if (useQuick3->keyDown(buffer, keyState, false)) 
		useQuick(3);
	else if (useQuick4->keyDown(buffer, keyState, false)) 
		useQuick(4);
	else if (useQuick5->keyDown(buffer, keyState, false)) 
		useQuick(5);
}

void MainCamera::setQuick(int key)
{
	std::pair<Vector, Vector> value(
		targetCam_.getCamera().getLookAt(),
		Vector(targetCam_.getCamera().getRotationXY(),
		targetCam_.getCamera().getRotationYZ(),
		targetCam_.getCamera().getZoom()));
	quickKeys_[key] = value;
	Logger::log(formatString("Saved camera preset %i", key));
}

void MainCamera::useQuick(int key)
{
	std::map<int, std::pair<Vector, Vector> >::iterator 
		findItor = quickKeys_.find(key);
	if (findItor != quickKeys_.end())
	{
		std::pair<Vector, Vector> value = (*findItor).second;
		targetCam_.setCameraType(TargetCamera::CamFree);
		targetCam_.getCamera().setLookAt(value.first);
		targetCam_.getCamera().movePosition(value.second[0],
			value.second[1], value.second[2]);
		Logger::log(formatString("Using camera preset %i", key));
	}
}

void MainCamera::SaveScreen::draw(const unsigned state)
{
	if (!saveScreen_) return;
	saveScreen_ = false;

	static unsigned counter = 0;
	time_t currentTime = time(0);
	char *fileName = (char *) 
		getHomeFile(formatString("ScreenShot-%i-%i.bmp", currentTime, counter++));

	GLBitmap screenMap;
	screenMap.grabScreen();
	screenMap.writeToFile(fileName);

	// Don't print to banner otherwise this message will be in
	// the screenshot!
	Logger::log(
		formatString("Screen shot saved as file \"%s\"", fileName));

	// snapshot sound
	CACHE_SOUND(sound,  (char *) getDataFile("data/wav/misc/camera.wav"));
	SoundUtils::playRelativeSound(VirtualSoundPriority::eText, sound);
}

void MainCamera::Precipitation::draw(const unsigned state)
{
	MainCamera::instance()->getTarget().drawPrecipitation();
}