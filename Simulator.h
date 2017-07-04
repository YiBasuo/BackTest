#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <vector>
#include <list>
#include "Utility.h"
#include "TradeOrderT.h"
#include "Strategy.h"
#include "DataGenerator.h"
#include "Log.h"

class Simulator
{
public:
	// Note: Mayers singleton is NOT thread safe
	static Simulator& GetInstance();

	void SetStrategy(Strategy* testStrategyPtr);

	void Run(Account& account, DataGenerator& dataGenerator, std::vector<DataGenerator*> refDataGeneratorVector);

	bool Match(Account& account, const std::vector<DataLineT>& dataLines, const std::vector<TradeOrderT>& activeOrders);

	// Accesser:
	time_t GetCurrentTime() const;
	int GetCurrentMillisec() const;

private:
	Simulator();
	void UpdateDataLines(DataGenerator& dataGenerator, std::vector<DataLineT>& dataLines, std::vector<DataLineT>& nextDataLines);

	Strategy* strategyPtr;
	time_t currentTime;
	int currentMillisec;
	Log mainLog;

	// Simulator is a singlton
	~Simulator() {}
	Simulator(const Simulator& );
	Simulator& operator=(const Simulator&);
};

#endif /*SIMULATOR_H_*/
