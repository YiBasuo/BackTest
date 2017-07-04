#ifndef TRADEORDERT_H_
#define TRADEORDERT_H_

#include <iostream>
#include "Utility.h"

class Account;

enum OrderStatus_e {ACTIVE, CANCELLED, MATCHED};

class TradeOrderT
{
public:
	// Accessors
	double GetPrice() const;
	int GetLots() const;
	Operation_e GetOp() const;
	int GetID() const;
	OrderStatus_e GetStatus() const;

	friend bool operator==(const TradeOrderT& left, const TradeOrderT& right);
	friend std::ostream& operator<<(std::ostream& os, const TradeOrderT& order);

	friend bool operator==(int leftID, const TradeOrderT& right);
	friend bool operator==(const TradeOrderT& left, int rightID);

	friend class Account;
private:
	// TradeOrderT can only be created by AccoutT instance
	TradeOrderT();
	TradeOrderT(double price_, int lots_, Operation_e op_, int id_, OrderStatus_e status_);

	double price;
	int lots;
	Operation_e op;
	int id;
	OrderStatus_e status;
};

#endif /*TRADEORDERT_H_*/
