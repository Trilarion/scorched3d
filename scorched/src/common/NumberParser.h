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


// NumberParser.h: interface for the NumberParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NumberParser_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)
#define AFX_NumberParser_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_

#include <string>
#include <list>
#include <common/RandomGenerator.h>
#include <engine/ScorchedContext.h>



class NumberParser
{
public:
        NumberParser();
        NumberParser(const char *expression);
        NumberParser(float value);
        NumberParser(int value);
        virtual ~NumberParser();

        float getValue(ScorchedContext &context); //RandomGenerator &generator);
        unsigned int getUInt(ScorchedContext &context);
	int getInt(ScorchedContext &context);
        bool setExpression(const char *expression);
        bool setExpression(int value);  // FIXME Just testing...
        bool setExpression(float value);

protected:
        bool getOperands();
        bool isInt_, isFloat_;
        std::string expression_;
        std::list<float> operands_;
        float max_, min_, step_;

};

#endif // !defined(AFX_NumberParser_H__54F37DA5_36EB_11D3_BE80_000000000000__INCLUDED_)