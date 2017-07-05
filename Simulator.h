#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <vector>
#include <list>
#include "Utility.h"
#include "TradeOrderT.h"
#include "Strategy.h"
#include "Report.h"
#include "DataGenerator.h"
#include "Timer.h"
#include "Log.h"

class Simulator
{
public:
	// Note: Mayers singleton is NOT thread safe
	static Simulator& GetInstance();

	void Run(Account& account, DataGenerator& dataGenerator, std::vector<DataGenerator*>& refDataGeneratorVector);

	bool Match(Account& account, const std::vector<TradeOrderT>& activeOrders);

	void GenerateReport(const std::vector<TradeOrderT>& matchedOrders);

	// Accesser:
	void SetReport(Report* reportPtr);
	void SetStrategy(Strategy* testStrategyPtr);

	time_t GetCurrentTime() const;
	int GetCurrentMillisec() const;

	Log& GetMainLog();
	// Log current market status
	void LogMarketStatus(int numLines = 1);

private:
	//**************Private Functions**************//
	Simulator();

	// Update the market status
	void UpdateDataLines(DataGenerator& dataGenerator, std::vector<DataLineT>& dataLines, std::vector<DataLineT>& nextDataLines);

	// Simulator is a singlton
	~Simulator();
	Simulator(const Simulator& );
	Simulator& operator=(const Simulator&);

	//**************Private Members**************//

	Strategy* strategyPtr;
	Report* reportPtr;
	// Timer is used to simulate the time change of a real trading day
	Timer timer;

	// Current market status for main instrument & reference instruments
	std::vector<DataLineT> dataLines;
	std::vector<std::vector<DataLineT> > refDataLines;

	Log mainLog;
};

#endif /*SIMULATOR_H_*/
