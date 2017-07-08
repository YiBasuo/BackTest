#ifndef TEST_STRATEGY_H_
#define TEST_STRATEGY_HH

#include <vector>
#include "Strategy.h"

class TestStrategy : public Strategy
{
public:
	TestStrategy() {}
	~TestStrategy() {}
	void RunData(
		std::vector<DataLineT> datalist, 
		std::vector<std::vector<DataLineT> > refDatalistVector,
		std::vector<TradeOrderT> activeOrders, 
		std::vector<PositionT> posiLong,
		std::vector<PositionT> posiShort
		);

private:
};


#endif
