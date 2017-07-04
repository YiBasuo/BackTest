#include <iostream>
#include <list>

#include "DataGenerator.h"

using namespace std;

int main()
{
	try
	{
		string filenameCu = "Data/cu1708_20170630";
		interval_t intervalCu(10);
		DataGenerator dataGeneratorCu(intervalCu, filenameCu);

		cout << "-----------------------Update data-----------------------" << endl;
		dataGeneratorCu.UpdateData();
		list<DataLineT> datalist = dataGeneratorCu.GetData();

		datalist.back().printAll();
	}
	catch (Error& e)
	{
		cout << e.GetMsg() << endl;
	}

	return 0;
}
