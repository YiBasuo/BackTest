#include <ctime>
#include <cstdio>
#include <cstring>

#include <iostream>
#include <fstream>
#include <sstream>

#include "DataGenerator.h"
#include "Simulator.h"

using namespace std;

//*********************************************Public functions*********************************************//
DataGenerator::DataGenerator(int interval_, const string& filename) : interval_c(interval_)
{
	nextDataLinePtr = 0;
	OpenAndRead(filename);
}

DataGenerator::~DataGenerator()
{
	if (fin.is_open())
	{
		fin.close();
	}

	if (!nextDataLinePtr)
	{
		delete nextDataLinePtr;
	}
}

interval_t DataGenerator::GetInterval() const
{
	return interval_c;
}

vector<DataLineT> DataGenerator::GetData() const
{
	return data;
}

time_t DataGenerator::GetNextDateTime() const
{
	return nextDataLinePtr->dateTime;
}

int DataGenerator::GetNextUpdateMillisec() const
{
	return nextDataLinePtr->updateMillisec;
}

bool DataGenerator::ReachEOF() const
{
	if (fin)
	{
		return 0;
	}
	return 1;
}



void DataGenerator::UpdateData()
{
	if (!fin.is_open())
	{
		fin.close();
		string msg = "DataGenerator: UpdateData\nFile is not open";
		throw Error(msg);
	}

	string line;
	if (!nextDataLinePtr)
	{
		nextDataLinePtr = new DataLineT();
		GetNextDataLine(*nextDataLinePtr);
	}

	if (Simulator::GetInstance().GetCurrentTime() == nextDataLinePtr->dateTime && Simulator::GetInstance().GetCurrentMillisec() == nextDataLinePtr->updateMillisec)
	{
		if (data.size() == interval_c * 2)
		{
			data.erase(data.begin());
		}

		data.push_back(*nextDataLinePtr);
		GetNextDataLine(*nextDataLinePtr);
	}
}

void DataGenerator::CheckDataIntegrity(const string& filename)
{
	ifstream finCheck;
	finCheck.open(filename.c_str());
	if (!finCheck.is_open())
	{
		finCheck.close();
		string msg = "DataGenerator: CheckDataIntegrity\nCannot open file: " + filename;
	}

	string line;
	DataLineT lastData;
	string lastline;

	double missingSec(0);
	int duplicatedCnt(0), inconsistCnt(0), gapCnt(0);

	double maxGapSec(0);
	string maxGapLineA, maxGapLineB;
	int maxGapLineNumA(0), maxGapLineNumB(0);
	for (int lineCnt = 0; getline(finCheck, line); ++ lineCnt)
	{
		// Find Title and move on
		if (line.find("TradingDay") != string::npos)
		{
			continue;
		}

		// Read a line from data file
		DataLineT dataline;
		ReadLine(line, dataline);

		// skip the first data line
		if (lastline.empty())
		{
			lastData = dataline;
			lastline = line;
			continue;
		}

		// Compare new line with the previous line to check time consistancy
		double timeDiff = difftime(dataline.dateTime, lastData.dateTime);

		if (timeDiff < 0.01 && lastData.updateMillisec + dataline.updateMillisec != 500)
		{
			++ duplicatedCnt;
		}
		else if (fabs(timeDiff - 1) < 0.01 && lastData.updateMillisec + dataline.updateMillisec != 500 && !(lastData.volume - dataline.volume < 0.01))
		{
			++ inconsistCnt;
			missingSec += 1;
		}
		else if (fabs(timeDiff - market_closed_gap_c) < 0.01 && !(lastData.volume - dataline.volume < 0.01))
		{
			++ gapCnt;
			double gapSec = difftime(dataline.dateTime, lastData.dateTime) + (dataline.updateMillisec - lastData.updateMillisec) * 0.001;
			missingSec += gapSec;

			if (maxGapSec < gapSec)
			{
				maxGapLineA = lastline;
				maxGapLineNumA = lineCnt;
				maxGapLineB = line;
				maxGapLineNumB = lineCnt + 1;
			}
			maxGapSec = maxGapSec > gapSec ? maxGapSec : gapSec;
		}

		if (dataline.updateMillisec != 0 && dataline.updateMillisec != 500)
		{
			cout << "Incorrect UpdateMillisec: " << dataline.updateMillisec << endl;
			cout << dataline << endl;
		}

		lastData = dataline;
		lastline = line;
	}

	cout << "# Integrity Check Summary: " << filename << endl;
	cout << "# 		Number of Duplicated lines: " << duplicatedCnt << endl;
	cout << "#		Number of InConsist lines: " << inconsistCnt << endl;
	cout << "#		Number of Gaps: " << gapCnt << endl;
	cout << "# Total Missing Seconds: " << missingSec << endl;
	cout << "# Maximum Gap Seconds: " << maxGapSec << endl;
	cout << "# Maximum Gap Lines: " << endl;
	cout << "\tline#" << maxGapLineNumA << ": " << maxGapLineA << endl;
	cout << "\tline#" << maxGapLineNumB << ": " << maxGapLineB << endl;

	finCheck.close();
}



//*********************************************Private functions*********************************************//


void DataGenerator::OpenAndRead(const string& filename)
{
	fin.open(filename.c_str());

	if (!fin.is_open())
	{
		fin.close();
		string msg = "DataGenerator: OpenAndRead\nFile -" + filename + "- open failre";
		throw Error(msg);
	}

	DataLineT dataLine;

	string line;
	while (fin)
	{
		getline(fin, line);
		if (line.find("TradingDay") != string::npos)
		{
			continue;
		}

		if (line.empty())
		{
			continue;
		}

		ReadLine(line, dataLine);
		data.push_back(dataLine);
		break;
	}

	cout << line << endl;

	if (!data.size())
	{
		string msg = "DataGenerator: OpenAndRead\nNo data found in file -" + filename;
		throw Error(msg);
	}
}

void DataGenerator::GetNextDataLine(DataLineT& dataLine)
{
	string line;
	if (getline(fin, line))
	{
		ReadLine(line, dataLine);
	}
}

void DataGenerator::ReadLine(const string& line, DataLineT& dataLine)
{
	string date;
	string updateTime;

	istringstream iss (line);
	iss >> date >> dataLine.instrumentID >> dataLine.lastPrice >> dataLine.preSettle >> dataLine.preClose >> dataLine.preOpenInterest >> dataLine.openPrice >> dataLine.highestPrice;
	iss >> dataLine.lowestPrice >> dataLine.volume >> dataLine.turnover >> dataLine.openInterest >> dataLine.upperLimitPrice >> dataLine.lowerLimitPrice >> updateTime;
	iss >> dataLine.updateMillisec >> dataLine.bidPrice >> dataLine.bidVolume >> dataLine.askPrice >> dataLine.askVolume >> dataLine.averagePrice;

	string datetime;
	datetime = date.substr(0, 4) + "-" + date.substr(4, 2) + "-" + date.substr(6, 2) + " " + updateTime;
	dataLine.dateTime = ConvertStringToTime(datetime);
}
