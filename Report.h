#ifndef REPORT_H_
#define REPORT_H_

#include <map>
#include <vector>
#include <string>
#include "Timer.h"
#include "DataGenerator.h"
#include "Utility.h"
#include "TradeOrderT.h"

class Report
{
public:
	Report(const std::string& dataFilename);
	~Report();

	virtual void GenerateReport(const std::vector<TradeOrderT>& matchedOrders);

	virtual void Render();
protected:
	struct ReportLineT{
		TradeOrderT openOrder;
		TradeOrderT closeOrder;
	
		DataLineT maxPriceDataLine;
		DataLineT minPriceDataLine;
	};


	virtual void Render(const DataLineT& line);
	virtual void Render(const TradeOrderT& order);
	virtual void Render(const ReportLineT& line);

	double ComputeMaxFloatProfit(const ReportLineT& line);
	double ComputeMaxFloatLoss(const ReportLineT& line);

	double ComputeProfit(const ReportLineT& line);
private:
	Report();
	Report(const Report&);
	Report& operator=(const Report&);

	void AddOrderToTracker(const TradeOrderT& order, const DataLineT& currentLine);
	void RmOrderFromTracker(const TradeOrderT& order, const DataLineT& currentLine);
	void UpdateTrackerDataLine(const DataLineT& currentLine);


private:
	DataGenerator dataGenerator;
	Timer timer;
	
	std::vector<TradeOrderT> unliqOrderTrackerVector;
	std::map<TradeOrderT, DataLineT> maxTrackerMap;
	std::map<TradeOrderT, DataLineT> minTrackerMap;
	std::vector<ReportLineT> reportData;

	std::ofstream fout;
};

#endif /*REPORT_H_*/
