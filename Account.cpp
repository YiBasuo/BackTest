#include <sstream>
#include <algorithm>
#include "Account.h"
#include "TradeOrderT.h"
#include "Constants.h"

using namespace std;

/****************************************Public Functions********************************************/
Account::Account() : maxOrderID(0), assets(500000), maxPositionHolding(0) {}
Account::Account(double assets_) : maxOrderID(0), assets(assets_) {}

Account::~Account() {}

void Account::SendOrder(double limitPrice, int lots, Operation_e op)
{
	TradeOrderT order(limitPrice, lots, op, maxOrderID, ACTIVE);
	++ maxOrderID;
	activeOrders.push_back(order);
	historyOrders.push_back(order);
}

void Account::CancelOrder(int cancelledOrderID)
{
	vector<TradeOrderT>::iterator it = find(activeOrders.begin(), activeOrders.end(), cancelledOrderID);

	if (it == activeOrders.end())
	{
		stringstream iss;
		iss << "Account: CancelOrder\nOrder id: \"" << cancelledOrderID << "\" not found";
		throw Error(iss.str());
	}

	it->status = CANCELLED;

	historyOrders.push_back(*it);
	activeOrders.erase(it);
}

void Account::MatchOrder(int matchedOrderID, double price, int lots, time_t currentTime)
{
	vector<TradeOrderT>::iterator it = find(activeOrders.begin(), activeOrders.end(), matchedOrderID);

	if (it == activeOrders.end())
	{
		stringstream iss;
		iss << "Account: Match\nOrder ID: \"" << matchedOrderID << "\" not found";
		throw Error(iss.str());
	}

	switch (it->GetOp())
	{
		case BUY:
			OpenPosition(LONG_POSI, price, lots, currentTime);
			break;
		case SELL:
			ClosePosition(LONG_POSI, price, lots);
			break;
		case SELLSHORT:
			OpenPosition(SHORT_POSI, price, lots, currentTime);
			break;
		case BUYTOCOVER:
			ClosePosition(SHORT_POSI, price, lots);
			break;
	}

	if (it->lots == lots)
	{
		it->status = MATCHED;
		it->price = price;

		historyOrders.push_back(*it);
		historyTrade.push_back(*it);

		activeOrders.erase(it);
		return;
	}

	if (it->lots > lots)
	{
		it->lots -= lots;

		TradeOrderT matchedOrder(*it);
		matchedOrder.status = MATCHED;
		matchedOrder.lots = lots;
		matchedOrder.price = price;

		historyOrders.push_back(matchedOrder);
		historyTrade.push_back(matchedOrder);
		return;
	}

	stringstream iss;
	iss << "Account: Match\nOrder id: \"" << matchedOrderID << "\" not processed";
	throw Error(iss.str());
}



vector<TradeOrderT> Account::GetActiveOrders() const
{
	return activeOrders;
}
vector<TradeOrderT> Account::GetHistoryOrders() const
{
	return historyOrders;
}
vector<TradeOrderT> Account::GetHistoryTrade() const
{
	return historyTrade;
}
vector<PositionT> Account::GetPositionLong() const
{
	return posiLong;
}
vector<PositionT> Account::GetPositionShort() const
{
	return posiShort;
}

void StringStreamInOrderVector(stringstream& iss, const vector<TradeOrderT>& targetVector)
{
	int label = 0;
	for (vector<TradeOrderT>::const_iterator it = targetVector.begin(); it != targetVector.end(); ++ it)
	{
		iss << "+\t" << label + 1 << ". " << *it  << endl;
		++ label;
	}
}

void StringStreamInPositionVector(stringstream& iss, const vector<PositionT> posiVector)
{
	int label = 0;
	for (vector<PositionT>::const_iterator it = posiVector.begin(); it != posiVector.end(); ++ it)
	{
		iss << "+\t" << label + 1 << ". " << *it  << endl;
		++ label;
	}
}

ostream& operator<<(ostream& os, const Account& account)
{
	stringstream iss;
	iss << "++++++++++++++++++++++++++++++++++Account Info+++++++++++++++++++++++++++++++++++" << endl;
	iss << "+ Total Asset:" << endl;
	iss << "+\t" << account.assets << endl;
	iss << "+ Active Orders:" << endl;
	StringStreamInOrderVector(iss, account.activeOrders);
	iss << "+ Orders History:" << endl;
	StringStreamInOrderVector(iss, account.historyOrders);
	iss << "+ Trade History:" << endl;
	StringStreamInOrderVector(iss, account.historyTrade);
	iss << "+ Current Long Positions:" << endl;
	StringStreamInPositionVector(iss, account.posiLong);
	iss << "+ Current Short Positions:" << endl;
	StringStreamInPositionVector(iss, account.posiShort);
	iss << "+ Maximum position ever holding: " << account.maxPositionHolding << endl;

	iss << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;

	os << iss.str();

	return os;
}


/******************************************Private Functions**************************************************************/
void Account::OpenPosition(Direction_e direction, double price, int lots, time_t currentTime)
{
	PositionT newPosi;
	newPosi.price = price;
	newPosi.lots = lots;
	newPosi.direction = direction;
	newPosi.openTime = currentTime;

	if (direction == LONG_POSI)
	{
		posiLong.push_back(newPosi);
	}

	if (direction == SHORT_POSI)
	{
		posiShort.push_back(newPosi);
	}

	int currentPositionHolding(0);
	for (vector<PositionT>::iterator it = posiLong.begin(); it != posiLong.end(); ++ it)
	{
		currentPositionHolding += it->lots;
	}
	for (vector<PositionT>::iterator it = posiShort.begin(); it != posiShort.end(); ++ it)
	{
		currentPositionHolding += it->lots;
	}
	maxPositionHolding = (maxPositionHolding > currentPositionHolding) ? maxPositionHolding : currentPositionHolding;
}

void Account::ClosePosition(Direction_e direction, double price, int lots)
{
	vector<PositionT>* posiVectorPtr = 0;
	posiVectorPtr = (direction == LONG_POSI) ? &posiLong : &posiShort;

	double profit = 0;

	while (lots >= posiVectorPtr->front().lots)
	{
		lots -= posiVectorPtr->front().lots;
		profit += (price - posiVectorPtr->front().price) * posiVectorPtr->front().lots;
		posiVectorPtr->erase(posiVectorPtr->begin());
	}

	posiVectorPtr->front().lots -= lots;
	profit += (price - posiVectorPtr->front().price) * lots;

	assets += profit * contract_size_c * ((direction == LONG_POSI) ? 1 : -1);
}
