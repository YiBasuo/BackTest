#ifndef STRATEGY_H_
#define STRATEGY_H_

#include "Utility.h"
#include "TradeOrderT.h"
#include <vector>

class Strategy
{
public:
	Strategy();
	~Strategy();

	int GetDefaultLots();

	virtual void RunData(
		std::vector<DataLineT> datalist, 
		std::vector<std::vector<DataLineT> > refDatalistVector,
		std::vector<TradeOrderT> activeOrders, 
		std::vector<PositionT> longPosi,
		std::vector<PositionT> shortPosi,
		std::vector<double>& limitPriceVector, 
		std::vector<int>& lotsVector, 
		std::vector<Operation_e>& opVector, 
		std::vector<int>& cancelledOrderIDVector) = 0;
	
protected:
	int defaultLots;
private:
};

#endif /*STRATEGY_H_*/
