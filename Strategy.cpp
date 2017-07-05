#include "Constants.h"
#include "Strategy.h"

using namespace std;

Strategy::Strategy()
	: quota(maximum_position_limit_c)
{
}

void Strategy::SendOrder(double limitPrice, int lots, Operation_e op)
{
	if ((op == BUY || op == SELLSHORT) && quota > 0)
	{
		int lotsSent = (lots > quota) ? quota : lots;
		limitPriceVector.push_back(limitPrice);
		lotsVector.push_back(lotsSent);
		opVector.push_back(op);

		quota -= lotsSent;
	}

	if (op == SELL || op == BUYTOCOVER)
	{
		limitPriceVector.push_back(limitPrice);
		lotsVector.push_back(lots);
		opVector.push_back(op);

		quota += lots;
	}
}

void Strategy::CacelOrder(int cancelledOrderID, int lots)
{
	cancelledOrderIDVector.push_back(cancelledOrderID);
	quota += lots;
}



vector<double> Strategy::GetLimitPriceVector() const
{
	return limitPriceVector;
}

vector<int> Strategy::GetLotsVector() const
{
	return lotsVector;
}
vector<Operation_e> Strategy::GetOpVector() const
{
	return opVector;
}

vector<int> Strategy::GetCancelledOrderIDVector() const
{
	return cancelledOrderIDVector;
}

void Strategy::Reset()
{
	limitPriceVector.clear();
	lotsVector.clear();
	opVector.clear();
}
