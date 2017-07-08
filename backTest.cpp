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

#include "TestStrategy.h"

using namespace std;

void QuickPrintOrder(const TradeOrderT& order);
void QuickPrintDataLine(const DataLineT& dataline);



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
