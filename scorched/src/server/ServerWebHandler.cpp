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

#include <server/ServerWebHandler.h>
#include <server/ServerLog.h>
#include <server/ScorchedServer.h>
#include <server/ScorchedServerUtil.h>
#include <server/ServerCommon.h>
#include <server/ServerTextHandler.h>
#include <server/ServerState.h>
#include <server/ServerParams.h>
#include <landscapedef/LandscapeDefinitionsBase.h>
#include <engine/ModFiles.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/OptionsGame.h>
#include <common/OptionsTransient.h>
#include <common/StatsLogger.h>
#include <common/FileList.h>
#include <net/NetInterface.h>
#include <tank/TankContainer.h>
#include <tank/TankColorGenerator.h>
#include <tank/TankState.h>
#include <tank/TankScore.h>
#include <tankai/TankAIStore.h>
#include <tankai/TankAIAdder.h>
#include <XML/XMLParser.h>
#include <vector>
#include <algorithm>

struct LogFile
{
	std::string fileName;
	time_t fileTime;
};

static inline bool lt_logfile(const LogFile &o1, const LogFile &o2) 
{ 
	return o1.fileTime < o2.fileTime;
}

static const char *getField(std::map<std::string, std::string> &fields, const char *field)
{
	std::map<std::string, std::string>::iterator itor = 
		fields.find(field);
	if (itor != fields.end())
	{
		return (*itor).second.c_str();
	}
	return 0;
}

static std::string getFile(const char *filename)
{
	char buffer[100];
	std::string file;
	FILE *in = fopen(filename, "r");
	if (in)
	{
		while (fgets(buffer, 100, in))
		{
			file += buffer;
		}
		fclose(in);
	}
	return file;
}

static std::string concatLines(std::list<std::string> &lines)
{
	std::string result;
	std::list<std::string>::iterator itor;
	for (itor = lines.begin();
		itor != lines.end();
		itor++)
	{
		result.append(*itor).append("<br>");

	}
	return result;
}

bool ServerWebHandler::PlayerHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	// Check for an add
	const char *addType = getField(fields, "add");
	if (addType)
	{
		TankAIAdder::addTankAI(*ScorchedServer::instance(), addType);
	}

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getAllTanks();
	std::map<unsigned int, Tank *>::iterator itor;

	// Check for any action
	const char *action = getField(fields, "action");
	if (action)
	{
		for (itor = tanks.begin(); itor != tanks.end(); itor++)
		{
			// Is this tank selected
			Tank *tank = (*itor).second;
			if (getField(fields, formatString("player-%u", tank->getPlayerId())))
			{
				if (0 == strcmp(action, "Kick"))
				{
					ServerCommon::kickPlayer(tank->getPlayerId());
					break;
				}
				else if (0 == strcmp(action, "Mute"))
				{
					tank->getState().setMuted(true);
				}
				else if (0 == strcmp(action, "UnMute"))
				{
					tank->getState().setMuted(false);
				}
				else if (0 == strcmp(action, "Flag"))
				{
					ServerCommon::banPlayer(tank->getPlayerId(), ServerBanned::Flagged);
				}
				else if (0 == strcmp(action, "Poor"))
				{
					ServerCommon::poorPlayer(tank->getPlayerId());
				}
				else if (0 == strcmp(action, "PermMute"))
				{
					ServerCommon::banPlayer(tank->getPlayerId(), ServerBanned::Muted);
				}
				else if (0 == strcmp(action, "UnPermMute"))
				{
					ServerCommon::banPlayer(tank->getPlayerId(), ServerBanned::NotBanned);
				}
				else if (0 == strcmp(action, "Banned"))
				{
					ServerCommon::banPlayer(tank->getPlayerId(), ServerBanned::Banned);
				}
				else if (0 == strcmp(action, "Slap"))
				{
					ServerCommon::slapPlayer(tank->getPlayerId(), 25.0f);
				}
				else if (0 == strcmp(action, "ShowAliases"))
				{
					std::list<std::string> aliases =
						StatsLogger::instance()->getAliases(tank);
					std::string lines = concatLines(aliases);
					return ServerWebServer::getHtmlMessage(
						"ShowAliases", lines.c_str(), fields, text);
				}
				else if (0 == strcmp(action, "ShowIPAliases"))
				{
					std::list<std::string> aliases =
						StatsLogger::instance()->getIpAliases(tank);
					std::string lines = concatLines(aliases);
					return ServerWebServer::getHtmlMessage(
						"ShowIPAliases", lines.c_str(), fields, text);
				}
				else
				{
					Logger::log(formatString("Unknown web handler player action \"%s\"", action));
				}
			}
		}
	}

	// Player Entries
	std::string players;
	for (itor = tanks.begin(); itor != tanks.end(); itor++)
	{
		Tank *tank = (*itor).second;
		std::string cleanName;
		std::string dirtyName(tank->getName());
		XMLNode::removeSpecialChars(dirtyName, cleanName);
		players += formatString(
			"<tr>"
			"<td>dest=%i ip=%s id=%i</td>" // Id
			"<td>%s</td>" // Name
			"<td>%s</td>" // Type
			"<td>%s</td>" // Time
			"<td>%s</td>" // Score
			"<td>%s</td>" // State
			"<td>%s</td>" // Team
			"<td><input type=\"checkbox\" name=\"player-%u\"></td>" // Select
			"</tr>\n",
			tank->getDestinationId(), NetInterface::getIpName(tank->getIpAddress()), tank->getPlayerId(),
			cleanName.c_str(),
			tank->getTankAI()?tank->getTankAI()->getName():"Human",
			tank->getScore().getTimePlayedString(),
			tank->getScore().getScoreString(),
			tank->getState().getStateString(),
			TankColorGenerator::getTeamName(tank->getTeam()),
			tank->getPlayerId()
		);
	}
	fields["PLAYERS"] = players;
	fields["NOPLAYERS"] = formatString("%i/%i", tanks.size(), 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers());

	// Add entries
	std::string add;
	std::list<TankAI *> &ais = ScorchedServer::instance()->getTankAIs().getAis();
	std::list<TankAI *>::iterator aiitor;
	for (aiitor = ais.begin();
		aiitor != ais.end();
		aiitor++)
	{
		TankAI *ai = (*aiitor);
		if (ai->availableForPlayers())
		{
			add += formatString(
				"<input type=\"submit\" name=\"add\" value=\"%s\"\n>",
				ai->getName());
		}
	}
	fields["ADD"] = add;

	return ServerWebServer::getHtmlTemplate("player.html", fields, text);
}

bool ServerWebHandler::LogHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	std::deque<ServerLog::ServerLogEntry> &entries = 
		ServerLog::instance()->getEntries();

	const int pagesize = 20;
	int start = (int(entries.size()) / pagesize) * pagesize;
	const char *page = getField(fields, "page");
	if (page) start = atoi(page);
	else start = entries.size() - pagesize;

	// Log entries
	std::string log;

	int min = MAX(start, 0);
	int max = MIN((int) entries.size(), start + pagesize);
	for (int i=min; i<max; i++)
	{
		std::string cleanText;
		XMLNode::removeSpecialChars(entries[i].text, cleanText);
		log += formatString(
			"<tr>"
			"<td><font size=-1>%u</font></td>"
			"<td><font size=-1>%s</font></td>"
			"</tr>\n",
			i,
			cleanText.c_str());
	}
	fields["LOG"] = log;

	// Pages
	std::string pages;
	for (int i =0; i<=int(entries.size())/pagesize-1; i++)	
	{
		pages +=
			formatString("<a href='?sid=%s&page=%i'>%i - %i</a>&nbsp;",
				fields["sid"].c_str(), 
				i * pagesize, i * pagesize, i * pagesize + pagesize - 1);
	}
	pages +=
		formatString("<a href='?sid=%s'>Last</a>",
			fields["sid"].c_str());
	fields["PAGES"] = pages;

        const char *refreshRate = getField(fields, "RefreshRate");
        int refreshSeconds = 0;
        if (refreshRate) refreshSeconds = atoi(refreshRate);
        else fields["RefreshRate"]="0";

	if (refreshSeconds > 0)
        fields["Meta"] = formatString("<meta  HTTP-EQUIV=\"Refresh\" CONTENT=\"%d;URL=%s?sid=%s&RefreshRate=%s\">", refreshSeconds, url,fields["sid"].c_str(),refreshRate);

	return ServerWebServer::getHtmlTemplate("log.html", fields, text);
}

bool ServerWebHandler::LogFileHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	std::deque<ServerLog::ServerLogEntry> &entries = 
		ServerLog::instance()->getEntries();

	const char *logFilename = getField(fields, "filename");
	if (logFilename &&
		// Disallow directory backtracking
		!strstr(logFilename, "..") &&
		!strstr(logFilename, ":") &&
		!strstr(logFilename, "/") &&
		!strstr(logFilename, "\\"))
	{
		// We've requested to view a log file
		// So load the file and display it
		text.append(formatString(
			"HTTP/1.1 200 OK\r\n"
			"Server: Scorched3D\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Disposition: filename=%s.txt\r\n"
			"Connection: Close\r\n"
			"\r\n", logFilename));

		const char *filename = getLogFile(logFilename);
		std::string file = getFile(filename);
		fields["FILE"] = file;

		return ServerWebServer::getTemplate("logfile.html", fields, text);
	}
	else
	{
		// We've requested to see the list of current log files

		// Check to see if we want to search these files
		const char *search = 0;
		const char *action = getField(fields, "action");
		if (action && 0 == strcmp(action, "Search"))
		{
			search = getField(fields, "search");
		}

		// Iterator through all of the log files in the logs directory
		std::vector<LogFile> logFiles;
		const char *dirName = getLogFile("");
		FileList dir(dirName, "*.log", false);
		if (dir.getStatus())
		{
			std::list<std::string> &files = dir.getFiles();
			std::list<std::string>::iterator itor;
			for (itor = files.begin();
				itor!= files.end();
				itor++)
			{
				const char *fileName = (*itor).c_str();
				const char *fullFilename = getLogFile(fileName);

				// If searching is enabled check to see if this file contains 
				// the specified string
				if (search)
				{
					std::string file = getFile(fullFilename);
					if (0 == strstr(file.c_str(), search)) continue;
				}

				// Add this file to the list of files to view
				LogFile logFile;
				logFile.fileName = fileName;
				logFile.fileTime = s3d_fileModTime(fullFilename);
				logFiles.push_back(logFile);
			}
		}

		// Sort the list of files to view
		std::sort(logFiles.begin(), logFiles.end(), lt_logfile);

		// Create the table that will show the list of log files
		std::string log;
		{
			std::vector<LogFile>::iterator itor;
			for (itor = logFiles.begin();
				itor != logFiles.end();
				itor++)
			{
				LogFile &logFile = *itor;
				const char *fileName = logFile.fileName.c_str();
				log += formatString(
					"<tr>"
					"<td><font size=-1><a href=?filename=%s&sid=%s>%s</a></font></td>"
					"<td><font size=-1>%s</font></td>"
					"</tr>\n",
					fileName,
					fields["sid"].c_str(),
					fileName,
					ctime(&logFile.fileTime));
			}
		}
		fields["LOG"] = log;

		return ServerWebServer::getHtmlTemplate("logfiles.html", fields, text);
	}
	return false;
}

bool ServerWebHandler::GameHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	// Check for any action
	const char *action = getField(fields, "action");
	if (action)
	{
		if (0 == strcmp(action, "NewGame"))
		{
			ServerCommon::startNewGame();
		}
		else if (0 == strcmp(action, "KillAll"))
		{
			ServerCommon::killAll();
		}
	}

	std::map<unsigned int, Tank *> &tanks = 
		ScorchedServer::instance()->getTankContainer().getPlayingTanks();
	fields["PLAYERS"] = formatString("%i/%i", tanks.size(), 
		ScorchedServer::instance()->getOptionsGame().getNoMaxPlayers());

	unsigned int state = ScorchedServer::instance()->getGameState().getState();
	fields["STATE"] = ((state == ServerState::ServerStateTooFewPlayers)?"Not Playing":"Playing");

	fields["ROUND"] = formatString("%i/%i",
		ScorchedServer::instance()->getOptionsTransient().getCurrentRoundNo(),
		ScorchedServer::instance()->getOptionsGame().getNoRounds());
	fields["MOVE"] = formatString("%i/%i",
		ScorchedServer::instance()->getOptionsTransient().getCurrentGameNo(),
		ScorchedServer::instance()->getOptionsGame().getNoMaxRoundTurns());
	
	fields["BI"] = formatString("%uK", NetInterface::getBytesIn() / 1000);
	fields["BO"] = formatString("%uK", NetInterface::getBytesOut() / 1000);
	fields["P"] = formatString("%u", NetInterface::getPings());
	fields["C"] = formatString("%u", NetInterface::getConnects());

	return ServerWebServer::getHtmlTemplate("game.html", fields, text);
}

bool ServerWebHandler::ServerHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	// Check for any action
	const char *action = getField(fields, "action");
	if (action)
	{
		if (0 == strcmp(action, "Stop Server"))
		{
			exit(0);
		}
		else if (0 == strcmp(action, "Stop Server When Empty"))
		{
			ServerCommon::getExitEmpty() = true;
		}
	}

	unsigned int state = ScorchedServer::instance()->getGameState().getState();
	fields["STATE"] = ((state == ServerState::ServerStateTooFewPlayers)?"Not Playing":"Playing");
	fields["VERSION"] = formatString("%s (%s) - Built %s", 
		ScorchedVersion, ScorchedProtocolVersion, ScorchedBuildTime);
	fields["STARTTIME"] = getStartTime();
	fields["EXITEMPTY"] = (ServerCommon::getExitEmpty()?"True":"False");

	return ServerWebServer::getHtmlTemplate("server.html", fields, text);
}

bool ServerWebHandler::TalkHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	const char *say = getField(fields, "text");
	if (say)
	{
		const char *type = getField(fields, "type");
		if (!type || 0 == strcmp(type, "all"))
		{
			ServerCommon::sendString(0, say);
		}
		else if (0 == strcmp(type, "message"))
		{
			ServerCommon::sendStringMessage(0, say);
		}
		else if (0 == strcmp(type, "admin"))
		{
			ServerCommon::sendStringAdmin(say);
		}
	}

	std::string texts;
	std::list<std::string> &textsList = 
		ServerTextHandler::instance()->getLastMessages();
	std::list<std::string>::iterator textsListItor;
	for (textsListItor = textsList.begin();
		textsListItor != textsList.end();
		textsListItor++)
	{
		std::string cleanText;
		XMLNode::removeSpecialChars((*textsListItor), cleanText);
		texts += cleanText;
		texts += "<br>\n";
	}
	fields["TEXTS"] = texts;
	const char *refreshRate = getField(fields, "RefreshRate");
	int refreshSeconds = 0;
	if (refreshRate) refreshSeconds = atoi(refreshRate);
	else fields["RefreshRate"]="0";
	if (refreshSeconds > 0)
		fields["Meta"] = formatString("<meta  HTTP-EQUIV=\"Refresh\" CONTENT=\"%d;URL=%s?sid=%s&RefreshRate=%s\">", refreshSeconds, url,fields["sid"].c_str(),refreshRate);
	return ServerWebServer::getHtmlTemplate("talk.html", fields, text);
}

bool ServerWebHandler::BannedHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	const char *action = getField(fields, "action");
	if (action && 0 == strcmp(action, "Load")) 
		ScorchedServerUtil::instance()->bannedPlayers.load(true);

	const char *selected = getField(fields, "selected");
	std::string banned;
	std::list<ServerBanned::BannedRange> &bannedIps = 
		ScorchedServerUtil::instance()->bannedPlayers.getBannedIps();
	std::list<ServerBanned::BannedRange>::iterator itor;
	for (itor = bannedIps.begin();
		itor != bannedIps.end();
		itor++)
	{
		ServerBanned::BannedRange &range = (*itor);
		std::string mask = NetInterface::getIpName(range.mask);

		std::map<unsigned int, ServerBanned::BannedEntry>::iterator ipitor;
		for (ipitor = range.ips.begin();
			ipitor != range.ips.end();
			ipitor++)
		{
			unsigned int ip = (*ipitor).first;
			ServerBanned::BannedEntry &entry = (*ipitor).second;
			std::string ipName = NetInterface::getIpName(ip);

			if (selected && 0 == strcmp(selected, ipName.c_str()))
				entry.type = ServerBanned::NotBanned;

			std::string cleanName;
			XMLNode::removeSpecialChars(entry.name, cleanName);
			banned += formatString("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td>"
				"<td><input type=\"checkbox\" name=\"selected\" value=\"%s\"></td>" // Select
				"</tr>",
				(entry.bantime?ctime(&entry.bantime):""),
				cleanName.c_str(),
				entry.uniqueid.c_str(),
				ServerBanned::getBannedTypeStr(entry.type),
				ipName.c_str(), mask.c_str(),
				ipName.c_str());
		}
	}
	fields["BANNED"] = banned;

	if (action && 0 == strcmp(action, "Save")) 
		ScorchedServerUtil::instance()->bannedPlayers.save();

	return ServerWebServer::getHtmlTemplate("banned.html", fields, text);
}

bool ServerWebHandler::ModsHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	std::string modfiles;
	std::map<std::string, ModFileEntry *> &modFiles = 
		ScorchedServer::instance()->getModFiles().getFiles();
	std::map<std::string, ModFileEntry *>::iterator itor;
	for (itor = modFiles.begin();
		itor != modFiles.end();
		itor++)
	{
		ModFileEntry *entry = (*itor).second;
		std::string cleanFileName;
		std::string dirtyFileName(entry->getFileName());
		XMLNode::removeSpecialChars(dirtyFileName, cleanFileName);
		modfiles += formatString("<tr><td>%s</td><td>%u</td><td>%u</td><td>%u</td></tr>",
			cleanFileName.c_str(),
			entry->getCompressedSize(),
			entry->getCompressedCrc(),
			entry->getUncompressedSize());
	}
	fields["MODFILES"] = modfiles;

	return ServerWebServer::getHtmlTemplate("mods.html", fields, text);
}

bool ServerWebHandler::SessionsHandler::processRequest(const char *url,
	std::map<std::string, std::string> &fields,
	std::map<std::string, NetMessage *> &parts,
	std::string &text)
{
	// Sessions Entries
	std::string sessions;
	std::map<unsigned int, ServerWebServer::SessionParams> &sessionparams = 
		ServerWebServer::instance()->getSessions();
	std::map<unsigned int, ServerWebServer::SessionParams>::iterator itor;
	for (itor = sessionparams.begin(); itor != sessionparams.end(); itor++)
	{
		ServerWebServer::SessionParams &params = (*itor).second;

		const char *timeStr = ctime((const time_t *) (&params.sessionTime));
		sessions += formatString(
			"<tr>"
			"<td>%s</td>" // Name
			"<td>%s</td>" // Time
			"<td>%s</td>" // Ip
			"</tr>\n",
			params.userName.c_str(),
			timeStr,
			params.ipAddress.c_str()
		);
	}
	fields["SESSIONS"] = sessions;

	return ServerWebServer::getHtmlTemplate("sessions.html", fields, text);
}
