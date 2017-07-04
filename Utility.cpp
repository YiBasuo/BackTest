#include <iostream>
#include <sstream>

#include "Utility.h"

using namespace std;

string Error::GetMsg()
{
	return msg;
}

ostream& operator<<(ostream& os, const DataLineT& dataline)
{
	stringstream iss;
	iss << "##########DataLine print##############" << endl;
	iss << "dateTime: " << ctime(&dataline.dateTime);
	iss << "updateMillisec: " << dataline.updateMillisec << endl;
	iss << "instrumentID: " << dataline.instrumentID << endl;
	iss << "lastPrice: " << dataline.lastPrice << endl;
	iss << "preSettle: " << dataline.preSettle << endl;
	iss << "preClose: " << dataline.preClose << endl;
	iss << "preOpenInterest: " << dataline.preOpenInterest << endl;
	iss << "openPrice: " << dataline.openPrice << endl;
	iss << "highestPrice: " << dataline.highestPrice << endl;
	iss << "lowestPrice: " << dataline.lowestPrice << endl;
	iss << "volume: " << dataline.volume << endl;
	iss << "turnover: " << dataline.turnover << endl;
	iss << "openInterest: " << dataline.openInterest << endl;
	iss << "upperLimitPrice: " << dataline.upperLimitPrice << endl;
	iss << "lowerLimitPrice: " << dataline.lowerLimitPrice << endl;
	iss << "bidPrice: " << dataline.bidPrice << endl;
	iss << "bidVolume: " << dataline.bidVolume << endl;
	iss << "askPrice: " << dataline.askPrice << endl;
	iss << "askVolume: " << dataline.askVolume << endl;
	iss << "averagePrice: " << dataline.averagePrice << endl;
	iss << "#######DataLine print ends############" << endl;

	os << iss.str();

	return os;
}

std::ostream& operator<<(std::ostream& os, const PositionT& posi)
{
	stringstream iss;
	iss << "Position " << (posi.direction == LONG_POSI ? "Long(+)" : "Short(-)"); 
	iss << " | Lots: " << posi.lots;
	iss << " | Price " << posi.price;
	iss << " | opened at time " << ctime(&posi.openTime);

	os << iss.str();

	return os;
}

double operator-(const DataLineT& left, const DataLineT& right)
{
	return difftime(left.dateTime, right.dateTime) + (left.updateMillisec - right.updateMillisec) / 1000.0;
}

double operator-(const DataLineT& left, time_t rightTime)
{
	return difftime(left.dateTime, rightTime) + left.updateMillisec / 1000.0;
}

double operator-(time_t leftTime, const DataLineT& right)
{
	return difftime(leftTime, right.dateTime) + right.updateMillisec / 1000.0;
}
