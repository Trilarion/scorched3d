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

#if !defined(__INCLUDE_ParticleEngineh_INCLUDE__)
#define __INCLUDE_ParticleEngineh_INCLUDE__

#include <engine/Particle.h>
#include <engine/GameStateI.h>

class ParticleEngine : public GameStateI
{
public:
	ParticleEngine(unsigned int maxParticles);
	virtual ~ParticleEngine();

	void setMaxParticles(unsigned int maxParticles);
	unsigned int getMaxParticles();
	unsigned int getParticlesOnScreen();

	void killAll();

	Particle *getNextAliveParticle();

	void setFast(float speedMult) { speed_ = speedMult; }
	float getFast() { return speed_; }

	// Inherited from GameStateI
	virtual void draw(const unsigned state);
	virtual void simulate(const unsigned int state, float simTime);

protected:
	Particle *particles_;
	Particle **freeParticles_;
	Particle **usedParticles_;

	float speed_;
	unsigned int maxParticles_;
	unsigned int particlesOnScreen_;

};

#endif // __INCLUDE_ParticleEngineh_INCLUDE__