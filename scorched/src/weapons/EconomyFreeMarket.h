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

#if !defined(__INCLUDE_EconomyFreeMarketh_INCLUDE__)
#define __INCLUDE_EconomyFreeMarketh_INCLUDE__

#include <weapons/Economy.h>
#include <map>
#include <string>

/**
This economy changes the prices of the weapons depending
on how many have been bought.  The more popular accessories
rise in price and the less popular accessories fall.
Thus making people choose different weapons from time to time
(perhaps).
*/
class EconomyFreeMarket : public Economy
{
public:
	EconomyFreeMarket();
	virtual ~EconomyFreeMarket();

	virtual bool loadPrices();
	virtual bool savePrices();
	virtual void calculatePrices();

	virtual void accessoryBought(Tank *tank, 
		const char *accessoryName);
	virtual void accessorySold(Tank *tank, 
		const char *accessoryName);

	virtual int getAccessoryBuyPrice(
		const char *accessoryName);
	virtual int getAccessorySellPrice(
		const char *accessoryName);

REGISTER_CLASS_HEADER(EconomyFreeMarket);

protected:
	struct PriceEntry
	{
		PriceEntry(int b=0, int s=0) : buyPrice(b), sellPrice(s) {}

		int buyPrice;
		int sellPrice;
	};
	std::map<std::string, PriceEntry> prices_;
};

#endif
