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

#include <server/ServerMain.h>
#include <client/ClientMain.h>
#include <client/ClientDialog.h>
#include <client/ScorchedClient.h>
#include <client/ClientAdmin.h>
#include <client/ClientSave.h>
#include <client/ClientParams.h>
#include <client/ClientChannelManager.h>
#include <client/ClientGameStoppedHandler.h>
#include <client/ClientMessageHandler.h>
#include <client/ClientConnectionRejectHandler.h>
#include <client/ClientGiftMoneyHandler.h>
#include <client/ClientLinesHandler.h>
#include <client/ClientStartGameHandler.h>
#include <client/ClientScoreHandler.h>
#include <client/ClientAddPlayerHandler.h>
#include <client/ClientNewGameHandler.h>
#include <client/ClientConnectionAcceptHandler.h>
#include <client/ClientRmPlayerHandler.h>
#include <client/ClientGameStateHandler.h>
#include <client/ClientInitializeHandler.h>
#include <client/ClientPlayerStateHandler.h>
#include <client/ClientTargetStateHandler.h>
#include <client/ClientStartTimerHandler.h>
#include <client/ClientSyncCheckHandler.h>
#include <client/ClientFileHandler.h>
#include <client/ClientPlayerAimHandler.h>
#include <client/ClientPlayMovesHandler.h>
#include <client/ClientDefenseHandler.h>
#include <client/ClientPlayerStatusHandler.h>
#include <client/ClientKeepAliveSender.h>
#include <client/ClientState.h>
#include <client/ClientWindowSetup.h>
#include <graph/FrameLimiter.h>
#include <graph/Mouse.h>
#include <graph/Gamma.h>
#include <graph/OptionsDisplay.h>
#include <graph/OptionsDisplayConsole.h>
#include <graph/MainCamera.h>
#include <graph/Main2DCamera.h>
#include <graph/Display.h>
#include <dialogs/HelpButtonDialog.h>
#include <dialogs/AnimatedBackdropDialog.h>
#include <dialogs/BackdropDialog.h>
#include <server/ScorchedServer.h>
#include <GLEXT/GLConsoleFileReader.h>
#include <GLEXT/GLConsole.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWWindowSkinManager.h>
#include <engine/MainLoop.h>
#include <engine/ActionController.h>
#include <dialogs/ProgressDialog.h>
#include <net/NetServerTCP.h>
#include <net/NetServerUDP.h>
#include <net/NetLoopBack.h>
#include <common/ARGParser.h>
#include <common/Keyboard.h>
#include <common/Logger.h>
#include <common/OptionsScorched.h>
#include <common/Keyboard.h>
#include <common/ProgressCounter.h>
#include <common/Clock.h>
#include <common/Defines.h>
#include <sound/Sound.h>
#include <SDL/SDL.h>

static int mouseEventCount = 0;
static bool paused = false;
extern char scorched3dAppName[128];

static bool initHardware(ProgressCounter *progressCounter)
{
	progressCounter->setNewPercentage(0.0f);
	progressCounter->setNewOp("Initializing Keyboard");
	if (!Keyboard::instance()->init())
	{
		dialogMessage("Scorched3D Keyboard", 
			"SDL failed to aquire keyboard.\n"
#ifdef WIN32
			"Is DirectX 5.0 installed?"
#endif
		);
		return false;
	}
	progressCounter->setNewOp("Loading Keyboard Bindings");
	if (!Keyboard::instance()->loadKeyFile())
	{
		dialogMessage("Scorched3D Keyboard", 
			"Failed to process keyboard file keys.xml");
		return false;
	}

	if (!OptionsDisplay::instance()->getNoSound())
	{
		progressCounter->setNewOp("Initializing Sound SW");
		if (!Sound::instance()->init(
			OptionsDisplay::instance()->getSoundChannels()))
		{
			dialogMessage("Scorched3D Sound", 
				"Failed to aquire sound.\n"
				"Is anything else currently using the sound card?");
		}
		Sound::instance()->getDefaultListener()->setGain(
			float(OptionsDisplay::instance()->getSoundVolume()) / 128.0f);
	}
	return true;
}

static bool initComs(ProgressCounter *progressCounter)
{
	progressCounter->setNewPercentage(0.0f);
	progressCounter->setNewOp("Initializing Coms");
	ScorchedClient::instance();

	// Tidy up any existing net handlers
	if (ScorchedClient::instance()->getContext().netInterface)
	{
		ScorchedClient::instance()->getContext().netInterface->stop();
		delete ScorchedClient::instance()->getContext().netInterface;
		ScorchedClient::instance()->getContext().netInterface = 0;
	}
	if (ScorchedServer::instance()->getContext().netInterface)
	{
		ScorchedServer::instance()->getContext().netInterface->stop();
		delete ScorchedServer::instance()->getContext().netInterface;
		ScorchedServer::instance()->getContext().netInterface = 0;
	}

	// Create the new net handlers
	if (ClientParams::instance()->getConnectedToServer())
	{
		ScorchedClient::instance()->getContext().netInterface = 
			//new NetServerTCP(new NetServerTCPScorchedProtocol());
			new NetServerUDP();
	}
	else
	{
		NetLoopBack *serverLoopBack = new NetLoopBack(NetLoopBack::ServerLoopBackID);
		ScorchedServer::instance()->getContext().netInterface = serverLoopBack;
		NetLoopBack *clientLoopBack = new NetLoopBack(NetLoopBack::ClientLoopBackID);
		ScorchedClient::instance()->getContext().netInterface = clientLoopBack;
		serverLoopBack->setLoopBack(clientLoopBack);
		clientLoopBack->setLoopBack(serverLoopBack);
	}
	ScorchedClient::instance()->getNetInterface().setMessageHandler(
		&ScorchedClient::instance()->getComsMessageHandler());
	ClientAdmin::instance();

	return true;
}

static bool initComsHandlers()
{
	// Setup the coms handlers
	ScorchedClient::instance()->getComsMessageHandler().setConnectionHandler(
		ClientMessageHandler::instance());
	ClientChannelManager::instance();
	ClientConnectionRejectHandler::instance();
	ClientGiftMoneyHandler::instance();
	ClientLinesHandler::instance();
	ClientConnectionAcceptHandler::instance();
	ClientAddPlayerHandler::instance();
	ClientNewGameHandler::instance();
	ClientInitializeHandler::instance();
	ClientRmPlayerHandler::instance();
	ClientPlayerAimHandler::instance();
	ClientStartTimerHandler::instance();
	ClientSyncCheckHandler::instance();
	ClientFileHandler::instance();
	ClientGameStoppedHandler::instance();
	ClientStartGameHandler::instance();
	ClientGameStateHandler::instance();
	ClientPlayerStateHandler::instance();
	ClientTargetStateHandler::instance();
	ClientDefenseHandler::instance();
	ClientPlayMovesHandler::instance();
	ClientPlayerStatusHandler::instance();
	ClientScoreHandler::instance();

	return true;
}

static bool initWindows(ProgressCounter *progressCounter)
{
	progressCounter->setNewPercentage(0.0f);
	progressCounter->setNewOp("Initializing Windows");
	if (!GLWWindowSkinManager::defaultinstance()->loadWindows())
	{
		dialogMessage("Scorched3D", "Failed to load default windows skins");
		return false;
	}
	ClientWindowSetup::setupStartWindows(GLWWindowSkinManager::defaultinstance());
	HelpButtonDialog::instance();

	std::string errorString;
	if (!GLConsoleFileReader::loadFileIntoConsole(getDataFile("data/autoexec.xml"), errorString))
	{
		dialogMessage("Failed to parse data/autoexec.xml", errorString.c_str());
		return false;
	}

	return true;
}

static bool initClient()
{
	ProgressCounter progressCounter;
	progressCounter.setUser(ProgressDialogSync::instance());
	progressCounter.setNewPercentage(0.0f);

	// Load in all the coms
	if (!initComs(&progressCounter)) return false;
	
	// Start the server (if required)
	if (!ClientParams::instance()->getConnectedToServer())
	{
		if (!startServer(true, &progressCounter)) return false;
	}

	ScorchedClient::instance()->getGameState().stimulate(ClientState::StimConnect);

	return true;
}

static bool startClientInternal()
{
	// Check if we are connecting to a server
	if (ClientParams::instance()->getConnect()[0])
	{
		return initClient();	
	}
	else if (ClientParams::instance()->getStartCustom() ||
		ClientParams::instance()->getClientFile()[0])
	{
		std::string clientFile = ClientParams::instance()->getClientFile();
		if (ClientParams::instance()->getStartCustom())
		{
			clientFile = getSettingsFile("singlecustom.xml");
		}

		// If not load the client settings file
		if (!s3d_fileExists(clientFile.c_str()))
		{
			dialogExit(scorched3dAppName, formatString(
				"Client file \"%s\" does not exist.",
				clientFile.c_str()));
		}
		ScorchedServer::instance()->getOptionsGame().getMainOptions().readOptionsFromFile(
			(char *) clientFile.c_str());

		return initClient();
	}
	else if (ClientParams::instance()->getSaveFile()[0])
	{
		// Or the client saved game
		if (!s3d_fileExists(ClientParams::instance()->getSaveFile()))
		{
			dialogExit(scorched3dAppName, formatString(
				"Client save file \"%s\" does not exist.",
				ClientParams::instance()->getSaveFile()));
		}
		if (!ClientSave::loadClient(ClientParams::instance()->getSaveFile()) ||
			!ClientSave::restoreClient(true, false))
		{
			dialogExit(scorched3dAppName, formatString(
				"Cannot load client save file \"%s\".",
				ClientParams::instance()->getSaveFile()));
		}

		return initClient();
	}
	else
	{
		// Do nothing
	}

	return true;
}

bool ClientMain::startClient()
{
	AnimatedBackdropDialog::instance()->drawBackground();
	BackdropDialog::instance()->capture();

	return startClientInternal();
}

bool ClientMain::clientEventLoop(float frameTime)
{
	static float serverTime = 0.0f;
	static SDL_Event event;
	bool idle = true;
	if (SDL_PollEvent(&event))
	{
		idle = false;
		switch (event.type)
		{
		/* keyboard events */
		case SDL_KEYUP:
			break;
		case SDL_KEYDOWN:
			/* keyevents are handled in mainloop */
			Keyboard::instance()->processKeyboardEvent(event);
			break;

			/* mouse events */
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			Mouse::instance()->processMouseEvent(event);
			break;
		case SDL_ACTIVEEVENT:
			if (event.active.gain == 0)
			{
				Gamma::instance()->reset();
			}
			else
			{
				Gamma::instance()->set();
			}

			paused = ( OptionsDisplay::instance()->getFocusPause() && (event.active.gain == 0));
			break;
		case SDL_VIDEORESIZE:
			/*Display::instance()->changeSettings(
				event.resize.w,event.resize.h, 
				OptionsDisplay::instance()->getFullScreen());*/
			MainCamera::instance()->getCamera().setWindowSize(
				event.resize.w, event.resize.h);
			Main2DCamera::instance()->getViewPort().setWindowSize(
				event.resize.w, event.resize.h);
				
			break;
		case SDL_QUIT:
			ScorchedClient::instance()->getMainLoop().exitLoop();
			break;
		}
	}

	ClientKeepAliveSender::instance()->sendKeepAlive();
	if (!ClientParams::instance()->getConnectedToServer())
	{
		serverTime += frameTime;
		if (serverTime > 0.05f)
		{
			serverTime = 0.0f;
			serverLoop();
		}
	}

	Logger::processLogEntries();
	if (ScorchedClient::instance()->getContext().netInterface)
	{
		ScorchedClient::instance()->getNetInterface().processMessages();
	}

	if (ClientParams::instance()->getExitTime() > 0)
	{
		if (time(0) > ClientParams::instance()->getExitTime())
		{
			exit(0);
		}
	}

	return idle;
}

bool ClientMain::clientMain()
{
	// Create the actual window
	if (!createScorchedWindow()) return false;

	// Start the initial windows
	ClientState::setupGameState();
	ProgressCounter progressCounter;
	ProgressDialog::instance()->changeTip();
	progressCounter.setUser(ProgressDialogSync::instance());
	progressCounter.setNewPercentage(0.0f);
	if (!initHardware(&progressCounter)) return false;
	if (!initWindows(&progressCounter)) return false;
	if (!initComsHandlers()) return false;

	OptionsDisplayConsole::instance()->addDisplayToConsole();

	// Try and start the client
	if (!startClientInternal()) return false;

	// Enter the SDL main loop to process SDL events
	Clock loopClock;
	FrameLimiter limiter;
	for (;;)
	{
		float frameTime = loopClock.getTimeDifference();
		bool idle = clientEventLoop(frameTime);

		if (!ScorchedClient::instance()->getMainLoop().mainLoop()) break;
		if ((!paused) && (idle) )
		{
			ScorchedClient::instance()->getMainLoop().draw();
			limiter.limitFrameTime(); // Make sure frame rate is not exceeded
		}
		else
		{
			limiter.dontLimitFrameTime();
		}
		if (paused) SDL_Delay(100);  // Otherwise when not drawing graphics its an infinite loop	
	}

	if (ScorchedClient::instance()->getContext().netInterface)
	{
		ScorchedClient::instance()->getNetInterface().disconnectAllClients();
	}
	GLWWindowManager::instance()->savePositions();
    SDL_Delay(1000);
	Gamma::instance()->reset();
	Sound::instance()->destroy();

	return true;
}

