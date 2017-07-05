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
		vector<PositionT> posiShort
		);

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

void CleanUpDataGeneratorVector(vector<DataGenerator*>& dataGeneratorVector)
{
	for (vector<DataGenerator*>::iterator it = dataGeneratorVector.begin(); it != dataGeneratorVector.end(); ++ it)
	{
		delete (*it);
	}
}

int main()
{
	Strategy* strategyPtr = new TestStrategy();
	vector<DataGenerator*> refDataGeneratorVector;

	Report* reportPtr(0);
	try
	{
		//************************Test Simulator****************************//
		string filenameCu = "Data/cu1708_20170629";
		string filenameAl = "Data/al1708_20170629";
		interval_t interval(60);
		
		// Integrity check
		cout << "Checking Cu data Integrity..." << endl << endl;
		DataGenerator::CheckDataIntegrity(filenameCu);
		cout << endl << "Cu Integrity check complete." << endl;
		cout << "Checking Al data Integrity..." << endl << endl;
		DataGenerator::CheckDataIntegrity(filenameAl);
		cout << endl << "Al Integrity check complete." << endl;

		// Create Account model
		cout << "Create an account..." << endl;
		Account account;
		cout << "Account created." << endl;
		cout << account << endl << endl;

		// Set up Main DataGenerator
		cout << "Loading Al data..." << endl;
		DataGenerator dataGeneratorAl(interval, filenameAl);
		cout << "Al Data loaded." << endl << endl;

		// Set up refDataGenerator #1
		cout << "Loading Cu data..." << endl;
		DataGenerator* refDataGenerator1 = new DataGenerator(interval, filenameCu);
		cout << "Cu Data loaded." << endl << endl;
		// Push to refDataGeneratorVector
		refDataGeneratorVector.push_back(refDataGenerator1);

		cout << "--------------------------Testing Simulator----------------------------" << endl;
		cout << "Setting strategy..." << endl;
		Simulator::GetInstance().SetStrategy(strategyPtr);
		cout << "Strategy set." << endl << endl;

		cout << "Run Strategy..." << endl;

		// Run
		Simulator::GetInstance().Run(account, dataGeneratorAl, refDataGeneratorVector);

		cout << "Run Strtegy complete." << endl;
		cout << endl << "Showing account summary..." << endl;
		cout << account << endl;

		cout << "Generating Report..." << endl;
		reportPtr = new Report(filenameAl);	
		Simulator::GetInstance().SetReport(reportPtr);

		Simulator::GetInstance().GenerateReport(account.GetHistoryTrade());
		cout << "Report Generated." << endl;

		if (strategyPtr)
		{
			delete strategyPtr;
		}
		if (reportPtr)
		{
			delete reportPtr;
		}
		CleanUpDataGeneratorVector(refDataGeneratorVector);
	}
	catch (Error& e)
	{
		cout << "Error: " << endl;
		cout << e.GetMsg() << endl;
		if (strategyPtr)
		{
			delete strategyPtr;
		}
		if (reportPtr)
		{
			delete reportPtr;
		}
		CleanUpDataGeneratorVector(refDataGeneratorVector);
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
