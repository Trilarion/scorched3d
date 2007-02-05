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

#include <dialogs/SoundDialog.h>
#include <graph/OptionsDisplay.h>
#include <sound/Sound.h>
#include <GLW/GLWTextButton.h>
#include <GLW/GLWWindowManager.h>
#include <GLW/GLWSlider.h>

SoundDialog *SoundDialog::instance_ = 0;

SoundDialog *SoundDialog::instance()
{
	if (!instance_)
	{
		instance_ = new SoundDialog;
	}
	return instance_;
}

SoundDialog::SoundDialog() :
	GLWWindow("Sound", 10.0f, 10.0f, 300.0f, 70.0f, eSmallTitle,
		"Allow the user to change sound settings")
{
	needCentered_ = true;

	GLWPanel *sliderPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	sliderPanel->addWidget(new GLWLabel(0.0f, 0.0f, "Volume"), 0, SpaceRight, 10.0f);
	soundVolume_ = new GLWSlider(0.0f, 0.0f, 300.0f);
	sliderPanel->addWidget(soundVolume_, 0, AlignCenterLeftRight);
	sliderPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(sliderPanel, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	GLWPanel *checkPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	noSoundBox_ = new GLWCheckBoxText(0.0f, 0.0f, "No Sound");
	checkPanel->addWidget(noSoundBox_, 0, SpaceRight, 10.0f);
	noAmbientSoundBox_ = new GLWCheckBoxText(0.0f, 0.0f, "No Ambient Sound");
	checkPanel->addWidget(noAmbientSoundBox_, 0, SpaceRight, 10.0f);
	noMusicBox_ = new GLWCheckBoxText(0.0f, 0.0f, "No Music");
	checkPanel->addWidget(noMusicBox_);
	checkPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(checkPanel, 0, SpaceLeft | SpaceRight | SpaceTop, 10.0f);

	GLWPanel *buttonPanel = new GLWPanel(0.0f, 0.0f, 0.0f, 0.0f, false, false);
	GLWButton *cancelButton = new GLWTextButton("Cancel", 95, 10, 105, this, 
		GLWButton::ButtonFlagCancel | GLWButton::ButtonFlagCenterX);
	cancelId_ = cancelButton->getId();
	buttonPanel->addWidget(cancelButton, 0, SpaceRight, 10.0f);
	GLWButton *okButton = new GLWTextButton("Ok", 235, 10, 55, this, 
		GLWButton::ButtonFlagOk | GLWButton::ButtonFlagCenterX);
	okId_ = okButton->getId();
	buttonPanel->addWidget(okButton);
	buttonPanel->setLayout(GLWPanel::LayoutHorizontal);
	addWidget(buttonPanel, 0, SpaceAll, 10.0f);

	setLayout(GLWPanel::LayoutVerticle);
	layout();
}

SoundDialog::~SoundDialog()
{
}

void SoundDialog::display()
{
	GLWWindow::display();

	soundVolume_->setCurrent(
		float(OptionsDisplay::instance()->getSoundVolume()) / 1.28f);
	noSoundBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoSound());
	noMusicBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoMusic());
	noAmbientSoundBox_->getCheckBox().setState(
		OptionsDisplay::instance()->getNoAmbientSound());
}

void SoundDialog::buttonDown(unsigned int id)
{
	if (id == okId_)
	{
		OptionsDisplay::instance()->getNoSoundEntry().setValue(
			noSoundBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getNoMusicEntry().setValue(
			noMusicBox_->getCheckBox().getState());
		OptionsDisplay::instance()->getNoAmbientSoundEntry().setValue(
			noAmbientSoundBox_->getCheckBox().getState());

		int volume = int(soundVolume_->getCurrent() * 1.28f);
		OptionsDisplay::instance()->getSoundVolumeEntry().setValue(volume);
		Sound::instance()->getDefaultListener()->setGain(float(volume) / 128.0f);
	}
	GLWWindowManager::instance()->hideWindow(getId());
}
