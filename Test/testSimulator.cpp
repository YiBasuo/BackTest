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
		list<DataLineT> datalist, 
		vector<list<DataLineT> > refDatalistVector,
		vector<TradeOrderT> activeOrders, 
		vector<double>& limitPriceVector, 
		vector<int>& lotsVector, 
		vector<Operation_e>& opVector, 
		vector<int>& cancelledOrderIDVector);

private:
};

void TestStrategy::RunData(
	list<DataLineT> datalist, 
	vector<list<DataLineT> > refDatalistVector,
	vector<TradeOrderT> activeOrders, 
	vector<double>& limitPriceVector, 
	vector<int>& lotsVector, 
	vector<Operation_e>& opVector, 
	vector<int>& cancelledOrderIDVector)
{
	static int cnt = 0;
	if (cnt == 0)
	{
		limitPriceVector.push_back(900000);
		lotsVector.push_back(2);
		opVector.push_back(BUY);
	}

	if (cnt == 1)
	{
		limitPriceVector.push_back(0.000001);
		lotsVector.push_back(2);
		opVector.push_back(BUY);
	}

	if (cnt == 3)
	{
		limitPriceVector.push_back(9000000);
		lotsVector.push_back(500);
		opVector.push_back(BUY);
	}

	if (cnt ==4)
	{
		cancelledOrderIDVector.push_back(1);
	}

	if (cnt ==5)
	{
		limitPriceVector.push_back(0.00001);
		lotsVector.push_back(5);
		opVector.push_back(SELL);
	}

	++ cnt;
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
		DataGenerator dataGeneratorAlpha(interval, filenameCu);
		cout << "Cu Data loaded." << endl << endl;
	
		// Set up DataGenerator #2
		cout << "Loading Al data..." << endl;
		DataGenerator dataGeneratorBeta(interval, filenameAl);
		cout << "Al Data loaded." << endl << endl;

		cout << "--------------------------Testing Simulator----------------------------" << endl;
		cout << "Setting strategy..." << endl;
		Simulator::GetInstance().SetStrategy(strategyPtr);
		cout << "Strategy set." << endl << endl;
		cout << "Run Strategy..." << endl;

		// Add all the reference data generators to the list
		vector<DataGenerator*> refDataGeneratorVector;
		refDataGeneratorVector.push_back(&dataGeneratorBeta);
		Simulator::GetInstance().Run(account, dataGeneratorAlpha, refDataGeneratorVector);

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
