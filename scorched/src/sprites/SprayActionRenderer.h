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

#if !defined(__INCLUDE_SprayActionRendererh_INCLUDE__)
#define __INCLUDE_SprayActionRendererh_INCLUDE__

#include <engine/Action.h>
#include <common/Vector.h>
#include <GLEXT/GLBilboardRenderer.h>
#include <list>
#include <vector>

class SprayActionRenderer : public ActionRenderer
{
public:
	SprayActionRenderer(Vector &position, float width);
	virtual ~SprayActionRenderer();

	// Inherited from SimulatorAction
	virtual void simulate(Action *action, float frameTime, bool &remove);
	virtual void draw(Action *action);

protected:
	std::vector<GLBilboardRenderer::Entry*> bilboardEntries_;
	struct Entry
	{
		Vector offset;
		Vector position;
		GLTexture *texture;
	};

	std::list<Entry> entries_;
	float totalTime_;
	float time_;
	int iteration_;

};

#endif
