#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <map>
#include <vector>
#include "Utility.h"
#include "TradeOrderT.h"

class Account
{
public:
	Account();
	Account(double asset_);
	~Account();

	void SendOrder(double limitPrice, int lots, Operation_e op, time_t orderTime, int millisec);
	void CancelOrder(int cancelledOrderID, time_t cancelTime, int cancelMillisec);
	void MatchOrder(int matchedOrderID, double price, int lots, time_t matchTime, int matchMillisec);

	std::vector<TradeOrderT> GetActiveOrders() const;
	std::vector<TradeOrderT> GetHistoryOrders() const;
	std::vector<TradeOrderT> GetHistoryTrade() const;
	std::vector<PositionT> GetPositionsLong() const;
	std::vector<PositionT> GetPositionsShort() const;
	std::vector<PositionT> GetMaxPositionsHolding() const;

	friend std::ostream& operator<<(std::ostream& os, const Account& account);

private:
	// Operation functions
	void OpenPosition(Direction_e direction, double price, int lots, time_t currentTime, int millisec);
	void ClosePosition(Direction_e direction, double price, int lots);

	// Analysis functions
	void UpdateTrackerOrder(const TradeOrderT& matchedOrder);
	void AddOrderToTracker(const TradeOrderT& matchedOrder);
	void RemoveOrderFromTracker(const TradeOrderT& matchedOrder);

private:
	//**************Functional Varibales*****************//
	// Order ID: Unique ID for trade order. Single increasing
	int maxOrderID;

	//**************Records Varibales*******************//
	// Order Records: active and history
	std::vector<TradeOrderT> activeOrders;
	std::vector<TradeOrderT> historyOrders;
	// Trade Record
	std::vector<TradeOrderT> historyTrade;

	//**************Account Varibles**********************//
	double assets;
	std::vector<PositionT> posiLong;
	std::vector<PositionT> posiShort;

	//**************Statistics Variables***************//
	// Maximum position hold at the same time during entire test
	int maxPositionHolding;
};

#endif /*ACCOUNT_H_*/
