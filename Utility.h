#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include <fstream>

/**********************************Exception handling***********************************************/
class Error
{
public:
	Error(std::string msg_) : msg(msg_) {}
	std::string GetMsg();

private:
	std::string msg;
};

/************************************Operation and Positions****************************************/
enum Operation_e {BUY, SELL, SELLSHORT, BUYTOCOVER};
enum Direction_e {LONG_POSI, SHORT_POSI};

struct PositionT
{
	double price;
	int lots;
	Direction_e direction;
	time_t openTime;
};
// outstream positionT
std::ostream& operator<<(std::ostream& os, const PositionT& posi);

/******************************Data Line Structure**************************************************/
// structure for storing info of a single line in data file
struct DataLineT
{
	// Time info
	time_t dateTime;
	int updateMillisec;
	// Pre-day info
	std::string instrumentID;
	double preSettle;
	double preClose;
	double preOpenInterest;

	// Overall info
	double openPrice;
	double highestPrice;
	double lowestPrice;
	double turnover;
	double volume;
	double openInterest;
	double upperLimitPrice;
	double lowerLimitPrice;

	// Critical info
	double lastPrice;
	double bidPrice;
	double bidVolume;
	double askPrice;
	double askVolume;
	double averagePrice;
};
std::ostream& operator<<(std::ostream& os, const DataLineT& dataline);
double operator-(const DataLineT& left, const DataLineT& right);

#endif /*UTILITY_H_*/
