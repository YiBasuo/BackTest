#include <iostream>
#include <sstream>
#include <ctime>
#include <cstring>

#include "Constants.h"
#include "Utility.h"

using namespace std;

string Error::GetMsg()
{
	return msg;
}

ostream& operator<<(ostream& os, const DataLineT& dataline)
{
	stringstream iss;
/*	iss << "dateTime: " << GetDateTimeStr(dataline.dateTime);
	iss << "\tupdateMillisec: " << dataline.updateMillisec;
	iss << "\tinstrumentID: " << dataline.instrumentID;
	iss << "\tlastPrice: " << dataline.lastPrice;
	iss << "\tpreSettle: " << dataline.preSettle;
	iss << "\tpreClose: " << dataline.preClose;
	iss << "\tpreOpenInterest: " << dataline.preOpenInterest;
	iss << "\topenPrice: " << dataline.openPrice;
	iss << "\thighestPrice: " << dataline.highestPrice;
	iss << "\tlowestPrice: " << dataline.lowestPrice;
	iss << "\tvolume: " << dataline.volume;
	iss << "\tturnover: " << dataline.turnover;
	iss << "\topenInterest: " << dataline.openInterest;
	iss << "\tupperLimitPrice: " << dataline.upperLimitPrice;
	iss << "\tlowerLimitPrice: " << dataline.lowerLimitPrice;
	iss << "\tbidPrice: " << dataline.bidPrice;
	iss << "\tbidVolume: " << dataline.bidVolume;
	iss << "\taskPrice: " << dataline.askPrice;
	iss << "\taskVolume: " << dataline.askVolume;
	iss << "\taveragePrice: " << dataline.averagePrice;
	*/
	iss << GetDateTimeStr(dataline.dateTime);
	iss << "\t" << dataline.updateMillisec;
	iss << "\t" << dataline.instrumentID;
	iss << "\t" << dataline.lastPrice;
	iss << "\t" << dataline.preSettle;
	iss << "\t" << dataline.preClose;
	iss << "\t" << dataline.preOpenInterest;
	iss << "\t" << dataline.openPrice;
	iss << "\t" << dataline.highestPrice;
	iss << "\t" << dataline.lowestPrice;
	iss << "\t" << dataline.volume;
	iss << "\t" << dataline.turnover;
	iss << "\t" << dataline.openInterest;
	iss << "\t" << dataline.upperLimitPrice;
	iss << "\t" << dataline.lowerLimitPrice;
	iss << "\t" << dataline.bidPrice;
	iss << "\t" << dataline.bidVolume;
	iss << "\t" << dataline.askPrice;
	iss << "\t" << dataline.askVolume;
	iss << "\t" << dataline.averagePrice;

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

string GetDateTimeStr(time_t dateTime)
{
    tm* ptm = localtime(&dateTime);  
  
    //"2011-07-18 23:03:01 ";  
    string strFormat = "%Y-%m-%d %H:%M:%S";  
  
    char buffer[30];  
	strftime(buffer, 30, strFormat.c_str(), ptm);
    string dateTimeStr(buffer);  

	return dateTimeStr;
}

time_t ConvertStringToTime(const string & timeString)
{
	if (timeString.size() >= max_time_string_size_c)
	{
		string msg = "DataGenerator: ConvertStringToTime\ntimeString -" + timeString + "- is too long";
		throw Error(msg);
	}

	tm rawtm;
	time_t time;
	char buf[max_time_string_size_c];

	strcpy(buf, timeString.c_str());
	strptime(buf, "%Y-%m-%d %H:%M:%S", &rawtm);
	rawtm.tm_isdst = -1;
	time = mktime(&rawtm);

	return time;
}
