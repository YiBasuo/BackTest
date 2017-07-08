#include <vector>
#include "Utility.h"
#include "Constants.h"
#include "TradeOrderT.h"
#include "Simulator.h"
#include "TestStrategy.h"

using namespace std;

double CalculateMeanVolume(const vector<DataLineT>& datalist, interval_t checkInterval)
{
	if (datalist.size() == 0)
	{
		return -1;
	}

	if (datalist.size() == 1)
	{
		return datalist[0].volume / 0.5;
	}

	vector<DataLineT>::const_reverse_iterator it = datalist.rbegin();
	for (; it != datalist.rend(); ++ it)
	{
		if (datalist.back() - (*it) > checkInterval)
		{
			break;
		}
	}
	Simulator::GetInstance().GetMainLog() << GetDateTimeStr((*it).dateTime) << " " << (*it).volume << endl;

	struct tm* startPtm = localtime(&(it->dateTime));
	double beginVolume = it->volume;

	double interval(checkInterval);
	if (startPtm->tm_hour == 20 && startPtm->tm_min == 59)
	{
		struct tm* endPtm = localtime(&datalist.back().dateTime);
		interval = endPtm->tm_sec + datalist.back().updateMillisec / 1000.0;
	}

	return (datalist.back().volume - beginVolume) / interval;
}

double CalculateInstantVolume(const vector<DataLineT>& datalist)
{
	if (datalist.empty())
	{
		return -1;
	}
	if (datalist.size() == 1)
	{
		return datalist[0].volume / 0.5;
	}
	vector<DataLineT>::const_reverse_iterator lastItr = datalist.rbegin();
	++ lastItr;
	return (datalist.back().volume - lastItr->volume) / 0.5;
}

void TestStrategy::RunData(
	vector<DataLineT> datalist, 
	vector<vector<DataLineT> > refDatalistVector,
	vector<TradeOrderT> activeOrders, 
	vector<PositionT> posiLong,
	vector<PositionT> posiShort)
{
	const interval_t checkInterval = 10;

	vector<DataLineT> sideDataList = refDatalistVector[0];
	vector<double> volumeVector;

	double meanVolume = CalculateMeanVolume(sideDataList, checkInterval);
	double instantVolume = CalculateInstantVolume(sideDataList);

	// Open position
	vector<DataLineT>::const_reverse_iterator lastItr = sideDataList.rbegin();
	++ lastItr;

	const double volumeThreshold_c = 1;

	if (sideDataList.back().askPrice > lastItr->askPrice && sideDataList.back().bidPrice >= lastItr->bidPrice && instantVolume - meanVolume > volumeThreshold_c)
	{
		Simulator::GetInstance().GetMainLog() << "~~~~" << "MeanVolume-" << meanVolume << "\tinstantVolume" << instantVolume << endl;
		SendOrder(datalist.back().askPrice, 1, BUY);
	}
	if (sideDataList.back().bidPrice < lastItr->bidPrice && sideDataList.back().askPrice <= lastItr->askPrice && instantVolume - meanVolume > volumeThreshold_c)
	{
		Simulator::GetInstance().GetMainLog() << "~~~~" << "MeanVolume-" << meanVolume << "\tinstantVolume" << instantVolume << endl;
		SendOrder(datalist.back().bidPrice, 1, SELLSHORT);
	}

	for (vector<TradeOrderT>::iterator it = activeOrders.begin(); it != activeOrders.end(); ++ it)
	{
	}

	const double holdingThreshold_c = 10;

	time_t currentTime = datalist.back().dateTime;
	tm* ptm = localtime(&currentTime);
	for (vector<PositionT>::iterator it = posiLong.begin(); it != posiLong.end(); ++ it)
	{
		if (Simulator::GetInstance().GetCurrentTime() - it->openTime >= holdingThreshold_c || ptm->tm_hour == 15)
		{
			SendOrder(datalist.back().bidPrice, it->lots, SELL);
		}
	}

	for (vector<PositionT>::iterator it = posiShort.begin(); it != posiShort.end(); ++ it)
	{
		if (Simulator::GetInstance().GetCurrentTime() - it->openTime >= holdingThreshold_c || ptm->tm_hour == 15)
		{
			SendOrder(datalist.back().askPrice, it->lots, BUYTOCOVER);
		}
	}
}
