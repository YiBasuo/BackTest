#include <iostream>

#include <ctime>
#include <sstream>
#include "Constants.h"
#include "Utility.h"
#include "Timer.h"

using namespace std;

/***************************************Public functions**************************************/

Timer::Timer() 
	: currentTime(GetTimeFromStrConst(night_start_time_c))
	, currentMillisec(0)
	, overnight(0)
	, endOfDay(0)
{
}

void Timer::Increment()
{
	if (currentMillisec != 0 && currentMillisec != 500)
	{
		string msg = "Timer::Increment\n Unknown milliseconds: " + ValueToStr(currentMillisec);
		throw Error(msg);
	}

	currentTime += (currentMillisec == 0) ? 0 : 1;
	currentMillisec = (currentMillisec == 0) ? 500 : 0;

	MidNightAdjustment();
	
	time_t updateTime;
	if (MarketClosed(updateTime))
	{
		currentTime = updateTime;
		currentMillisec = 0;
	}
}

void Timer::Reset()
{
	currentTime = GetTimeFromStrConst(night_start_time_c);
	currentMillisec = 0;
	overnight = 0;
	endOfDay = 0;
}

time_t Timer::GetCurrentTime() const
{
	return currentTime;
}

int Timer::GetCurrentMillisec() const
{
	return currentMillisec;
}

bool Timer::EndOfDay() const
{
	return endOfDay;
}

ostream& operator<<(ostream& os, const Timer& timer)
{
	//os << "Timer Read: " << GetDateTimeStr(timer.currentTime) << " Milliseconds: " << timer.currentMillisec;
	os  << GetDateTimeStr(timer.currentTime) << " Milliseconds: " << timer.currentMillisec;
	return os;
}

/***************************************Private functions**************************************/
time_t Timer::GetTimeFromStrConst(const string& strConst)
{
	string dateTimeStr(DataFileName::GetInstance().GetTestDate());
	dateTimeStr = dateTimeStr.substr(0, 4) + "-" + dateTimeStr.substr(4, 2) + "-" + dateTimeStr.substr(6, 2) + " " + strConst;
	time_t dateTime = ConvertStringToTime(dateTimeStr);
	return dateTime;
}

bool Timer::MarketClosed(time_t& updateTime)
{
	time_t trueTime(currentTime);
	trueTime -= (overnight) ? 0 : day_seconds_adjustor_c;

	time_t pmStartTime = GetTimeFromStrConst(pm_start_c);
	time_t pmEndTime = GetTimeFromStrConst(pm_end_time_c);

	if (trueTime <= pmEndTime && trueTime >= pmStartTime)
	{
		return 0;
	}
	if (trueTime > pmEndTime)
	{
		updateTime = currentTime;
		endOfDay = 1;
		return 1;
	}

	time_t postBreakStartTime = GetTimeFromStrConst(post_break_start_time_c);
	time_t postBreakEndTime = GetTimeFromStrConst(post_break_end_time_c);
	if (trueTime <= postBreakEndTime && trueTime >= postBreakStartTime)
	{
		return 0;
	}
	if (trueTime > postBreakEndTime)
	{
		updateTime = pmStartTime;
		return 1;
	}


	time_t preBreakStartTime = GetTimeFromStrConst(pre_break_start_time_c);
	time_t preBreakEndTime = GetTimeFromStrConst(pre_break_end_time_c);
	if (trueTime <= preBreakEndTime && trueTime >= preBreakStartTime)
	{
		return 0;
	}
	if (trueTime > preBreakEndTime)
	{
		updateTime = postBreakStartTime;
		return 1;
	}

	time_t nightStartTime = GetTimeFromStrConst(night_start_time_c);
	nightStartTime -= day_seconds_adjustor_c;

	time_t nightEndTime = GetTimeFromStrConst(night_end_time_c);
	bool longNightMarket = (nightEndTime > GetTimeFromStrConst("0:0:0")) ? 1 : 0;

	nightEndTime -= (longNightMarket) ? 0 : day_seconds_adjustor_c;

	if (currentTime <= nightEndTime && trueTime >= nightStartTime)
	{
		return 0;
	}
	if (trueTime > nightEndTime)
	{
		updateTime = preBreakStartTime;
		return 1;
	}
	if (trueTime < nightStartTime)
	{
		updateTime = nightStartTime;
		return 1;
	}
		
	return 0;
}

void Timer::MidNightAdjustment()
{
	tm* ptm = localtime(&currentTime);
	if (ptm->tm_hour == 0 && ptm->tm_min == 0 && ptm->tm_sec == 0 && !overnight)
	{
		currentTime -= day_seconds_adjustor_c;
		overnight = 1;
	}
}
