#include <sstream>
#include <ctime>
#include "Utility.h"
#include "Log.h"

using namespace std;
  
Log::Log()  
    :enabled(true)  
{  
}  
  
Log::~Log()  
{  
	Close();
}  
  
bool Log::Open(string sFileName)  
{  
    logFout.open(sFileName.c_str(), ios_base::out | ios_base::app);  
  
    if( !logFout )  
    {  
        return false;  
    }  
  
    return true;  
}  
  
void Log::Close()  
{  
    if(logFout.is_open())  
    {  
        logFout.close();  
    }  
}  
  
bool Log::CommonLogInit()  
{  
    time_t nowTime;
    time(&nowTime);
  
    /*string sDateStr = ValueToStr(tLocalTime->tm_year+1900) + "-" +  
        ValueToStr(tLocalTime->tm_mon+1) + "-" +  
        ValueToStr(tLocalTime->tm_mday);  */
	string dateTime = GetDateTimeStr(nowTime);
  
    return Open("Log/Log_" + dateTime + ".log");  
}  
  
void Log::Enable()  
{  
    enabled = true;  
}  
  
void Log::Disable()  
{  
    enabled = false;  
}  
  
string Log::GetLogDateTimeStr()  
{  
    time_t nowTime;  
    time(&nowTime);  

	string logDateTimeStr = "Log Time~: " + GetDateTimeStr(nowTime) + ": ";
  
    return logDateTimeStr;  
}  
