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

#if !defined(__INCLUDE_EventContainerh_INCLUDE__)
#define __INCLUDE_EventContainerh_INCLUDE__

#include <landscapedef/LandscapeEvents.h>
#include <map>

class EventContainer
{
public:
	EventContainer();
	virtual ~EventContainer();

	void clear();
	void initialize(ScorchedContext &context);
	void simulate(float frameTime, ScorchedContext &context);

protected:
	struct EventEntry
	{
		float eventTime;
		int eventNumber;
	};

	std::map<LandscapeEvent *, EventEntry> events_;

	void addEvent(ScorchedContext &context, 
		std::vector<LandscapeEvent *> &events);
	void addEvents(ScorchedContext &context, 
		std::vector<LandscapeEvents *> &events);
};

#endif // __INCLUDE_EventContainerh_INCLUDE__
