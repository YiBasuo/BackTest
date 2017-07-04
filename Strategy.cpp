#include "Strategy.h"

using namespace std;

Strategy::Strategy()
	: defaultLots(1) 
	{
	}

Strategy::~Strategy()
{
}

int Strategy::GetDefaultLots()
{
	return defaultLots;
}
