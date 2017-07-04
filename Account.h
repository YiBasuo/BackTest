#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <vector>
#include "Utility.h"
#include "TradeOrderT.h"

class Account
{
public:
	Account();
	Account(double asset_);
	~Account();

	void SendOrder(double limitPrice, int lots, Operation_e op);
	void CancelOrder(int cancelledOrderID);
	void MatchOrder(int matchedOrderID, double price, int lots, time_t currentTime);

	std::vector<TradeOrderT> GetActiveOrders() const;
	std::vector<TradeOrderT> GetHistoryOrders() const;
	std::vector<TradeOrderT> GetHistoryTrade() const;
	std::vector<PositionT> GetPositionLong() const;
	std::vector<PositionT> GetPositionShort() const;
	std::vector<PositionT> GetMaxPositionHolding() const;

	friend std::ostream& operator<<(std::ostream& os, const Account& account);
private:
	void OpenPosition(Direction_e direction, double price, int lots, time_t currentTime);
	void ClosePosition(Direction_e direction, double price, int lots);

	int maxOrderID;

	std::vector<TradeOrderT> activeOrders;
	std::vector<TradeOrderT> historyOrders;

	std::vector<TradeOrderT> historyTrade;

	double assets;
	std::vector<PositionT> posiLong;
	std::vector<PositionT> posiShort;

	int maxPositionHolding;
};

#endif /*ACCOUNT_H_*/
