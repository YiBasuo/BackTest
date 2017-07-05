#include <sstream>
#include "TradeOrderT.h"
#include "Utility.h"
#include "Account.h"

using namespace std;

/*************************************Public functions*****************************************/
double TradeOrderT::GetPrice() const
{
	return price;
}

int TradeOrderT::GetLots() const
{
	return lots;
}

Operation_e TradeOrderT::GetOp() const
{
	return op;
}

int TradeOrderT::GetID() const
{
	return id;
}

OrderStatus_e TradeOrderT::GetStatus() const
{
	return status;
}

time_t TradeOrderT::GetTime() const
{
	return orderTime;
}

int TradeOrderT::GetMillisec() const
{
	return millisec;
}

bool operator==(const TradeOrderT& left, const TradeOrderT& right)
{
	if (left.id == right.id)
	{
		return 1;
	}
	return 0;
}

bool operator==(int leftID, const TradeOrderT& right)
{
	if (leftID == right.id)
	{
		return 1;
	}
	return 0;
}

bool operator==(const TradeOrderT& left, int rightID)
{
	if (left.id == rightID)
	{
		return 1;
	}
	return 0;
}

ostream& operator<<(ostream& os, const TradeOrderT& order)
{
	stringstream iss;
	switch (order.status)
	{
		case ACTIVE:
			iss << "Active    Order ";
			break;
		case CANCELLED:
			iss << "Cancelled Order ";
			break;
		case MATCHED:
			iss << "Matched   Order ";
			break;
		default:
			iss << "UNKNOWN Status";
	}
	iss << "id- " << order.id << ": ";
	iss << "Time- " << GetDateTimeStr(order.orderTime);
	if (order.millisec == 0)
	{
		iss << ".0\t";
	}
	if (order.millisec == 500)
	{
		iss << ".5\t";
	}
	switch (order.op)
	{
		case BUY:
			iss << "Buy        ";
			break;
		case SELL:
			iss << "Sell       ";
			break;
		case SELLSHORT:
			iss << "Sellshort  ";
			break;
		case BUYTOCOVER:
			iss << "BuyToCover ";
			break;
		default:
			iss << "UNKNOWN operation";
	}
	iss << order.lots << " at price: " << order.price;
	os << iss.str();
	return os;
}

bool operator<(const TradeOrderT& left, const TradeOrderT& right)
{
	return left.id < right.id;
}

/******************************************Private functions************************************************/
TradeOrderT::TradeOrderT()
	: price(0),
	  lots(0), 
	  op(BUY),
	  id(-1),
	  status(CANCELLED)
{
}

TradeOrderT::TradeOrderT(double price_, int lots_, Operation_e op_, int id_, OrderStatus_e status_, time_t orderTime_, int millisec_) 
	: price(price_),
	  lots(lots_), 
	  op(op_),
	  id(id_),
	  status(status_),
	  orderTime(orderTime_),
	  millisec(millisec_)
{
}
