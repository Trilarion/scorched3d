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

#include <engine/EventContainer.h>
#include <engine/ScorchedContext.h>
#include <landscapedef/LandscapeDefn.h>
#include <landscapedef/LandscapeTex.h>
#include <landscapedef/LandscapeEvents.h>
#include <landscapemap/LandscapeMaps.h>

EventContainer::EventContainer()
{
}

EventContainer::~EventContainer()
{
}

void EventContainer::clear()
{
	events_.clear();
}

void EventContainer::initialize(ScorchedContext &context)
{
	clear();

	LandscapeTex &tex = 
		*context.landscapeMaps->getDefinitions().getTex();
	LandscapeDefn &defn = 
		*context.landscapeMaps->getDefinitions().getDefn();

	addEvents(context, tex.texDefn.includes);
	addEvents(context, defn.texDefn.includes);
}

void EventContainer::addEvents(ScorchedContext &context, 
	std::vector<LandscapeInclude *> &events)
{
	std::vector<LandscapeInclude *>::iterator itor;
	for (itor = events.begin();
		itor != events.end();
		itor++)
	{
		LandscapeInclude *event = (*itor);
		addEvent(context, event->events);
	}
}

void EventContainer::addEvent(ScorchedContext &context, 
	std::vector<LandscapeEvent *> &events)
{
	std::vector<LandscapeEvent *>::iterator itor;
	for (itor = events.begin();
		itor != events.end();
		itor++)
	{
		LandscapeEvent *event = (*itor);

		EventEntry entry;
		entry.eventNumber = 0;
		entry.eventTime = 
			event->condition->getNextEventTime(context, ++entry.eventNumber);
		entry.event = event;
		events_.push_back(entry);
	}
}

void EventContainer::simulate(fixed frameTime, ScorchedContext &context)
{
	std::vector<EventEntry>::iterator itor;
	for (itor = events_.begin();
		itor != events_.end();
		itor++)
	{
		EventEntry &entry = (*itor);
		
		LandscapeEvent *event = entry.event;
		entry.eventTime -= frameTime;
		if (event->condition->fireEvent(context, 
			entry.eventTime, entry.eventNumber))
		{
			event->action->fireAction(context);
			entry.eventTime = 
				event->condition->getNextEventTime(
					context, ++entry.eventNumber);
		}
	}	
}
