////////////////////////////////////////////////////////////////////////////////
//    Scorched3D (c) 2000-2004
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

#include <GLW/GLWChannelText.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWColors.h>
#include <GLEXT/GLState.h>
#include <image/ImageFactory.h>
#include <XML/XMLParser.h>
#include <client/ScorchedClient.h>
#include <client/ClientChannelManager.h>
#include <tank/TankContainer.h>
#include <tank/TankState.h>
#include <tank/TankColorGenerator.h>
#include <common/ToolTip.h>
#include <common/Defines.h>
#include <common/Keyboard.h>

std::list<ChannelText> GLWChannelText::lastMessages_;

REGISTER_CLASS_SOURCE(GLWChannelText);

GLWChannelText::GLWChannelText() :
	ctime_(0.0f), cursor_(false), maxTextLen_(0), 
	visible_(false),
	button_(x_ + 2.0f, y_ + 4.0f, 12.0f, 12.0f),
	fontSize_(12.0f), outlineFontSize_(14.0f),
	whisperDest_(0)
{
	view_.setHandler(this);
	button_.setHandler(this);
	button_.setToolTip(new ToolTip(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"Chat", "Show chat menu"));
	prompt_.setChannelView(&view_);
}

GLWChannelText::~GLWChannelText()
{
}

void GLWChannelText::simulate(float frameTime)
{
	view_.simulate(frameTime);
	button_.simulate(frameTime);

	if (!visible_) return;

	ctime_ += frameTime;
	if (ctime_ > 0.5f)
	{
		ctime_ = 0.0f;
		cursor_ = !cursor_;
	}
}

void GLWChannelText::draw()
{
	view_.draw();
	if (view_.getParentSized())
	{
		setW(view_.getW());
		setH(view_.getH());
	}

	if (!buttonTexture_.textureValid())
	{
		ImageHandle buttonImg = ImageFactory::loadAlphaImageHandle(
			S3D::getDataFile("data/windows/arrow_r.png"));
		buttonTexture_.create(buttonImg, false);
		button_.setTexture(&buttonTexture_);
		ImageHandle map = ImageFactory::loadImageHandle(
			S3D::getDataFile("data/windows/white.bmp"));
		colorTexture_.create(map);
	}

	button_.draw();

	if (!visible_) return;

	GLWidget::draw();
	glColor4f(0.4f, 0.6f, 0.8f, 0.6f);

	{
		GLState currentStateBlend(GLState::BLEND_ON);
		glBegin(GL_TRIANGLE_FAN);
			glVertex2f(x_ + 25.0f, y_ + 5.0f);
			glVertex2f(x_ + 25.0f, y_);
			drawRoundBox(x_ + 15.0f, y_, w_ - 15.0f, h_, 10.0f);
			glVertex2f(x_ + 25.0f, y_);
		glEnd();
	}

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
		drawRoundBox(x_ + 15.0f, y_, w_ - 15.0f, h_, 10.0f);
	glEnd();

	// Get the width of the prompt
	float promptWidth = GLWFont::instance()->getGameFont()->getWidth(
		fontSize_, prompt_.getText());

	// Draw prompt black outline
	GLWFont::instance()->getGameShadowFont()->
		drawA(GLWColors::black, 1.0f, fontSize_,
			x_ + 20.0f - 1.0f, y_ + 5.0f + 1.0f, 0.0f, 
			prompt_.getText());
	
	// Draw text black outline
	GLWFont::instance()->getGameShadowFont()->drawWidthRhs(
		w_ - 25.0f - promptWidth,
		GLWColors::black, fontSize_,
		x_ + 20.0f + promptWidth - 1.0f, y_ + 5.0f + 1.0f, 0.0f, 
		text_.c_str());

	// Draw the prompt
	GLWFont::instance()->getGameFont()->
		drawA(&prompt_, channelEntry_.color, 1.0f, fontSize_,
			x_ + 20.0f, y_ + 5.0f, 0.0f, 
			prompt_.getText());

	// Draw the text
	GLWFont::instance()->getGameFont()->drawWidthRhs(
		w_ - 25.0f - promptWidth,
		channelEntry_.color, fontSize_,
		x_ + 20.0f + promptWidth, y_ + 5.0f, 0.0f, 
		text_.c_str());
}

void GLWChannelText::keyDown(char *buffer, unsigned int keyState, 
		KeyboardHistory::HistoryElement *history, int hisCount, 
		bool &skipRest)
{
	if (visible_) skipRest = true;
	for (int i=0; i<hisCount; i++)
	{
		char c = history[i].representedKey;
		unsigned int dik = history[i].sdlKey;

		if (!visible_)
		{
			processNotVisibleKey(c, dik, skipRest);
		}
		else
		{
			processVisibleKey(c, dik);
		}
	}
	if (visible_) skipRest = true;
	else view_.keyDown(buffer, keyState, history, hisCount, skipRest);
}

void GLWChannelText::processNotVisibleKey(char c, unsigned int dik, bool &skipRest)
{
	// Check all of the key entries to see if the channel needs to be displayed
	std::map<KeyboardKey *, std::string>::iterator keyItor;
	for (keyItor = keys_.begin();
		keyItor != keys_.end();
		keyItor++)
	{
		KeyboardKey *key = keyItor->first;
		std::string &channel = keyItor->second;

		// Check in the list of keys to see if we need to display 
		std::vector<KeyboardKey::KeyEntry> &talkKeys = key->getKeys();
		std::vector<KeyboardKey::KeyEntry>::iterator itor;
		for (itor = talkKeys.begin();
			itor != talkKeys.end();
			itor++)
		{
			KeyboardKey::KeyEntry &entry = *itor;
			if (dik == entry.key)
			{
				setVisible(true);
				skipRest = true;

				text_ = "";
				if (!channel.empty())
				{
					GLWChannelView::CurrentChannelEntry *channelEntry = view_.getChannel(
						channel.c_str());
					if (channelEntry) setChannelEntry(*channelEntry);
				}

				if (entry.key == SDLK_SLASH) text_ = "/";
				else if (entry.key == SDLK_BACKSLASH) text_ = "\\";

				break;
			}
		}
	}

	if (visible_)
	{
		if (!checkCurrentChannel()) setVisible(false);
	}
}

void GLWChannelText::processVisibleKey(char c, unsigned int dik)
{
	if (dik == SDLK_BACKSPACE || dik == SDLK_DELETE)
	{
		if (!text_.empty())
		{
			text_ = text_.substr(0, text_.length() - 1);
		}
	}
	else if (dik == SDLK_ESCAPE)
	{
		if (!text_.empty())
		{
			text_ = "";
		}
		else
		{
			setVisible(false);
		}
	}
	else if (dik == SDLK_RETURN)
	{
		if (!text_.empty())
		{
			char *text = (char *) text_.c_str();
			if (text_[0] == '\\' || text_[0] == '/')
			{
				processSpecialText();
			}
			else
			{
				processNormalText();
			}
		}
		setVisible(false);
		text_ = "";
	}
	else if (c >= ' ')
	{
		if ((maxTextLen_==0) || ((int) text_.size() < maxTextLen_))
		{
			if ((text_[0] == '\\' || text_[0] == '/') && c == ' ')
			{
				processSpecialText();
			}
			else
			{
				text_ += c;
			}
		}
	}
}

void GLWChannelText::setVisible(bool visible)
{
	visible_ = visible;

	if (0 == strcmp("GLWWindow", getParent()->getClassName()))
	{
		GLWWindow *window = (GLWWindow *) getParent();
		window->setWindowLevel(visible_?40000:200000);
		GLWWindowManager::instance()->sortWindowLevels();
	}
}

void GLWChannelText::processNormalText()
{
	ChannelText text(channelEntry_.channel.c_str(), text_.c_str());
	if (channelEntry_.type & ChannelDefinition::eWhisperChannel)
	{
		text.setDestPlayerId(whisperDest_);
	}

	lastMessages_.push_back(text);
	if (lastMessages_.size() > 10) lastMessages_.pop_front();
	ClientChannelManager::instance()->sendText(text);
}

void GLWChannelText::processSpecialText()
{
	GLWChannelView::CurrentChannelEntry *channelEntry = 
		view_.getChannel(&text_[1]);
	if (channelEntry && 
		channelValid(channelEntry->channel.c_str()))
	{
		text_ = "";
		setChannelEntry(*channelEntry);
	}
	else if (strcmp("r", &text_[1]) == 0)
	{
		text_ = "";

		whisperDest_ = view_.getLastWhisperSrc();
		GLWChannelView::CurrentChannelEntry *channelEntry = 
			view_.getChannel("whisper");
		if (channelEntry && 
			channelValid(channelEntry->channel.c_str()))
		{
			setChannelEntry(*channelEntry);
		}
	}
	else if (strcmp("t", &text_[1]) == 0)
	{
		text_ = "";

		GLWChannelView::CurrentChannelEntry *channelEntry = 
			view_.getChannel("team");
		if (channelEntry && 
			channelValid(channelEntry->channel.c_str()))
		{
			setChannelEntry(*channelEntry);
		}
	}
	else if (strcmp("s", &text_[1]) == 0 ||
		strcmp("say", &text_[1]) == 0)
	{
		text_ = "";

		GLWChannelView::CurrentChannelEntry *channelEntry = 
			view_.getChannel("general");
		if (channelEntry && 
			channelValid(channelEntry->channel.c_str()))
		{
			setChannelEntry(*channelEntry);
		}
	}
}

void GLWChannelText::mouseDown(int button, float x, float y, bool &skipRest)
{
	view_.mouseDown(button, x, y, skipRest);
	button_.mouseDown(button, x, y, skipRest);
}

void GLWChannelText::mouseUp(int button, float x, float y, bool &skipRest)
{
	view_.mouseUp(button, x, y, skipRest);
	button_.mouseUp(button, x, y, skipRest);
}

void GLWChannelText::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	view_.mouseDrag(button, mx, my, x, y, skipRest);
	button_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWChannelText::setParent(GLWPanel *parent)
{
	GLWidget::setParent(parent);
	button_.setParent(parent);
	view_.setParent(parent);
}

void GLWChannelText::setX(float x)
{
	GLWidget::setX(x);
	button_.setX(x + 2.0f);
	view_.setX(x);
}

void GLWChannelText::setY(float y)
{
	GLWidget::setY(y);
	button_.setY(y + 4.0f);
	view_.setY(y + 20.0f);
}

void GLWChannelText::setW(float w)
{
	GLWidget::setW(w);
	view_.setW(w);
}

void GLWChannelText::setH(float h)
{
	GLWidget::setH(20.0f);
	view_.setH(h - 20.0f);
}

enum
{
	eMuteSelectorStart = 1000,
	eWhisperSelectorStart = 2000,
	eJoinSelectorStart = 3000,
	eLeaveSelectorStart = 4000,
	eSelectSelectorStart = 5000,
	eColorSelectorStart = 6000,
	eChatSelectorStart = 7000,
	eReplySelectorStart = 8000,
	eResendSelectorStart = 9000
};

void GLWChannelText::buttonDown(unsigned int id)
{
	// Make sure the current channel is valid
	checkCurrentChannel();

	// All of the tooltips
	static ToolTip muteTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Ignore", "Ignore chat from another player (mute)");
	static ToolTip whisperTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Whisper", "Send private chat to another player");
	static ToolTip joinTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Join Channel", "Join another chat channel.\n"
		"You will be able to see messages sent on this channel");
	static ToolTip leaveTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Leave Channel", "Leave a current chat channel.\n"
		"You will stop recieving messages sent on this channel");
	static ToolTip selectTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Select Channel", "Select the current channel.\n"
		"This is the channel you will send messages on.");
	static ToolTip colorTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Channel Color", "Change the color of the current channel.");
	static ToolTip resendTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Resend", "Resend a previously sent message.");
	static ToolTip replyTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Reply", "Reply to the last person that whispered you.");
	static ToolTip chatTooltip(ToolTip::ToolTipHelp | ToolTip::ToolTipAlignBottom, 
		"Chat", "Show or hide the chat text entry box.");

	GLWSelectorEntry mute("Ignore", &muteTooltip);
	GLWSelectorEntry whisper("Whisper", &whisperTooltip);
	GLWSelectorEntry bar("---");
	bar.setSeperator();
	GLWSelectorEntry joinChannel("Join Channel", &joinTooltip);
	GLWSelectorEntry leaveChannel("Leave Channel", &leaveTooltip);
	GLWSelectorEntry selectChannel("Select Channel", &selectTooltip);
	GLWSelectorEntry colorChannel("Channel Color", &colorTooltip);
	GLWSelectorEntry resend("Resend", &resendTooltip);
	GLWSelectorEntry reply("Reply (/r)", &replyTooltip, false, 0, (void *) eReplySelectorStart);
	GLWSelectorEntry chat("Chat", &chatTooltip, false, 0, (void *) eChatSelectorStart);

	// For each resend
	std::list<ChannelText>::iterator resendItor;
	for (resendItor = lastMessages_.begin();
		resendItor != lastMessages_.end();
		resendItor++)
	{
		ChannelText &channelText = *resendItor;
		resend.getPopups().push_back(GLWSelectorEntry(channelText.getMessage(), 
			0, false, 0, (void *) eResendSelectorStart, channelText.getMessage()));
	}

	// For each tank
	Tank *currentTank = 
		ScorchedClient::instance()->getTankContainer().getCurrentTank();
	std::map<unsigned int, Tank *> &tanks = ScorchedClient::instance()->
		getTankContainer().getPlayingTanks();
	std::map<unsigned int, Tank *>::iterator tankItor;
	for (tankItor = tanks.begin();
		tankItor != tanks.end();
		tankItor++)
	{
		Tank *tank = (*tankItor).second;
		if (tank == currentTank ||
			!tank->getDestinationId()) continue;

		// Add tanks to the mute and whisper lines
		mute.getPopups().push_back(GLWSelectorEntry(tank->getName(), 
			0, tank->getState().getMuted(), 0, (void *) eMuteSelectorStart, tank->getName()));
		whisper.getPopups().push_back(GLWSelectorEntry(tank->getName(),
			0, false, 0, (void *) eWhisperSelectorStart, tank->getName()));
	}

	// For each current channel
	std::list<GLWChannelView::CurrentChannelEntry> &channels = view_.getCurrentChannels();
	std::list<GLWChannelView::CurrentChannelEntry>::iterator channelItor;
	for (channelItor = channels.begin();
		channelItor != channels.end();
		channelItor++)
	{
		GLWChannelView::CurrentChannelEntry &channel = (*channelItor);

		// Add an entry saying which current channels we can leave
		std::string text =
			S3D::formatStringBuffer("%u. %s%s", 
				channel.id, channel.channel.c_str(),
				(channel.type & ChannelDefinition::eReadOnlyChannel?" (RO)":""));
		leaveChannel.getPopups().push_back(GLWSelectorEntry(
			text, 0, false, 0, 
			(void *) eLeaveSelectorStart, channel.channel.c_str()));

		if (channelValid(channel.channel.c_str()))
		{
			// Add an entry saying which channels we can write on
			selectChannel.getPopups().push_back(GLWSelectorEntry(
				S3D::formatStringBuffer("%u. %s %s", channel.id, channel.channel.c_str(),
				(channel.type & ChannelDefinition::eWhisperChannel?whisperDestStr_.c_str():"")),
				0, (channelEntry_.channel == channel.channel), 0, 
				(void *) eSelectSelectorStart, channel.channel.c_str()));
		}
	}

	// For each available channel
	std::list<GLWChannelView::BaseChannelEntry> &available = view_.getAvailableChannels();
	std::list<GLWChannelView::BaseChannelEntry>::iterator availableItor;
	for (availableItor = available.begin();
		availableItor != available.end();
		availableItor++)
	{
		GLWChannelView::BaseChannelEntry &channel = *availableItor;

		// Add an entry saying which channels we can join
		std::string text = 
			S3D::formatStringBuffer("%s%s",
				availableItor->channel.c_str(),
				(channel.type & ChannelDefinition::eReadOnlyChannel?" (RO)":""));
		joinChannel.getPopups().push_back(GLWSelectorEntry(text,
			0, false, 0, (void *) eJoinSelectorStart, 
			availableItor->channel.c_str()));
	}

	// For each color
	std::vector<Vector *> &colors = TankColorGenerator::instance()->getAllColors();
	std::vector<Vector *>::iterator colorItor;
	for (colorItor = colors.begin();
		colorItor != colors.end();
		colorItor++)
	{
		Vector *color = *colorItor;

		// Add an entry allowing the user to change channel color
		GLWSelectorEntry entry("", 0, false, &colorTexture_, (void *) eColorSelectorStart);
		entry.getColor() = *color;
		entry.getTextureWidth() = 32;
		colorChannel.getPopups().push_back(entry);
	}

	// Build the meny structure, with the popup menus below the main
	std::list<GLWSelectorEntry> topLevel;
	topLevel.push_back(mute);
	topLevel.push_back(whisper);
	topLevel.push_back(bar);
	topLevel.push_back(joinChannel);
	topLevel.push_back(leaveChannel);
	topLevel.push_back(selectChannel);
	topLevel.push_back(colorChannel);
	topLevel.push_back(bar);
	topLevel.push_back(resend);
	topLevel.push_back(reply);
	topLevel.push_back(chat);

	// Show all the entries
	float x = (float) ScorchedClient::instance()->getGameState().getMouseX();
	float y = (float) ScorchedClient::instance()->getGameState().getMouseY();
	GLWSelector::instance()->showSelector(this, x, y, topLevel);
}

void GLWChannelText::itemSelected(GLWSelectorEntry *entry, int position)
{
	int type = int(long(entry->getUserData()));
	switch (type)
	{
	case eMuteSelectorStart:
		{
			Tank *tank = 
				ScorchedClient::instance()->getTankContainer().
					getTankByName(entry->getDataText());
			if (tank) 
			{
				if (tank->getState().getMuted()) 
				{
					tank->getState().setMuted(false);
					ClientChannelManager::instance()->getMutedPlayers().erase(
						tank->getPlayerId());
				}
				else
				{
					tank->getState().setMuted(true);
					ClientChannelManager::instance()->getMutedPlayers().insert(
						tank->getPlayerId());
				}
			}
		}
		break;
	case eWhisperSelectorStart:
		{
			Tank *tank = 
				ScorchedClient::instance()->getTankContainer().
					getTankByName(entry->getDataText());
			if (tank)
			{
				whisperDest_ = tank->getPlayerId();
				GLWChannelView::CurrentChannelEntry *channelEntry = 
					view_.getChannel("whisper");
				if (channelEntry && 
					channelValid(channelEntry->channel.c_str()))
				{
					setChannelEntry(*channelEntry);
					if (checkCurrentChannel()) setVisible(true);
				}
			}
		}
		break;
	case eJoinSelectorStart:
		view_.joinChannel(entry->getDataText());
		break;
	case eLeaveSelectorStart:
		view_.leaveChannel(entry->getDataText());
		break;
	case eSelectSelectorStart:
		{
			GLWChannelView::CurrentChannelEntry *channel = 
				view_.getChannel(entry->getDataText());
			if (channel) setChannelEntry(*channel);
		}
		break;
	case eColorSelectorStart:
		{
			GLWChannelView::CurrentChannelEntry *channel = 
				view_.getChannel(channelEntry_.channel.c_str());
			if (channel) 
			{
				channelEntry_.color = entry->getColor();
				channel->color = entry->getColor();
			}
		}
		break;
	case eChatSelectorStart:
		text_ = "";
		if (visible_) setVisible(false);
		else if (checkCurrentChannel()) setVisible(true);
		break;
	case eReplySelectorStart:
		{
			whisperDest_ = view_.getLastWhisperSrc();
			GLWChannelView::CurrentChannelEntry *channelEntry = 
				view_.getChannel("whisper");
			if (channelEntry && 
				channelValid(channelEntry->channel.c_str()))
			{
				setChannelEntry(*channelEntry);

				text_ = "";
				setVisible(true);
			}
		}
		break;
	case eResendSelectorStart:
		{
			std::list<ChannelText>::iterator resendItor;
			for (resendItor = lastMessages_.begin();
				resendItor != lastMessages_.end();
				resendItor++)
			{
				ChannelText &text = *resendItor;
				if (0 == strcmp(text.getMessage(), entry->getDataText())) 
				{
					ClientChannelManager::instance()->sendText(text);
					break;
				}
			}
		}
		break;
	}
}

void GLWChannelText::channelsChanged(unsigned int id)
{
	checkCurrentChannel();
}

bool GLWChannelText::channelValid(const char *channelName)
{
	// Check the whisper destination is ok
	bool whisperDestValid = false;
	if (whisperDest_)
	{
		Tank *whisperTank = ScorchedClient::instance()->getTankContainer().
			getTankById(whisperDest_);
		if (whisperTank)
		{
			whisperDestValid = true;
			whisperDestStr_ = whisperTank->getName();
		}
		else
		{
			whisperDest_ = 0;
			whisperDestStr_ = "";
		}
	}

	// Check the actual channel exists
	GLWChannelView::CurrentChannelEntry *channelEntry = view_.getChannel(
		channelName);				
	if (!channelEntry) return false;

	// Check if this channel is actualy readonly
	if (channelEntry->type & ChannelDefinition::eReadOnlyChannel)
	{
		return false;
	}

	// Check if the channel is a invalid whisper dest
	if ((channelEntry->type & ChannelDefinition::eWhisperChannel) && 
		!whisperDestValid)
	{
		return false;
	}
	return true;
}

bool GLWChannelText::checkCurrentChannel()
{
	// Just turned visible, check the current channel is valid
	if (!channelValid(channelEntry_.channel.c_str()))
	{
		// Its not, try to find a valid channel
		std::list<GLWChannelView::CurrentChannelEntry> &entries = 
			view_.getCurrentChannels();
		std::list<GLWChannelView::CurrentChannelEntry>::iterator itor;
		for (itor = entries.begin();
			itor != entries.end();
			itor++)
		{
			GLWChannelView::CurrentChannelEntry &entry = *itor;
			if (channelValid(channelEntry_.channel.c_str()))
			{
				// Use the first valid channel
				setChannelEntry(entry);
				return true;
			}
		}
		return false;
	}
	else 
	{
		// Get the current channel
		GLWChannelView::CurrentChannelEntry *channelEntry = view_.getChannel(
			channelEntry_.channel.c_str());	

		// The current channel is valid
		setChannelEntry(*channelEntry);
		return true;
	}	

	return true;
}

void GLWChannelText::setChannelEntry(GLWChannelView::CurrentChannelEntry &entry)
{
	channelEntry_ = entry;

	std::string channelName = "";
	if (channelEntry_.type & ChannelDefinition::eWhisperChannel)
	{
		channelName = S3D::formatStringBuffer("%u. [c:%s][p:%s] : ",
			channelEntry_.id, channelEntry_.channel.c_str(), 
			whisperDestStr_.c_str());
	}
	else
	{
		channelName = S3D::formatStringBuffer("%u. [c:%s] : ",
			channelEntry_.id, channelEntry_.channel.c_str());
	}
	prompt_.parseText(ScorchedClient::instance()->getContext(), channelName.c_str());
}

bool GLWChannelText::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;
	if (!view_.initFromXMLInternal(node)) return false;

	XMLNode *keyNode;
	while (node->getNamedChild("talkkey", keyNode, false))
	{
		std::string key, channel;
		if (!keyNode->getNamedChild("key", key)) return false;
		if (!keyNode->getNamedChild("channel", channel)) return false;

		KeyboardKey *actualKey = Keyboard::instance()->getKey(key.c_str());
		if (!actualKey) return keyNode->returnError("Unknown key");

		keys_[actualKey] = channel;
	}

	if (!node->getNamedChild("defaultchannel", channelEntry_.channel)) return false;

	return true;
}
