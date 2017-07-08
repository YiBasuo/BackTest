#include <cmath>
#include <cstring>

#include <iostream>
#include <fstream>
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

#include "UserStrategy.h"

using namespace std;

void QuickPrintOrder(const TradeOrderT& order);
void QuickPrintDataLine(const DataLineT& dataline);
void CleanUpDataGeneratorVector(vector<DataGenerator*>& dataGeneratorVector);
void ReadDataFileList(const string& dataFileSummary, list<string>& dataFileList);

void RunOnFile(const string& filename);
void BulkRun();
void PrintUsage();


int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		PrintUsage();
		return 0;
	}

	if (strcmp(argv[1], "-b") == 0)
	{
		BulkRun();
	}
	else if (strcmp(argv[1], "-f") == 0 && argc == 3)
	{
		string datafile = argv[2];
		RunOnFile(datafile);
	}
	else
	{
		PrintUsage();
	}
	return 0;
}

void PrintUsage()
{
	cout << "usage: " << endl;
	cout << "\tBulk Run: ./run -b" << endl;
	cout << "\tRun on file: ./run -f [filename]" << endl;
}

void RunOnFile(const string& datafile)
{
	DataFileName::GetInstance().SetDataFileName(datafile);

	string filename = data_folder_path_c + DataFileName::GetInstance().GetFilename();
	interval_t interval(60);

	Strategy* strategyPtr = new UserStrategy();
	Report* reportPtr = new Report(filename);

	vector<DataGenerator*> refDataGeneratorVector;
	try
	{
		cout << "==========================================================================================" << endl;
		// Integrity check
		cout << "Checking Data Integrity for " << filename << "..." << endl << endl;
		DataGenerator::CheckDataIntegrity(filename);
		cout << endl << "Integrity check complete." << endl;

		// Create Account model
		cout << "Create an account..." << endl;
		Account account;
		cout << "Account created." << endl;
		cout << account << endl << endl;

		// Set up Main DataGenerator
		DataGenerator dataGenerator(interval, filename);

		Simulator simulator;

		cout << "Setting strategy..." << endl;
		simulator.SetStrategy(strategyPtr);
		cout << "Strategy set." << endl << endl;

		
		cout << "Run Strategy..." << endl;
		simulator.Run(account, dataGenerator, refDataGeneratorVector);

		cout << "Run Strtegy complete." << endl;

		cout << "Generating Report for " << filename << "..." << endl;
		simulator.SetReport(reportPtr);

		simulator.GenerateReport(account);
		cout << "Report Generated." << endl;

		cout << "==========================================================================================" << endl;

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

		cout << "==========================================================================================" << endl;
	}
}

void BulkRun()
{
	// Readin Data File Names
	string dataFileSummary = default_data_file_summary_c;
	list<string> dataFileList;
	try
	{
		ReadDataFileList(dataFileSummary, dataFileList);
	}
	catch (Error& e)
	{
		cout << e.GetMsg() << endl;
	}

	for (list<string>::iterator it = dataFileList.begin(); it != dataFileList.end(); ++ it)
	{
		RunOnFile(*it);
	}
}

void QuickPrintOrder(const TradeOrderT& order)
{
	cout << order << endl;
}

void QuickPrintDataLine(const DataLineT& dataline)
{
	cout << dataline << endl;
}

void CleanUpDataGeneratorVector(vector<DataGenerator*>& dataGeneratorVector)
{
	for (vector<DataGenerator*>::iterator it = dataGeneratorVector.begin(); it != dataGeneratorVector.end(); ++ it)
	{
		delete (*it);
	}
}

void ReadDataFileList(const string& dataFileSummary, list<string>& dataFileList)
{
	ifstream fin;
	fin.open(dataFileSummary.c_str());
	string datafile;
	while (fin >> datafile)
	{
		dataFileList.push_back(datafile);
	}
}
