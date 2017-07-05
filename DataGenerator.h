#ifndef DATAGENERATOR_H_
#define DATAGENERATOR_H_

#include <cmath>
#include <fstream>
#include <vector>
#include "Utility.h"
#include "Constants.h"


class DataGenerator
{
public:
	DataGenerator(int interval_, const std::string& filename_);
	~DataGenerator();

	// Remove the earliest line in data list
	// Read in and append the latest line to data list if there is any.
	void UpdateData(time_t currentTime, int currentMillisec);

	// Check DataIntegrity
	static void CheckDataIntegrity(const std::string& filename);

	interval_t GetInterval() const;
	std::vector<DataLineT> GetData() const;
	time_t GetNextDateTime() const;
	int GetNextUpdateMillisec() const;
	std::string GetFilename() const;

	bool ReachEOF() const;

private:
	//*******************Private functions************************//
	// Open a file and read in data during the first interval
	void OpenAndRead(const std::string& filename);

	// Convert a line in data file to DataLineT
	static void ReadLine(const std::string& line, DataLineT& dataLine);

	void GetNextDataLine(DataLineT& dataLine);

	// Disablecopy constructor and assign operator
	DataGenerator();
	DataGenerator(const DataGenerator&);
	DataGenerator& operator=(const DataGenerator&);
	
	//*********************Private variables*********************//
	std::vector<DataLineT> data;
	DataLineT* nextDataLinePtr;

	std::string filename;
	std::ifstream fin;

	const interval_t interval_c;
};

#endif /*DATAGENERATOR_H_*/
