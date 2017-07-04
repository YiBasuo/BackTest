#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>

#include "Utility.h"
#include "DataGenerator.h"
#include "TradeOrderT.h"
#include "Account.h"
#include "Strategy.h"
#include "Simulator.h"

using namespace std;

void QuickPrintOrder(const TradeOrderT& order);
void QuickPrintDataLine(const DataLineT& dataline);

class TestStrategy : public Strategy
{
public:
	TestStrategy() {}
	~TestStrategy() {}
	void RunData(
		vector<DataLineT> datalist, 
		vector<vector<DataLineT> > refDatalistVector,
		vector<TradeOrderT> activeOrders, 
		vector<PositionT> posiLong,
		vector<PositionT> posiShort,
		vector<double>& limitPriceVector, 
		vector<int>& lotsVector, 
		vector<Operation_e>& opVector, 
		vector<int>& cancelledOrderIDVector);

private:
};

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

	struct tm* startPtm = localtime(&(it->dateTime));
	//double beginVolume = (startPtm->tm_hour == 20 && startPtm->tm_min == 59) ? 0 : it->volume;
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
	vector<PositionT> posiShort,
	vector<double>& limitPriceVector, 
	vector<int>& lotsVector, 
	vector<Operation_e>& opVector, 
	vector<int>& cancelledOrderIDVector)
{
	const interval_t checkInterval = 10;

	vector<DataLineT> sideDataList = refDatalistVector[0];
	vector<double> volumeVector;

	double meanVolume = CalculateMeanVolume(sideDataList, checkInterval);
	double instantVolume = CalculateInstantVolume(sideDataList);
	/*static int cnt(0);
	if (cnt < 20)
	{
		cout << "#" << cnt + 1 << " mean ";
		cout << meanVolume << " | " << " instant " << instantVolume << endl;
	}
	++ cnt;*/

	// Open position
	vector<DataLineT>::const_reverse_iterator lastItr = datalist.rbegin();
	++ lastItr;

	const double volumeThreshold_c = 1;

	if (sideDataList.back().askPrice > lastItr->askPrice && sideDataList.back().bidPrice >= lastItr->bidPrice && instantVolume - meanVolume > volumeThreshold_c)
	{
		limitPriceVector.push_back(datalist.back().bidPrice);
		lotsVector.push_back(1);
		opVector.push_back(BUY);
	}
	if (sideDataList.back().bidPrice < lastItr->bidPrice && sideDataList.back().askPrice <= lastItr->askPrice && instantVolume - meanVolume > volumeThreshold_c)
	{
		limitPriceVector.push_back(datalist.back().askPrice);
		lotsVector.push_back(1);
		opVector.push_back(SELLSHORT);
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
			limitPriceVector.push_back(datalist.back().askPrice);
			lotsVector.push_back(it->lots);
			opVector.push_back(SELL);
		}
	}

	for (vector<PositionT>::iterator it = posiShort.begin(); it != posiShort.end(); ++ it)
	{
		if (Simulator::GetInstance().GetCurrentTime() - it->openTime >= holdingThreshold_c || ptm->tm_hour == 15)
		{
			limitPriceVector.push_back(datalist.back().bidPrice);
			lotsVector.push_back(it->lots);
			opVector.push_back(BUYTOCOVER);
		}
	}
}

int main()
{
	Strategy* strategyPtr = new TestStrategy();

	try
	{
		//************************Test Simulator****************************//
		string filenameCu = "Data/cu1708_20170629";
		string filenameAl = "Data/al1708_20170629";
		interval_t interval(60);
		
		// Integrity check
		cout << "Checking Cu data Integrity..." << endl << endl;
		DataGenerator::CheckDataIntegrity(filenameCu);
		cout << endl << "Integrity check complete." << endl;
		cout << "Checking Al data Integrity..." << endl << endl;
		DataGenerator::CheckDataIntegrity(filenameAl);
		cout << endl << "Integrity check complete." << endl;

		// Create Account model
		cout << "Create an account..." << endl;
		Account account;
		cout << "Account created." << endl;
		cout << account << endl << endl;

		// Set up DataGenerator #1
		cout << "Loading Cu data..." << endl;
		DataGenerator dataGeneratorCu(interval, filenameCu);
		cout << "Cu Data loaded." << endl << endl;
	
		// Set up DataGenerator #2
		cout << "Loading Al data..." << endl;
		DataGenerator dataGeneratorAl(interval, filenameAl);
		cout << "Al Data loaded." << endl << endl;

		cout << "--------------------------Testing Simulator----------------------------" << endl;
		cout << "Setting strategy..." << endl;
		Simulator::GetInstance().SetStrategy(strategyPtr);
		cout << "Strategy set." << endl << endl;
		cout << "Run Strategy..." << endl;

		// Add all the reference data generators to the list
		vector<DataGenerator*> refDataGeneratorVector;
		refDataGeneratorVector.push_back(&dataGeneratorCu);

		// Run
		Simulator::GetInstance().Run(account, dataGeneratorAl, refDataGeneratorVector);

		cout << "Run Strtegy complete." << endl;
		cout << endl << "Showing account summary..." << endl;
		cout << account << endl;

		delete strategyPtr;
	}
	catch (Error& e)
	{
		cout << "Error: " << endl;
		cout << e.GetMsg() << endl;
		delete strategyPtr;
	}

	return 0;
}

void QuickPrintOrder(const TradeOrderT& order)
{
	cout << order << endl;
}

void QuickPrintDataLine(const DataLineT& dataline)
{
	cout << dataline << endl;
}
