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


#if !defined(__INCLUDE_OptionEntryh_INCLUDE__)
#define __INCLUDE_OptionEntryh_INCLUDE__

#include <common/FileLines.h>
#include <coms/NetBuffer.h>
#include <common/ARGParser.h>

class OptionEntry
{
public:
	enum OptionType
	{
		OptionEntryBoolType,
		OptionEntryIntType,
		OptionEntryStringType
	};

	OptionEntry(std::list<OptionEntry *> &group,
				const char *name,
				const char *description,
				unsigned int data);
	virtual ~OptionEntry();

	virtual bool writeToBuffer(NetBuffer &buffer) = 0;
	virtual bool readFromBuffer(NetBufferReader &reader) = 0;
	virtual bool addToArgParser(ARGParser &parser) = 0;

	const char *getName() { return name_.c_str(); }
	const char *getDescription() { return description_.c_str(); }
	unsigned getData() { return data_; }

	virtual const char *getDefaultValueAsString() = 0;
	virtual const char *getValueAsString() = 0;
	virtual bool setValueFromString(const char *string) = 0;
	virtual OptionType getType() = 0;

protected:
	unsigned int data_;
	std::string name_;
	std::string description_;
};

class OptionEntryHelper
{
public:
	static bool writeToBuffer(std::list<OptionEntry *> &options,
							  NetBuffer &buffer);
	static bool readFromBuffer(std::list<OptionEntry *> &options,
							   NetBufferReader &reader);

	static bool writeToFile(std::list<OptionEntry *> &options,
							char *fileName);
	static bool readFromFile(std::list<OptionEntry *> &options,
							 char *fileName);

	static bool addToArgParser(std::list<OptionEntry *> &options,
								ARGParser &parser);
};

class OptionEntryInt : public OptionEntry, public ARGParserIntI
{
public:
	OptionEntryInt(std::list<OptionEntry *> &group,
				   const char *name, 
				   const char *description,
				   unsigned int data,
				   int defaultValue);
	virtual ~OptionEntryInt();

	virtual bool writeToBuffer(NetBuffer &buffer);
	virtual bool readFromBuffer(NetBufferReader &reader);

	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const char *string);

	virtual int getValue();
	virtual bool setValue(int value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setIntArgument(int value);
	virtual OptionType getType();

protected:
	int defaultValue_;
	int value_;

};

class OptionEntryBoundedInt : public OptionEntryInt
{
public:
	OptionEntryBoundedInt(std::list<OptionEntry *> &group,
						  const char *name, 
						  const char *description,
						  unsigned int data,
						  int defaultValue,
						  int minValue, int maxValue);
	virtual ~OptionEntryBoundedInt();

	virtual bool setValue(int value);

protected:
	int minValue_, maxValue_;

};

class OptionEntryBool : public OptionEntry, public ARGParserBoolI
{
public:
	OptionEntryBool(std::list<OptionEntry *> &group,
					const char *name, 
					const char *description,
					unsigned int data,
					bool defaultValue);
	virtual ~OptionEntryBool();

	virtual bool writeToBuffer(NetBuffer &buffer);
	virtual bool readFromBuffer(NetBufferReader &reader);

	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const char *string);

	virtual bool setValue(bool value);
	virtual bool getValue();

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setBoolArgument(bool value);
	virtual OptionType getType();

protected:
	bool defaultValue_;
	bool value_;
};

class OptionEntryString : public OptionEntry, public ARGParserStringI
{
public:
	OptionEntryString(std::list<OptionEntry *> &group,
					  const char *name,
					  const char *description,
					  unsigned int data,
					  const char *defaultValue);
	virtual ~OptionEntryString();

	virtual bool writeToBuffer(NetBuffer &buffer);
	virtual bool readFromBuffer(NetBufferReader &reader);

	virtual const char *getValueAsString();
	virtual const char *getDefaultValueAsString();
	virtual bool setValueFromString(const char *string);

	virtual const char *getValue();
	virtual bool setValue(const char* value);

	virtual bool addToArgParser(ARGParser &parser);
	virtual bool setStringArgument(const char* value);
	virtual OptionType getType();

protected:
	std::string value_;
	std::string defaultValue_;

};

#endif
