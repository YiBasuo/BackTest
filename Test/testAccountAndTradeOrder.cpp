#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>

#include "Utility.h"
#include "DataGenerator.h"
#include "TradeOrderT.h"
#include "Account.h"

using namespace std;


/*
class Strategy
{
public:
	Strategy() {};
	~Strategy() {};

	virtual TradeOrderT& RunData(list<DataLineT> datalist);
private:
};

class Simulator
{
public:
	static Simulator& GetInstance()
	{
		static Simulator singleton;
		return singleton;
	}

	static void SetStrategy(Strategy* testStrategyPtr)
	{
		strategyPtr = testStrategyPtr;
	}

	static void Run(DataGenerator& dataGenerator)
	{
		for (list<DataLineT> datalist = dataGenerator.GetData(); !datalist.empty(); datalist.Update())
		{
			TradeOrderT order = strategyPtr->RunData(datalist);
			if (order.op == noop)
			{
				continue;
			}

			orderHistory.push_back(order);
			MatchOrder(order);
		}
	}

	static void Run(DataGenerator& dataAlpha, DataGenerator& dataBeta)
	{
	}

private:
	Simulator()
	{
		strategyPtr = 0;
	}

	static Strategy* strategyPtr;


	static vector<TradeOrderT> orderHistory;

	// Simulator is a singlton
	~Simulator();
	Simulator(const Simulator& );
	Simulator& operator=(const Simulator&);
};*/

void QuickPrint(const TradeOrderT& order)
{
	cout << order << endl;
}


int main()
{
	try
	{
		//************************Create DataGenerator****************************//
		string filenameCu = "Data/cu1708_20170630";
		string filenameZn = "Data/zn1708_20170630";

		interval_t intervalCu(60);
		interval_t intervalZn(60);

		cout << "Loading data..." << endl;

		DataGenerator dataGeneratorCu(intervalCu, filenameCu);
		DataGenerator dataGeneratorZn(intervalZn, filenameZn);

		//************************Simulate********************************//
		
		Account account;
		account.SendOrder(2210, 5, BUY);
		account.SendOrder(2250, 100, SHORT);
		vector<TradeOrderT> history = account.GetHistoryOrders();
		vector<TradeOrderT> actives = account.GetActiveOrders();

		cout << "Printing all history orders..." << endl;
		for_each(history.begin(), history.end(), QuickPrint);

		cout << endl << "Printing all active orders..." << endl;
		for_each(actives.begin(), actives.end(), QuickPrint);

		cout << "------------------------------";
		cout << endl << "Cancelling order: " << endl;
		TradeOrderT order(history.back());
		cout << order << endl;
		account.CancelOrder(order);

		history = account.GetHistoryOrders();
		actives = account.GetActiveOrders();

		cout << "------------------------------";
		cout << endl << "Order cancelled. History Orders now: " << endl;
		for_each(history.begin(), history.end(), QuickPrint);
		cout << endl << "Order cancelled. Active Orders now: " << endl;
		for_each(actives.begin(), actives.end(), QuickPrint);

		cout << "#########################################################################" << endl;
		history = account.GetHistoryOrders();
		actives = account.GetActiveOrders();
		vector<TradeOrderT> tradeHis = account.GetHistoryTrade();
		cout << "Testing match function" << endl;
		cout << "History orders are:" << endl;
		for_each(history.begin(), history.end(), QuickPrint);
		cout << endl << "Active orders are:" << endl;
		for_each(actives.begin(), actives.end(), QuickPrint);
		cout << endl << "History Trade are:" << endl;
		for_each(tradeHis.begin(), tradeHis.end(), QuickPrint);

		cout << "------------------------------";
		cout << endl << "Start to match..." << endl;

		account.MatchOrder(actives.back(), 2250, 1);

		cout << "Order Matched!" << endl;
		cout << "------------------------------";

		history = account.GetHistoryOrders();
		actives = account.GetActiveOrders();
		tradeHis = account.GetHistoryTrade();

		cout << endl << "History orders are:" << endl;
		for_each(history.begin(), history.end(), QuickPrint);
		cout << endl << "Active orders are:" << endl;
		for_each(actives.begin(), actives.end(), QuickPrint);
		cout << endl << "History Trade are:" << endl;
		for_each(tradeHis.begin(), tradeHis.end(), QuickPrint);

		cout << "------------------------------";
		cout << endl << "Start to match again..." << endl;

		account.MatchOrder(actives.back(), 2299, 4);

		cout << "Order Matched!" << endl;
		cout << "------------------------------";
		history = account.GetHistoryOrders();
		actives = account.GetActiveOrders();
		tradeHis = account.GetHistoryTrade();

		cout << endl << "History orders are:" << endl;
		for_each(history.begin(), history.end(), QuickPrint);
		cout << endl << "Active orders are:" << endl;
		for_each(actives.begin(), actives.end(), QuickPrint);
		cout << endl << "History Trade are:" << endl;
		for_each(tradeHis.begin(), tradeHis.end(), QuickPrint);


	}
	catch (Error& e)
	{
		cout << "Error: " << endl;
		cout << e.GetMsg() << endl;
	}

	return 0;
}
