#include <sstream>
#include <algorithm>

#include "Constants.h"
#include "Account.h"
#include "Simulator.h"
#include "Log.h"

using namespace std;
Simulator& Simulator::GetInstance()
{
	static Simulator singleton;
	return singleton;
}

void Simulator::SetStrategy(Strategy* testStrategyPtr)
{
	strategyPtr = testStrategyPtr;

	mainLog << mainLog.GetLogDateTimeStr() << "Strategy Set." << endl;
}

void Simulator::Run(Account& account, DataGenerator& dataGenerator, vector<DataGenerator*>& refDataGenerators)
{
	if (!strategyPtr)
	{
		cout << "No strategy loaded." << endl;
		return ;
	}
	// Containers for Function Strategy->Run return
	vector<double> limitPriceVector;
	vector<int> lotsVector;
	vector<Operation_e> opVector;
	vector<int> cancelledOrderIDVector;

	vector<TradeOrderT> activeOrders;

	dataLines = dataGenerator.GetData();
	for (vector<DataGenerator*>::iterator it = refDataGenerators.begin(); it != refDataGenerators.end(); ++ it)
	{
		refDataLines.push_back((*it)->GetData());
	}

	LogMarketStatus();

	while (!timer.EndOfDay())
	{
		activeOrders = account.GetActiveOrders();
		// Clear return containers
		limitPriceVector.clear();
		lotsVector.clear();
		opVector.clear();
		cancelledOrderIDVector.clear();

		// Run strategy
		strategyPtr->RunData(
			dataLines, 
			refDataLines, 
			activeOrders, 
			account.GetPositionLong(), 
			account.GetPositionShort(), 
			limitPriceVector,
			lotsVector,
			opVector,
			cancelledOrderIDVector
		);

		// Send orders if any
		for (size_t i = 0; i < limitPriceVector.size(); ++ i)
		{
			account.SendOrder(limitPriceVector[i], lotsVector[i], opVector[i]);
		}

		// Cancel orders if any
		for (vector<int>::iterator it = cancelledOrderIDVector.begin(); it != cancelledOrderIDVector.end(); ++ it)
		{
			account.CancelOrder(*it);
		}

		// Match orders. Update activeOrders before Match
		activeOrders = account.GetActiveOrders();
		bool modified = Match(account, activeOrders);
		if (modified && 0)
		{
			mainLog << account << endl;
		}

		// Increment Time:
		timer.Increment();
		// Update data
		dataGenerator.UpdateData();
		dataLines = dataGenerator.GetData();
		size_t index = 0; 
		for (vector<DataGenerator*>::iterator it = refDataGenerators.begin(); it != refDataGenerators.end(); ++ it, ++ index)
		{
			(*it)->UpdateData();
			refDataLines[index] = (*it)->GetData();
		}
		LogMarketStatus();

		// Run strategy on current data.
	}
}

bool Simulator::Match(Account& account, const vector<TradeOrderT>& activeOrders)
{
	bool modified = 0;
	DataLineT latestLine = dataLines.back();
	for (vector<TradeOrderT>::const_iterator it = activeOrders.begin(); it != activeOrders.end(); ++ it)
	{
		if ((it->GetOp() == BUY || it->GetOp() == BUYTOCOVER) && it->GetPrice() >= latestLine.bidPrice)
		{
			account.MatchOrder(it->GetID(), 
				latestLine.askPrice, 
				it->GetLots() <= latestLine.bidVolume ? it->GetLots() : latestLine.bidVolume,
				timer.GetCurrentTime()
				);
			modified = 1;
		}

		if ((it->GetOp() == SELL || it->GetOp() == SELLSHORT) && it->GetPrice() <= latestLine.askPrice)
		{
			account.MatchOrder(it->GetID(), 
				latestLine.bidPrice, 
				it->GetLots() <= latestLine.askVolume? it->GetLots() : latestLine.askVolume,
				timer.GetCurrentTime()
				);
			modified = 1;
		}
	}

	return modified;
}

void Simulator::LogMarketStatus(int numLines)
{
	int cnt(numLines);
	//mainLog << mainLog.GetLogDateTimeStr() << "Market Status" << endl;
	mainLog << timer << endl;
	if (dataLines.empty())
	{
		mainLog << "\tNo Main Instrument Data" << endl;
	}
	else
	{
		mainLog << "\tMain Instrument Status: " << endl;
		for (vector<DataLineT>::reverse_iterator it = dataLines.rbegin(); it != dataLines.rend() && cnt != 0; ++ it, --cnt)
		{
			mainLog << "\t\t" << (*it) << endl;
		}
	}

	cnt = numLines;
	for (size_t i = 0; i != refDataLines.size(); ++ i)
	{
		if (refDataLines[i].empty())
		{
			mainLog << "No Data for ref instrument#" << i + 1 << endl;
		}
		else
		{
			mainLog << "\tRef Instrument#" << i + 1 << " Status: " << endl;
			for (vector<DataLineT>::reverse_iterator it = refDataLines[i].rbegin(); it != refDataLines[i].rend() && cnt != 0; ++ it, --cnt)
			{
				mainLog << "\t\t" << (*it) << endl;
			}
		}
	}
}

time_t Simulator::GetCurrentTime() const
{
	return timer.GetCurrentTime();
}

int Simulator::GetCurrentMillisec() const
{
	return timer.GetCurrentMillisec();
}

Log& Simulator::GetMainLog()
{
	return mainLog;
}

/*************************************************Private Functions**********************************************************/
Simulator::Simulator()
	: strategyPtr(0) 
{
	// Create log file with default log file name: log_date_createtime.txt
	mainLog.CommonLogInit();
}

Simulator::~Simulator()
{
}
