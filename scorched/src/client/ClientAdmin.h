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

#if !defined(AFX_ClientAdmin_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_)
#define AFX_ClientAdmin_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_

#include <GLEXT/GLConsoleRuleMethodIAdapter.h>

class ClientAdmin
{
public:
	static ClientAdmin *instance();

protected:
	static ClientAdmin *instance_;

	void admin(std::list<GLConsoleRuleSplit> split, std::list<std::string> &result);
	void adminHelp(std::list<std::string> &result);

private:
	ClientAdmin();
	virtual ~ClientAdmin();

};

#endif // !defined(AFX_ClientAdmin_H__571BC1CA_BDBC_4F2B_9F83_4AA083520198__INCLUDED_)

