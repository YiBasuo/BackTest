#include <string>
#include "Constants.h"
#include "Utility.h"

using namespace std;

DataFileName& DataFileName::GetInstance()
{
	static DataFileName singleton;
	return singleton;
}

void DataFileName::SetDataFileName(const string& datafile)
{
	if (datafile.find('_') == std::string::npos)
	{
		throw Error("DataFileName::SetDataFileName:\nInvalid Datafile Name");
	}

	filename = datafile;
	test_instrument = datafile.substr(0, datafile.find('_'));
	test_date = datafile.substr(datafile.find('_') + 1, datafile.length());
}

string DataFileName::GetFilename() const
{
	return filename;
}

string DataFileName::GetTestInstrument() const
{
	return test_instrument;
}

string DataFileName::GetTestDate() const
{
	return DataFileName::test_date;
}
