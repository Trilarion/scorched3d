#include <XML/XMLParser.h>
#include <common/Defines.h>
#include <stdio.h>

XMLNode::XMLNode(const char *name, XMLNode *parent, NodeType type) : 
	name_(name), parent_(parent), type_(type)
{
}

XMLNode::~XMLNode()
{
	while (!children_.empty())
	{
		XMLNode *node = children_.front();
		children_.pop_front();
		delete node;
	}
	while (!parameters_.empty())
	{
		XMLNode *node = parameters_.front();
		parameters_.pop_front();
		delete node;
	}
}

XMLNode *XMLNode::getNamedChild(const char *name)
{
	std::list<XMLNode *>::iterator itor;
	for (itor = children_.begin();
		itor != children_.end();
		itor++)
	{
		XMLNode *node = (*itor);
		if (strcmp(name, node->getName()) == 0) return node;
	}
	return 0;
}

XMLNode *XMLNode::getNamedParameter(const char *name)
{
	std::list<XMLNode *>::iterator itor;
	for (itor = parameters_.begin();
		itor != parameters_.end();
		itor++)
	{
		XMLNode *node = (*itor);
		if (strcmp(name, node->getName()) == 0) return node;
	}
	return 0;
}

XMLParser::XMLParser() : root_(0), current_(0)
{
	// Init the XML parser
	p_ = XML_ParserCreate(0);
	XML_SetStartElementHandler(p_, startElementStaticHandler);
	XML_SetEndElementHandler(p_, endElementStaticHandler);
	XML_SetCharacterDataHandler(p_, characterDataStaticHandler);
	XML_SetUserData(p_, this);
}

XMLParser::~XMLParser()
{
	// Cleanup
	XML_ParserFree(p_);
	delete root_;
}

bool XMLParser::parse(const char *data, int len, int final)
{
	// Parser the XML
	XML_Status status;
	status = XML_Parse(p_, data, len, final);
	return (status == XML_STATUS_OK);
}

const char *XMLParser::getParseError()
{
	XML_Error errorCode = XML_GetErrorCode(p_);

	static char message[1024];
	sprintf(message, "Parse Error, Line:%i Col:%i Error:%s",
		XML_GetCurrentLineNumber(p_),
		XML_GetCurrentColumnNumber(p_),
		XML_ErrorString(errorCode));
	return message;
}

void XMLParser::startElementHandler(const XML_Char *name,
                           const XML_Char **atts)
{
	if (!root_)
	{
		// Create the root node
		root_ = current_ = new XMLNode(name, 0);
	}
	else
	{
		DIALOG_ASSERT(current_);

		// Add a new child to this node
		XMLNode *newNode = new XMLNode(name, current_);
		current_->addChild(newNode);
		current_ = newNode;
	}

	if (atts)
	{
		while (*atts)
		{
			const XML_Char *name = *atts;
			atts++;
			const XML_Char *value = *atts;
			atts++;

			XMLNode *param = new XMLNode(name, current_, XMLNode::XMLParameterType);
			param->addContent(value, strlen(value));
			current_->getParameters().push_back(param);
		}
	}
}

void XMLParser::endElementHandler(const XML_Char *name)
{
	DIALOG_ASSERT(current_);
	DIALOG_ASSERT(strcmp(name, current_->getName()) == 0);

	current_ = (XMLNode *) current_->getParent();
}

void XMLParser::characterDataHandler(const XML_Char *s,
                            int len)
{
	current_->addContent(s, len);
}

void XMLParser::startElementStaticHandler(void *userData,
                           const XML_Char *name,
                           const XML_Char **atts)
{
	XMLParser *parser = (XMLParser *) userData;
	parser->startElementHandler(name, atts);
}

void XMLParser::endElementStaticHandler(void *userData,
                         const XML_Char *name)
{
	XMLParser *parser = (XMLParser *) userData;
	parser->endElementHandler(name);
}

void XMLParser::characterDataStaticHandler(void *userData,
                            const XML_Char *s,
                            int len)
{
	XMLParser *parser = (XMLParser *) userData;
	parser->characterDataHandler(s, len);
}
