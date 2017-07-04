#include "Constants.h"
#include "Account.h"
#include "Simulator.h"
#include "Log.h"

using namespace std;
Simulator::Simulator() : strategyPtr(0)
{
	// Create log file with default log file name: log_date_createtime.txt
	mainLog.CommonLogInit();
}

Simulator& Simulator::GetInstance()
{
	static Simulator singleton;
	return singleton;
}

void Simulator::SetStrategy(Strategy* testStrategyPtr)
{
	strategyPtr = testStrategyPtr;

	mainLog << mainLog.GetTimeStr() << "Strategy Set" << endl;
}

void Simulator::UpdateDataLines(DataGenerator& dataGenerator, vector<DataLineT>& dataLines, vector<DataLineT>& nextDataLines)
{
	if (nextDataLines.back().dateTime == currentTime && nextDataLines.back().updateMillisec == currentMillisec)
	{
		dataLines = nextDataLines;
		dataGenerator.UpdateData();
		nextDataLines = dataGenerator.GetData();
	}
}

void Simulator::Run(Account& account, DataGenerator& dataGenerator, vector<DataGenerator*> refDataGenerator)
{
	if (!strategyPtr)
	{
		cout << "No strategy loaded" << endl;
		return;
	}
	// Containers for Function Strategy->Run to return to
	vector<double> limitPriceVector;
	vector<int> lotsVector;
	vector<Operation_e> opVector;
	vector<int> cancelledOrderIDVector;

	// Active Orders and first dataline
	vector<TradeOrderT> activeOrders;

	vector<DataLineT> dataLines = dataGenerator.GetData();
	dataGenerator.UpdateData();
	vector<DataLineT> nextDataLines = dataGenerator.GetData();

	vector<vector<DataLineT> > refDataLines;
	vector<vector<DataLineT> > nextRefDataLines;
	for (vector<DataGenerator*>::iterator it = refDataGenerator.begin(); it != refDataGenerator.end(); ++ it)
	{
		refDataLines.push_back((*it)->GetData());
		(*it)->UpdateData();
		nextRefDataLines.push_back((*it)->GetData());
	} 
	// Set currentTime and currentMillisec
	currentTime = dataLines.back().dateTime;
	currentMillisec = dataLines.back().updateMillisec;

	tm *ptm = localtime(&currentTime);
	bool overnight = 0;

	while (!(ptm->tm_hour == 15 && ptm->tm_sec == 1))
	{
		UpdateDataLines(dataGenerator, dataLines, nextDataLines);
		for (size_t i = 0; i != refDataGenerator.size(); ++ i)
		{
			UpdateDataLines((*refDataGenerator[i]), refDataLines[i], nextRefDataLines[i]);
		}

		if (difftime(nextDataLines.back().dateTime, currentTime) > market_closed_gap_c)
		{
			currentTime = nextDataLines.back().dateTime;
			currentMillisec = 0;
			continue;
		}

		bool modified = 0;
		// Match orders and update accout since Data has been updated
		activeOrders = account.GetActiveOrders();
		modified = Match(account, dataLines, activeOrders);

		if (modified)
		{
//			cout << account << endl;
		}

		// Run strategy on updated data, Clear return containers before runing the strategy
		activeOrders = account.GetActiveOrders();
		limitPriceVector.clear();
		lotsVector.clear();
		opVector.clear();
		cancelledOrderIDVector.clear();

		strategyPtr->RunData(dataLines, refDataLines, activeOrders, account.GetPositionLong(), account.GetPositionShort(), limitPriceVector, lotsVector, opVector, cancelledOrderIDVector);

		// Send order according to the strategy return
		for (size_t i = 0; i < limitPriceVector.size(); ++ i)
		{
			account.SendOrder(limitPriceVector[i], lotsVector[i], opVector[i]);
//			cout << account << endl;
		}

		// Cancel order according to the strategy return
		for (vector<int>::iterator it = cancelledOrderIDVector.begin(); it != cancelledOrderIDVector.end(); ++ it)
		{
			account.CancelOrder(*it);
//			cout << account << endl;
		}

		// Match orders and update account again since active orders has been uppdated 
		modified = 0;
		activeOrders = account.GetActiveOrders();
		modified = Match(account, dataLines, activeOrders);
		if (modified)
		{
//			cout << account << endl;
		}
		
		// Update dataLines
		dataGenerator.UpdateData();
		dataLines = dataGenerator.GetData();
		for (vector<DataGenerator*>::const_iterator it = refDataGenerator.begin(); it != refDataGenerator.end(); ++ it)
		{
			(*it)->UpdateData();
		}

		// Increment time
		if (currentMillisec == 0)
		{
			currentMillisec = 500;
		}
		else if (currentMillisec == 500)
		{
			currentMillisec = 0;
			currentTime += 1;
		}

		ptm = localtime(&currentTime);
		if (ptm->tm_hour == 0 && ptm->tm_min == 0 && ptm->tm_sec == 0 && !overnight)
		{
			currentTime -= 86401;
			overnight = 1;
		}
	}
}

bool Simulator::Match(Account& account, const vector<DataLineT>& dataLines, const vector<TradeOrderT>& activeOrders)
{
	bool modified = 0;
	DataLineT latestLine = dataLines.back();
	for (vector<TradeOrderT>::const_iterator it = activeOrders.begin(); it != activeOrders.end(); ++ it)
	{
		if ((it->GetOp() == BUY || it->GetOp() == BUYTOCOVER) && it->GetPrice() >= latestLine.bidPrice)
		{
			account.MatchOrder(it->GetID(), 
				latestLine.bidPrice, 
				it->GetLots() <= latestLine.bidVolume ? it->GetLots() : latestLine.bidVolume,
				currentTime
				);
			modified = 1;
		}

		if ((it->GetOp() == SELL || it->GetOp() == SELLSHORT) && it->GetPrice() <= latestLine.askPrice)
		{
			account.MatchOrder(it->GetID(), 
				latestLine.askPrice, 
				it->GetLots() <= latestLine.askVolume? it->GetLots() : latestLine.askVolume,
				currentTime
				);
			modified = 1;
		}
	}

	return modified;
}

time_t Simulator::GetCurrentTime() const
{
	return currentTime;
}

int Simulator::GetCurrentMillisec() const
{
	return currentMillisec;
}
