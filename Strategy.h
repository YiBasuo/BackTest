#ifndef STRATEGY_H_
#define STRATEGY_H_

#include "Utility.h"
#include "TradeOrderT.h"
#include <vector>

class Strategy
{
public:
	Strategy();

	int GetDefaultLots();

	virtual void RunData(
		std::vector<DataLineT> datalist, 
		std::vector<std::vector<DataLineT> > refDatalistVector,
		std::vector<TradeOrderT> activeOrders, 
		std::vector<PositionT> longPosi,
		std::vector<PositionT> shortPosi
		) = 0;

	// Accessors
	std::vector<double> GetLimitPriceVector() const;
	std::vector<int> GetLotsVector() const;
	std::vector<Operation_e> GetOpVector() const;
	std::vector<int> GetCancelledOrderIDVector() const;

	// After simulator processed all orders, call this to reset the containers
	void Reset();
protected:
	void SendOrder(double limitPrice, int lots, Operation_e op);

	void CacelOrder(int cancelledOrderID, int lots);

private:
	int quota;

	std::vector<double> limitPriceVector;
	std::vector<int> lotsVector;
	std::vector<Operation_e> opVector;

	std::vector<int> cancelledOrderIDVector;
};

#endif /*STRATEGY_H_*/
