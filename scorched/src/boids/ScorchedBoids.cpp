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

#include <boids/ScorchedBoids.h>
#include <boids/BoidWorld.h>
#include <common/OptionsDisplay.h>
#include <3dsparse/ModelID.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeBoids.h>
#include <landscape/LandscapeMaps.h>
#include <client/ScorchedClient.h>

ScorchedBoids::ScorchedBoids()
{
}

ScorchedBoids::~ScorchedBoids()
{
}

void ScorchedBoids::generate()
{
	// Clean up old boids
	while (!worlds_.empty())
	{
		BoidWorld *world = worlds_.back();
		worlds_.pop_back();
		delete world;
	}

	// Create some new boids based on the definitions in the
	// landscape texture file
	LandscapeTex &tex = 
		*ScorchedClient::instance()->getLandscapeMaps().
			getDefinitions().getTex();
	LandscapeDefn &defn = 
		*ScorchedClient::instance()->getLandscapeMaps().
			getDefinitions().getDefn();

	addBoids(tex.texDefn.boids);
	addBoids(defn.texDefn.boids);
}

void ScorchedBoids::addBoids(std::vector<LandscapeBoids *> boids)
{
	std::vector<LandscapeBoids *>::iterator itor;
	for (itor = boids.begin();
		itor != boids.end();
		itor++)
	{
		LandscapeBoids *boid = (*itor);
		addBoid(boid);
	}
}

void ScorchedBoids::addBoid(LandscapeBoids *boid)
{
	std::vector<LandscapeBoidsType *> &boids = boid->objects;
	std::vector<LandscapeBoidsType *>::iterator itor;
	for (itor = boids.begin();
		itor != boids.end();
		itor++)
	{
		LandscapeBoidsType *boids = (*itor);

		BoidWorld *world = new BoidWorld(boids);
		worlds_.push_back(world);
	}
}

void ScorchedBoids::simulate(float frameTime)
{
	if (!OptionsDisplay::instance()->getNoBOIDS())
	{
		for (unsigned int w=0; w<worlds_.size(); w++)
		{
			worlds_[w]->simulate(frameTime);
		}
	}
}

void ScorchedBoids::draw()
{
	if (!OptionsDisplay::instance()->getNoBOIDS())
	{
		for (unsigned int w=0; w<worlds_.size(); w++)
		{
			worlds_[w]->draw();
		}
	}
}
