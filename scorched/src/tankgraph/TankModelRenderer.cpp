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


#include <tankgraph/TankModelRenderer.h>
#include <tankgraph/TankModelStore.h>
#include <landscape/GlobalHMap.h>
#include <landscape/Landscape.h>
#include <landscape/Hemisphere.h>
#include <actions/TankFalling.h>
#include <client/MainCamera.h>
#include <common/OptionsDisplay.h>
#include <GLEXT/GLCameraFrustum.h>
#include <GLW/GLWFont.h>

float TankModelRendererHUD::timeLeft_ = -1.0f;
float TankModelRendererHUD::percentage_ = -1.0f;
std::string TankModelRendererHUD::textA_ = "";
std::string TankModelRendererHUD::textB_ = "";

TankModelRenderer::TankModelRenderer(Tank *tank) :
	tank_(tank), tankTip_(tank),
	model_(0), canSeeTank_(false),
	smokeTime_(0.0f), smokeWaitForTime_(0.0f),
	fireOffSet_(0.0f), posX_(0.0f), posY_(0.0f), posZ_(0.0f)
{
	model_ = TankModelStore::instance()->getModelByName(
		tank->getModel().getModelName());
}

TankModelRenderer::~TankModelRenderer()
{
}

void TankModelRenderer::draw(bool currentTank)
{
	// Check we can see the tank
	canSeeTank_ = true;
	if (!GLCameraFrustum::instance()->
		sphereInFrustum(tank_->getPhysics().getTankPosition(), 1) ||
		(tank_->getState().getState() != TankState::sNormal))
	{
		canSeeTank_ = false;
		return;
	}

	// Store the position in which we should draw the players names
	storeTank2DPos();

	// Add the tank shadow
	GLState currentState(GLState::TEXTURE_OFF);
	Landscape::instance()->getShadowMap().addCircle(
		tank_->getPhysics().getTankPosition()[0], 
		tank_->getPhysics().getTankPosition()[1], 
		2.0f);

	// Draw the life bars
	drawLife();

	// Draw the tank model
	model_->draw(currentTank, 
		tank_->getPhysics().getAngle(),
		tank_->getPhysics().getTankPosition(), 
		fireOffSet_, 
		tank_->getPhysics().getRotationGunXY(), 
		tank_->getPhysics().getRotationGunYZ());
}

void TankModelRenderer::drawSecond(bool currentTank)
{
	if (!canSeeTank_) return;

	// Draw the current shield (if any)
	if (tank_->getAccessories().getShields().getCurrentShield())
	{
		drawShield();
	}

	// Draw the names above all the tanks
	if (OptionsDisplay::instance()->getDrawPlayerNames())
	{
		Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX(); 
		bilX *= float(tank_->getNameLen()) * 0.35f;

		glDepthMask(GL_FALSE);
		GLWFont::instance()->getFont()->drawBilboard(
			tank_->getColor(), 1,
			(float) tank_->getPhysics().getTankPosition()[0] - bilX[0], 
			(float) tank_->getPhysics().getTankPosition()[1] - bilX[1], 
			(float) tank_->getPhysics().getTankPosition()[2] + 8.0f,
			tank_->getName());
		glDepthMask(GL_TRUE);
	}

	// Draw the parachutes (if any)
	if (tank_->getAccessories().getParachutes().parachutesEnabled())
	{
		std::set<unsigned int>::iterator findItor = 
			TankFalling::fallingTanks.find(tank_->getPlayerId());
		if (findItor != TankFalling::fallingTanks.end())
		{
			drawParachute();
		}
	}
}

void TankModelRenderer::drawShield()
{
	// Create the shield textures
	static GLTexture *texture = 0;
	static GLTexture *texture2 = 0;
	static GLUquadric *obj = 0;
	if (!texture)
	{
		GLBitmap map( PKGDIR "data/textures/bordershield/grid2.bmp", 
			 PKGDIR "data/textures/bordershield/grid2.bmp", false);
		texture = new GLTexture;
		texture->create(map, GL_RGBA, true);

		GLBitmap map2( PKGDIR "data/textures/bordershield/grid22.bmp", 
			 PKGDIR "data/textures/bordershield/grid22.bmp", false);
		texture2 = new GLTexture;
		texture2->create(map2, GL_RGBA, true);

		obj = gluNewQuadric();
		gluQuadricTexture(obj, GL_TRUE);
	}

	// Create the shield objects
	static unsigned int smallListNo = 0;
	static unsigned int largeListNo = 0;
	static unsigned int smallMagListNo = 0;
	static unsigned int largeMagListNo = 0;
	if (!smallListNo)
	{
		glNewList(smallListNo = glGenLists(1), GL_COMPILE);
			gluSphere(obj, 3.0f, 8, 8);
		glEndList();
		glNewList(largeListNo = glGenLists(1), GL_COMPILE);
			gluSphere(obj, 6.0f, 8, 8);
		glEndList();
		Hemisphere h;
		glNewList(smallMagListNo = glGenLists(1), GL_COMPILE);
			h.draw(3.0f, 3.0f, 10, 10, 6, 0, true);
			h.draw(3.0f, 3.0f, 10, 10, 6, 0, false);
		glEndList();
		glNewList(largeMagListNo = glGenLists(1), GL_COMPILE);
			h.draw(6.0f, 6.0f, 10, 10, 6, 0, true);
			h.draw(6.0f, 6.0f, 10, 10, 6, 0, false);
		glEndList();
	}

	// Draw the actual shield
	Shield *shield = tank_->getAccessories().getShields().getCurrentShield();
	GLState state(GLState::BLEND_ON | GLState::TEXTURE_ON); 

	Vector &position = tank_->getPhysics().getTankPosition();
	Vector &color = shield->getColor();
	texture->draw();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
		glColor4f(color[0], color[1], color[2], 0.5f);
		glTranslatef(position[0], position[1], position[2]);
		if (shield->getShieldType() == Shield::ShieldTypeReflectiveMag)
		{
			if (shield->getRadius() == Shield::ShieldSizeSmall) glCallList(smallMagListNo);
			else glCallList(largeMagListNo);
		}
		else
		{
			if (shield->getRadius() == Shield::ShieldSizeSmall) glCallList(smallListNo);
			else glCallList(largeListNo);
		}
	glPopMatrix();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void TankModelRenderer::drawParachute()
{
	static GLuint listNo = 0;
	if (!listNo)
	{
		float a;
		glNewList(listNo = glGenLists(1), GL_COMPILE);
			glColor3f(1.0f, 1.0f, 1.0f);
			glBegin(GL_LINES);
				for (a=0.0f; a< 3.14f*2.0f; a+=3.14f / 4.0f)
				{
					glVertex3f(0.0f, 0.0f, 0.0f);
					glVertex3f(sinf(a) * 2.0f, cosf(a) * 2.0f, 2.0f);
				}
			glEnd();
			glBegin(GL_TRIANGLE_FAN);
				glVertex3f(0.0f, 0.0f, 3.0f);

				glColor3f(0.5f, 0.5f, 0.5f);
				for (a=3.14f*2.0f; a> 0.0f; a-=3.14f / 4.0f)
				{
					glVertex3f(sinf(a) * 2.0f, cosf(a) * 2.0f, 2.0f);
				}				
			glEnd();
		glEndList();
	}

	Vector &position = tank_->getPhysics().getTankPosition();
	GLState state(GLState::TEXTURE_OFF);
	glPushMatrix();
		glTranslatef(position[0], position[1], position[2]);
		glCallList(listNo);
	glPopMatrix();
}

void TankModelRenderer::fired()
{
	fireOffSet_ = -0.25f;
}

void TankModelRenderer::simulate(float frameTime)
{
	if (fireOffSet_ < 0.0f)
	{
		fireOffSet_ += frameTime / 25.0f;
		if (fireOffSet_ > 0.0f) fireOffSet_ = 0.0f;
	}
	if (tank_->getState().getLife() < 100)
	{
		smokeTime_ += frameTime;
		if (smokeTime_ >= smokeWaitForTime_)
		{
			const float randOff = 1.0f;
			const float randOffDiv = 0.5f;
			float randX = RAND * randOff - randOffDiv; 
			float randY = RAND * randOff - randOffDiv; 
			Landscape::instance()->getSmoke().addSmoke(
				tank_->getPhysics().getTankTurretPosition()[0] + randX, 
				tank_->getPhysics().getTankTurretPosition()[1] + randY, 
				tank_->getPhysics().getTankTurretPosition()[2]);

			smokeWaitForTime_ = (
				(RAND * float(tank_->getState().getLife()) * 10.0f) + 250.0f) 
				/ 3000.0f;;
			smokeTime_ = 0.0f;
		}
	}
}

void TankModelRenderer::drawLife()
{
	Vector &bilX = GLCameraFrustum::instance()->getBilboardVectorX();
	bilX /= 2.0f;

	Vector &position = tank_->getPhysics().getTankPosition();
	float height = position[2];
	float groundHeight = GlobalHMap::instance()->getHMap().
		getHeight((int) position[0], (int) position[1]);
	if (height < groundHeight)
	{
		height = groundHeight;
	}

	glBegin(GL_TRIANGLES);
		glColor3fv(tank_->getColor());
		glVertex3f(position[0], 
			position[1], height + 5.0f);
		glVertex3f(position[0] + bilX[0], 
			position[1] + bilX[1], height + 8.0f);
		glVertex3f(position[0] - bilX[0], 
			position[1] - bilX[1], height + 8.0f);
	glEnd();

	float shieldLife = 0.0f;
	Shield *currentShield =
		tank_->getAccessories().getShields().getCurrentShield();
	if (currentShield) shieldLife = 
		tank_->getAccessories().getShields().getShieldPower();

	drawLifeBar(bilX, tank_->getState().getLife(), height, 3.3f);
	drawLifeBar(bilX, shieldLife, height, 3.7f);
}

void TankModelRenderer::drawLifeBar(Vector &bilX, float value, 
									float height, float barheight)
{
	Vector &position = tank_->getPhysics().getTankPosition();
	glBegin(GL_QUADS);
		if (value == 100.0f || value == 0.0f)
		{
			if (value == 100.0f) glColor3f(0.0f, 1.0f, 0.0f);
			else glColor3f(0.0f, 0.0f, 0.0f);

			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight);
			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight);
		}
		else
		{
			float lifePer = value / 50.0f;
			static Vector newBilX;
			newBilX = -bilX;
			newBilX += bilX * lifePer;

			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] - bilX[0], 
				position[1] - bilX[1], height + barheight);

			glColor3f(1.0f, 0.0f, 0.0f);

			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight);
			glVertex3f(position[0] + bilX[0], 
				position[1] + bilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight + 0.2f);
			glVertex3f(position[0] + newBilX[0], 
				position[1] + newBilX[1], height + barheight);
		}
	glEnd();
}

void TankModelRenderer::storeTank2DPos()
{
	Vector &tankTurretPos = 
		tank_->getPhysics().getTankTurretPosition();
	Vector camDir = 
		MainCamera::instance()->getCamera().getLookAt() - 
		MainCamera::instance()->getCamera().getCurrentPos();
	Vector tankDir = tankTurretPos - 
		MainCamera::instance()->getCamera().getCurrentPos();

	if (camDir.dotP(tankDir) < 0.0f)
	{
		posX_ = - 1000.0;
	}
	else
	{
		static GLdouble modelMatrix[16];
		static GLdouble projMatrix[16];
		static GLint viewport[4];

		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
		glGetIntegerv(GL_VIEWPORT, viewport);

		int result = gluProject(
			tankTurretPos[0], 
			tankTurretPos[1], 
			tankTurretPos[2],
			modelMatrix, projMatrix, viewport,
			&posX_, 
			&posY_,
			&posZ_);
	}
}

void TankModelRenderer::draw2d(bool currentTank)
{
	if (!canSeeTank_) return;

	// Add the tooltip that displays the tank info
	GLWToolTip::instance()->addToolTip(&tankTip_,
		float(posX_) - 10.0f, float(posY_) - 10.0f, 20.0f, 20.0f);

	// Draw the hightlighted ring around the tank
	if (currentTank)
	{
		GLState firstState(GLState::DEPTH_OFF);
		if (TankModelRendererHUD::drawText())
		{
			Vector yellow(0.7f, 0.7f, 0.0f);
			GLWFont::instance()->getFont()->draw(
				yellow, 12,
				(float) posX_ + 47.0f, (float) posY_ - 4.0f, (float) posZ_,
				TankModelRendererHUD::getTextA());
			GLWFont::instance()->getFont()->draw(
				yellow, 12,
				(float) posX_ + 47.0f, (float) posY_ - 13.0f, (float) posZ_,
				TankModelRendererHUD::getTextB());

			if (TankModelRendererHUD::getPercentage() >= 0.0f)
			{
				float totalHeight = 40.0f;
				float halfHeight = totalHeight / 2.0f;
				float height = totalHeight * TankModelRendererHUD::getPercentage() / 100.0f;
				
				GLState state2(GLState::TEXTURE_OFF);
				glBegin(GL_QUADS);
					glColor3f(1.0f, 0.0f, 0.0f);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight + height);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight + height);

					glColor3f(0.0f, 0.0f, 0.0f);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight + height);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight + height);
					glVertex2f((float) posX_ + 45.0f, (float) posY_ -halfHeight + totalHeight);
					glVertex2f((float) posX_ + 42.0f, (float) posY_ -halfHeight + totalHeight);
				glEnd();
				glBegin(GL_LINES);
					glColor3f(1.0f, 0.0f, 0.0f);
					glVertex2f((float) posX_ + 40.0f, (float) posY_ -halfHeight);
					glVertex2f((float) posX_ + 47.0f, (float) posY_ -halfHeight);

					glVertex2f((float) posX_ + 40.0f, (float) posY_ +halfHeight);
					glVertex2f((float) posX_ + 47.0f, (float) posY_ +halfHeight);
				glEnd();
			}
		}

		GLState newState(GLState::TEXTURE_OFF | GLState::BLEND_ON);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

		glColor4f(
			tank_->getColor()[0],
			tank_->getColor()[1],
			tank_->getColor()[2],
			0.3f);

		static GLuint listNo = 0;
		if (!listNo)
		{
			glNewList(listNo = glGenLists(1), GL_COMPILE);
				glBegin(GL_QUADS);
					for (float a=0; a<6.25f ;a+=0.25f)
					{
						const float skip = 0.2f;
						glVertex2d(sin(a + skip) * 40.0, 
							cos(a + skip) * 40.0);
						glVertex2d(sin(a) * 40.0, 
							cos(a) * 40.0);
						glVertex2d(sin(a) * 35.0, 
							cos(a) * 35.0);			
						glVertex2d(sin(a + skip) * 35.0, 
							cos(a + skip) * 35.0);
					}
				glEnd();
			glEndList();
		}

		glPushMatrix();
			glTranslated(posX_, posY_, 0.0);
			glCallList(listNo);
		glPopMatrix();
	}
}
