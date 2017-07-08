#include <list>
#include "Report.h"

using namespace std;

Report::Report(const std::string& dataFilename)
	: dataGenerator(1, dataFilename)
{
	string outputPath = report_folder_path_c + "report_" + DataFileName::GetInstance().GetTestInstrument() + "_" + DataFileName::GetInstance().GetTestDate();

	fout.open(outputPath.c_str());

	if (!fout.is_open())
	{
		string msg = "Cannot open report output file: " + outputPath;
		throw Error(msg);
	}
}

Report::~Report()
{
	if (fout.is_open())
	{
		fout.close();
	}
}

void Report::GenerateReport(const vector<TradeOrderT>& matchedOrders)
{
	DataLineT currentData = dataGenerator.GetData().back();
	Timer timer;
	while (!timer.EndOfDay())
	{
		timer.Increment();

		UpdateTrackerDataLine(currentData);

		vector<TradeOrderT>::const_iterator it = matchedOrders.begin();
		for (;it != matchedOrders.end(); ++ it)
		{
			if (it->GetTime() == timer.GetCurrentTime() && it->GetMillisec() == timer.GetCurrentMillisec())
			{
				if (it->GetOp() == BUY || it->GetOp() == SELLSHORT)
				{
					AddOrderToTracker(*it, currentData);
				}

				if (it->GetOp() == SELL || it->GetOp() == BUYTOCOVER)
				{
					RmOrderFromTracker(*it, currentData);
				}
			}
		}

		dataGenerator.UpdateData(timer.GetCurrentTime(), timer.GetCurrentMillisec());
		currentData = dataGenerator.GetData().back();
	}

	Render();
}

void Report::GenerateReport(const Account& account)
{
	GenerateReport(account.GetHistoryTrade());

	fout << "Account Summary: " << endl;
	fout << account << endl;
}

void Report::Render()
{
	double accumulatedProfit = 0;
	int profitableCnt = 0;
	for (vector<ReportLineT>::iterator it = reportData.begin(); it != reportData.end(); ++ it)
	{
		Render(*it);
		accumulatedProfit += ComputeProfit(*it);
		fout << "Accumulated Profit: " << accumulatedProfit << endl;
		fout << "----------------------------------------------------------------------------" << endl;
		profitableCnt += ComputeMaxFloatProfit(*it) > 0 ? 1 : 0;
	}

	fout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	fout << "Total Number of TranX: " << reportData.size() << endl;
	fout << "Total Profitable TransX: " << profitableCnt << endl;
	fout << "Profitable Ratio: " << profitableCnt * 1.0 / reportData.size() << endl;
	fout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl << endl;
}

/**************************Protected Functions**********************************/
void Report::Render(const ReportLineT& line)
{

	fout << "Open@~  ";
	Render(line.openOrder);
	fout << '\t' << "Max Price@~ ";
	Render(line.maxPriceDataLine);
	fout << '\t' << "Min Price@~ ";
	Render(line.minPriceDataLine);
	fout << "Closed@~";
	Render(line.closeOrder);
	fout << "TransX Profit:    " << ComputeProfit(line) << endl;
	fout << "Max Float Profit: "  << ComputeMaxFloatProfit(line) << endl;
	fout << "Max Float Loss:   "  << ComputeMaxFloatLoss(line) << endl;
}

void Report::Render(const TradeOrderT& order)
{
	fout << "Order id#" << order.id << " Time: " << GetDateTimeStr(order.orderTime) << " ";
	if (order.op == BUY)
	{
		fout << "BUY	   ";
	}
	if (order.op == SELL)
	{
		fout << "SELL	   ";
	}
	if (order.op == SELLSHORT)
	{
		fout << "SELLSHORT ";
	}
	if (order.op == BUYTOCOVER)
	{
		fout << "BUYTOCOVER";
	}

	fout << " " << order.lots << " at " << order.price << endl;
}

void Report::Render(const DataLineT& line)
{
	fout << GetDateTimeStr(line.dateTime);
	if (line.updateMillisec == 0)
	{
		fout << ".0";
	}
	if (line.updateMillisec == 500)
	{
		fout << ".5";
	}

	fout << " " << line.lastPrice << " | " << line.bidPrice << " " << line.bidVolume << " | " << line.askPrice << " " << line.askVolume << endl;
}

double Report::ComputeMaxFloatProfit(const ReportLineT& line)
{
	return (line.openOrder.op == BUY) ? 
		(line.maxPriceDataLine.lastPrice - line.openOrder.price) * contract_size_c
		: (line.openOrder.price - line.minPriceDataLine.lastPrice) * contract_size_c;
}
double Report::ComputeMaxFloatLoss(const ReportLineT& line)
{
	return (line.openOrder.op == BUY) ?
		(line.minPriceDataLine.lastPrice - line.openOrder.price) * contract_size_c
		: (line.openOrder.price - line.maxPriceDataLine.lastPrice) * contract_size_c;
}

double Report::ComputeProfit(const ReportLineT& line)
{
	return (line.openOrder.op == BUY) ? 
		(line.closeOrder.price - line.openOrder.price) * contract_size_c
		: (line.openOrder.price - line.closeOrder.price) * contract_size_c;
}

void Report::AddOrderToTracker(const TradeOrderT& order, const DataLineT& currentLine)
{
	unliqOrderTrackerVector.push_back(order);
	maxTrackerMap.insert(pair<TradeOrderT, DataLineT>(order, currentLine));
	minTrackerMap.insert(pair<TradeOrderT, DataLineT>(order, currentLine));
}

void Report::RmOrderFromTracker(const TradeOrderT& closeOrder, const DataLineT& currentLine)
{
	list<int> eraseIndexVector;
	TradeOrderT order(closeOrder);
	for (vector<TradeOrderT>::iterator it = unliqOrderTrackerVector.begin(); it != unliqOrderTrackerVector.end(); ++ it)
	{
		if ((it->GetOp() == BUY && order.GetOp() == SELL) || (it->GetOp() == SELLSHORT && order.GetOp() == BUYTOCOVER))
		{
			ReportLineT reportLine;
			// If the liquidating order has the same amount as the first opening order in queue, add report and break
			if (order.GetLots() == it->GetLots())
			{
				reportLine.openOrder = *it;
				reportLine.closeOrder = order;
				reportLine.maxPriceDataLine = maxTrackerMap[*it];
				reportLine.minPriceDataLine = minTrackerMap[*it];

				reportData.push_back(reportLine);
				eraseIndexVector.push_back(it - unliqOrderTrackerVector.begin());
				break;
			}
			// If the liquidating order has less amount than the first opening order in queue
			// Consider part of the order in queue with the liquidating order as 1 transx.
			// Add data to report and break
			if (order.GetLots() < it->GetLots())
			{
				reportLine.openOrder = *it;
				reportLine.openOrder.lots = it->GetLots();
				reportLine.closeOrder = order;
				reportLine.maxPriceDataLine = maxTrackerMap[*it];
				reportLine.minPriceDataLine = minTrackerMap[*it];

				reportData.push_back(reportLine);
				eraseIndexVector.push_back(it - unliqOrderTrackerVector.begin());
				break;
			}
			// If the liquidating order has more lots than the first opening order in queue
			// Consider part of the liquidating order with the first order in queue as 1 transX.
			// Add the data to report and move onto the next order in queue
			if (order.GetLots() > it->GetLots())
			{
				reportLine.openOrder = *it;
				order.lots -= it->GetLots();
				reportLine.closeOrder = order;
				reportLine.closeOrder.lots = it->GetLots();
				reportLine.maxPriceDataLine = maxTrackerMap[*it];
				reportLine.minPriceDataLine = minTrackerMap[*it];

				reportData.push_back(reportLine);
				eraseIndexVector.push_back(it - unliqOrderTrackerVector.begin());
			}
		}
	}

	for (list<int>::iterator it = eraseIndexVector.begin(); it != eraseIndexVector.end(); ++ it)
	{
		maxTrackerMap.erase(unliqOrderTrackerVector[*it]);
		minTrackerMap.erase(unliqOrderTrackerVector[*it]);
		unliqOrderTrackerVector.erase(unliqOrderTrackerVector.begin() + (*it));
	}
}

void Report::UpdateTrackerDataLine(const DataLineT& currentLine)
{
	for (vector<TradeOrderT>::iterator it = unliqOrderTrackerVector.begin(); it != unliqOrderTrackerVector.end(); ++ it)
	{
		maxTrackerMap[*it] = (maxTrackerMap[*it].lastPrice > currentLine.lastPrice) ? maxTrackerMap[*it] : currentLine;
		minTrackerMap[*it] = (minTrackerMap[*it].lastPrice < currentLine.lastPrice) ? minTrackerMap[*it] : currentLine;
	}
}



/**************************Private Functions**********************************/

