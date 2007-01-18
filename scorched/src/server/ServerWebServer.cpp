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

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <server/ServerWebServer.h>
#include <server/ServerWebHandler.h>
#include <server/ServerWebSettingsHandler.h>
#include <server/ServerCommon.h>
#include <server/ServerAdminHandler.h>
#include <server/ScorchedServer.h>
#include <net/NetMessagePool.h>
#include <common/OptionsGame.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <common/Defines.h>

ServerWebServer *ServerWebServer::instance_ = 0;

ServerWebServer *ServerWebServer::instance()
{
	if (!instance_)
	{
		instance_ = new ServerWebServer();
	}
	return instance_;
}

ServerWebServer::ServerWebServer() : 
	netServer_(new NetServerHTTPProtocolRecv),
	logger_(0)
{
	addRequestHandler("/players", new ServerWebHandler::PlayerHandler());
	addRequestHandler("/logs", new ServerWebHandler::LogHandler());
	addRequestHandler("/logfile", new ServerWebHandler::LogFileHandler());
	addRequestHandler("/game", new ServerWebHandler::GameHandler());
	addRequestHandler("/server", new ServerWebHandler::ServerHandler());
	addRequestHandler("/talk", new ServerWebHandler::TalkHandler());
	addRequestHandler("/banned", new ServerWebHandler::BannedHandler());
	addRequestHandler("/mods", new ServerWebHandler::ModsHandler());
	addRequestHandler("/sessions", new ServerWebHandler::SessionsHandler());
	addRequestHandler("/settingsall", new ServerWebSettingsHandler::SettingsAllHandler());
	addRequestHandler("/settingslandscape", new ServerWebSettingsHandler::SettingsLandscapeHandler());
	addRequestHandler("/settingsplayers", new ServerWebSettingsHandler::SettingsPlayersHandler());
}

ServerWebServer::~ServerWebServer()
{
}

void ServerWebServer::start(int port)
{
	Logger::log(formatString("Starting management web server on port %i", port));
	netServer_.setMessageHandler(this);
	netServer_.start(port);

	if (0 != strcmp(ScorchedServer::instance()->getOptionsGame().
		getServerFileLogger(), "none"))
	{
		logger_ = new FileLogger(
			formatString("ServerWeb-%i-",
				ScorchedServer::instance()->getOptionsGame().getPortNo()));
	}
}

void ServerWebServer::addRequestHandler(const char *url,
	ServerWebServerI *handler)
{
	handlers_[url] = handler;
}

void ServerWebServer::processMessages()
{
	// Check if any delayed messages should be sent
	while (!delayedMessages_.empty())
	{
		unsigned int theTime = (unsigned int) time(0);
		std::pair<unsigned int, NetMessage *> &delayedMessage =
			delayedMessages_.front();
		if (delayedMessage.first <= theTime)
		{
			// Get the message
			NetMessage *message = delayedMessage.second;
			delayedMessages_.pop_front();

			// Send this message now
			netServer_.sendMessageDest(message->getBuffer(), message->getDestinationId());
			NetMessagePool::instance()->addToPool(message);
		}
		else break;
	}

	// Check if any non-delayed messages should be processed
	netServer_.processMessages();
}

static void extractQueryFields(std::map<std::string, std::string> &fields, char *sep)
{
	char *token = strtok(sep, "&");
	while(token)
	{
		char *eq = strchr(token, '=');
		if (eq)
		{
			*eq = '\0';
			std::string value;
			for (const char *valueStr = (eq + 1); *valueStr; valueStr++)
			{
				char c = *valueStr;
				if (c == '+') c = ' ';
				else if (c == '%')
				{
					char buf[3] = { 0, 0, 0 };

					buf[0] = *(valueStr + 1);
					if (!buf[0]) break;
					buf[1] = *(valueStr + 2);
					if (!buf[1]) break;

					c = (char) strtol(buf, 0, 16);

					valueStr += 2;
				}

				if (c != '\r') value += c;
			}

			if (fields.find(token) == fields.end())
			{
				fields[token] = value;
			}
			*eq = '=';
		}				
		token = strtok(0, "&");
	}
}

void ServerWebServer::processMessage(NetMessage &message)
{
	if (message.getMessageType() == NetMessage::BufferMessage)
	{
		// We have received a request for the web server
		message.getBuffer().addToBuffer("");
		const char *buffer = message.getBuffer().getBuffer();
		
		// Check it is a GET
		bool ok = false;
		bool get = (strstr(buffer, "GET ") == buffer);
		bool post = (strstr(buffer, "POST ") == buffer);
		if (get || post)
		{
			std::map<std::string, std::string> fields;
		
			// Get POST query fields if any
			if (post)
			{
				char *sep = (char *) strstr(buffer, "\r\n\r\n");
				if (sep)
				{
					sep += 4;
					extractQueryFields(fields, sep);
				}
			}
		
			// Check it has a url
			const char *url = buffer + (get?4:5);
			char *eol = (char *) strchr(url, ' ');
			if (eol)
			{
				*eol = '\0';
				if (*url)
				{
					// Get GET query fields if any
					char *sep = (char *) strchr(url, '?');
					if (sep)
					{
						*sep = '\0'; sep++;
						extractQueryFields(fields, sep);
					}

					// Add ip address into fields
					fields["ipaddress"] = 
						NetInterface::getIpName(message.getIpAddress());

					// Log info
					if (logger_)
					{
						time_t t = time(0);
						std::string f;
						std::map<std::string, std::string>::iterator itor;
						for (itor = fields.begin();
							itor != fields.end();
							itor++)
						{
							if (0 != strcmp((*itor).first.c_str(), "password"))
							{
								f += formatString("%s=%s ",
									(*itor).first.c_str(),
									(*itor).second.c_str());
							}
						}

						std::string username;
						if (fields.find("sid") != fields.end())
						{
							unsigned int sid = (unsigned int) atoi(fields["sid"].c_str());
							std::map <unsigned int, SessionParams>::iterator findItor =
								sessions_.find(sid);
							if (findItor != sessions_.end())
							{
								username = (*findItor).second.userName;
							}
						}

						LoggerInfo info(LoggerInfo::TypeNormal,
							formatString("%u %s http://%s [%s]", 
							message.getDestinationId(), 
							username.c_str(), url, f.c_str()),
							ctime(&t));
						logger_->logMessage(info);
					}
					
					// Process request
					const char *ipaddress = NetInterface::getIpName(message.getIpAddress());
					ok = processRequest(message.getDestinationId(), ipaddress, url, fields);
				}
			}
		}

		if (!ok)
		{
			// Disconnect the client
			netServer_.disconnectClient(message.getDestinationId());
		}
	}
	else if (message.getMessageType() == NetMessage::SentMessage)
	{
		netServer_.disconnectClient(message.getDestinationId());
	}
}

bool ServerWebServer::processRequest(
	unsigned int destinationId,
	const char *ip,
	const char *url,
	std::map<std::string, std::string> &fields)
{
	bool delayed = false; // Set delayed on authentication failure
	std::string text;
	if (0 == strcmp(url, "/"))
	{
		// We have requested the login page
		// Have the login credentials been supplied
		if (validateUser(ip, url, fields))
		{
			// Yes, and credentials are correct
			// Show the starting (players) page
			getHtmlRedirect(formatString("/players?sid=%s", fields["sid"].c_str()), text);
		}
		else
		{
			// No, or credentials are not correct
			// Show the login page after a delay
			if (!getHtmlTemplate("login.html", fields, text)) return false;
			delayed = true;
		}
	}
	else
	{
		// A "normal" page has been requested
		// Check the session is valid
		if (validateSession(ip, url, fields))
		{
			// The session is valid, show the page
			if (!generatePage(url, fields, text)) return false;
		}
		else
		{
			// The session is invalid show the login page after a delay
			getHtmlRedirect("/", text);
			delayed = true;
		}
	}

	// Generate the message to send
	NetMessage *message = NetMessagePool::instance()->getFromPool(
		NetMessage::BufferMessage, destinationId, 0, 0);
	message->getBuffer().addDataToBuffer(text.c_str(), text.size()); // No null
	if (delayed)
	{
		// Generate an outgoing message, that will be sent after a time delay
		unsigned int delayedTime = (unsigned int) time(0) + 5;
		std::pair<unsigned int, NetMessage *> delayedMessage(delayedTime, message);
		delayedMessages_.push_back(delayedMessage);
	}
	else
	{
		// Send this message now
		netServer_.sendMessageDest(message->getBuffer(), message->getDestinationId());
		NetMessagePool::instance()->addToPool(message);
	}
	
	return true;
}

bool ServerWebServer::validateSession(
	const char *ip,
	const char *url,
	std::map<std::string, std::string> &fields)
{
	const unsigned int SessionTimeOut = 60 * 15;
	unsigned int currentTime = (unsigned int) time(0);

	// Tidy expired sessions
	std::map <unsigned int, SessionParams>::iterator sitor;
	for (sitor = sessions_.begin();
		sitor != sessions_.end();
		sitor++)
	{
		SessionParams &params = (*sitor).second;
		if (currentTime > params.sessionTime + SessionTimeOut)
		{
			sessions_.erase(sitor);
			break;
		}
	}

	// Check if user has a valid session
	if (fields.find("sid") != fields.end())
	{
		unsigned int sid = (unsigned int) atoi(fields["sid"].c_str());
		std::map <unsigned int, SessionParams>::iterator findItor =
			sessions_.find(sid);
		if (findItor != sessions_.end())
		{
			SessionParams &params = (*findItor).second;
			if (currentTime < params.sessionTime + SessionTimeOut)
			{
				params.sessionTime = currentTime;
				return true;
			}
			else
			{
				sessions_.erase(findItor);
			}
		}
	}

	return false;
}

bool ServerWebServer::validateUser(
	const char *ip,
	const char *url,
	std::map<std::string, std::string> &fields)
{
	unsigned int currentTime = (unsigned int) time(0);
	bool authenticated = false;

	// Check if this is a local user
	if (0 == strcmp(ip, "127.0.0.1"))
	{
		fields["name"] = "localconnection";
		authenticated = true;
	}
	else
	{
		// Check if user has a valid username and password
		if (fields.find("name") != fields.end() ||
			fields.find("password") != fields.end()) 
		{
			if (ServerAdminHandler::instance()->login(
				fields["name"].c_str(),
				fields["password"].c_str()))
			{
				authenticated = true;
			}
		}
	}

	// Create a session for the authenticated user
	if (authenticated)
	{
		SessionParams params;
		params.sessionTime = currentTime;
		params.userName = fields["name"];
		params.ipAddress = ip;

		// Generate a new unique session id
		unsigned int sid = 0;
		do 
		{
			sid = rand();
		} while (sessions_.find(sid) != sessions_.end());
		
		sessions_[sid] = params;
		fields["sid"] = formatString("%u", sid);

		ServerCommon::sendString(0,
			formatString("server admin \"%s\" logged in",
			fields["name"].c_str()));
	}

	return authenticated;
}

bool ServerWebServer::generatePage(
	const char *url,
	std::map<std::string, std::string> &fields,
	std::string &text)
{
	std::map <std::string, ServerWebServerI *>::iterator itor =
		handlers_.find(url);
	if (itor == handlers_.end()) return false;

	ServerWebServerI *handler = (*itor).second;
	return handler->processRequest(url, fields, text);
}

void ServerWebServer::getHtmlRedirect(
	const char *url,
	std::string &result)
{
	const char *header = 
		formatString(
		"HTTP/1.1 302 OK\r\n"
		"Server: Scorched3D\r\n"
		"Content-Type: text/html\r\n"
		"Connection: Close\r\n"
		"Location: %s\r\n"
		"\r\n", url);
	result.append(header);
}

bool ServerWebServer::getHtmlTemplate(
	const char *name,
	std::map<std::string, std::string> &fields,
	std::string &result)
{
	const char *header = 
		"HTTP/1.1 200 OK\r\n"
		"Server: Scorched3D\r\n"
		"Content-Type: text/html\r\n"
		"Connection: Close\r\n"
		"\r\n";
	result.append(header);

	return getTemplate(name, fields, result);
}

bool ServerWebServer::getTemplate(
	const char *name,
	std::map<std::string, std::string> &fields,
	std::string &result)
{
	// Perhaps cache this
	const char *fileName = getDataFile(formatString("data/html/server/%s", name));
	FILE *in = fopen(fileName, "r");
	if (!in) 
	{
		Logger::log(formatString("ERROR: Failed to open web template \"%s\"", fileName));
		return false;
	}

	char buffer[1024], include[256];
	while (fgets(buffer, 1024, in))
	{
		// Check for an include line
		if (sscanf(buffer, "#include %s",
			include) == 1)
		{
			// Add the included file
			std::string tmp;
			if (!getTemplate(include, fields, tmp))
			{
				return false;
			}

			result += tmp;
		}
		else
		{
			// Check for any value replacements
			char *position = buffer;
			for (;;)
			{
				char *start, *end;
				if ((start = strstr(position, "[[")) &&
					(end = strstr(position, "]]")) &&
					(end > start))
				{
					// Replace the text [[name]] with the value
					*start = '\0';
					*end = '\0';
					result += position;
					position = end + 2;

					char *name = start + 2;

					// First check to see if it is in the supplied fields
					if (fields.find(name) != fields.end())
					{
						result += fields[name];
					}
					else
					{
						// Then in the scorched3d settings
						std::list<OptionEntry *>::iterator itor;
						std::list<OptionEntry *> &options = 
							ScorchedServer::instance()->getOptionsGame().getOptions();
						for (itor = options.begin();
							itor != options.end();
							itor++)
						{
							OptionEntry *entry = (*itor);
							if (!(entry->getData() & OptionEntry::DataProtected))
							{
								if (strcmp(entry->getName(), name) == 0)
								{
									result += entry->getValueAsString();
								}
								else
								{
									std::string newName(entry->getName());
									newName.append("_set");
									if (strcmp(newName.c_str(), name) == 0)
									{
										std::string value;
										ServerWebSettingsHandler::generateSettingValue(entry,value);
										result += value;
									}
								}
							}
						}						
					}
				}
				else
				{
					// No replacements
					result += position;
					break;
				}
			}
		}
	}
	fclose(in);

	return true;
}

