#ifndef TIMER_H_
#define TIMER_H_

class Timer
{
public:
	Timer();

	// Increment time
	void Increment();

	// Accessors
	time_t GetCurrentTime() const;
	int GetCurrentMillisec() const;
	bool EndOfDay() const;

	friend std::ostream& operator<<(std::ostream& os, const Timer& timer);

private:
	/*******Priavete fucntions*********/
	void MidNightAdjustment();
	bool MarketClosed(time_t& updateTime);
	time_t GetTimeFromStrConst(const std::string& strConst);

	time_t GetStartTime() const;

	/*******Priavete members*********/
	time_t currentTime;
	int currentMillisec;

	bool overnight;
	bool endOfDay;
};

#endif /*TIMER_H_*/
