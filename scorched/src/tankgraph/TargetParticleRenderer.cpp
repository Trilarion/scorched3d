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

#include <tankgraph/TargetParticleRenderer.h>
#include <tankgraph/TargetRendererImpl.h>
#include <engine/Particle.h>
#include <target/TargetContainer.h>
#include <client/ScorchedClient.h>

TargetParticleRenderer *TargetParticleRenderer::getInstance()
{
	static TargetParticleRenderer instance_;
	return &instance_;
}

void TargetParticleRenderer::simulateParticle(Particle &particle, float time)
{
	Target *target =
		ScorchedClient::instance()->getTargetContainer().getTargetById(
			((TargetParticleUserData *) particle.userData_)->playerId_);
	if (target)
	{
		// Target and particle alive
		particle.life_ = 1000.0f; // Alive
		particle.position_ = target->getTargetPosition();
	}
	else
	{
		// Target expired but particle has not
		particle.life_ = 0.0f; // Dead
	}
}

void TargetParticleRenderer::renderParticle(Particle &particle)
{
	Target *target =
		ScorchedClient::instance()->getTargetContainer().getTargetById(
			((TargetParticleUserData *) particle.userData_)->playerId_);
	if (target)
	{
		TargetRenderer *renderer = target->getRenderer();
		if (renderer)
		{
			renderer->drawSecond(particle.distance_);
			glDepthMask(GL_FALSE);
		}
	}
}

void TargetParticleRenderer::recycleParticle(Particle &particle)
{
	Target *target =
		ScorchedClient::instance()->getTargetContainer().getTargetById(
			((TargetParticleUserData *) particle.userData_)->playerId_);
	if (target)
	{
		TargetRendererImpl *renderer = (TargetRendererImpl *)
			target->getRenderer();
		if (renderer)
		{
			// Particle expired but Target has not
			renderer->setMakeParticle();
		}
	}
}
