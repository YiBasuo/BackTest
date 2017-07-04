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
    time_t tNowTime;  
    time(&tNowTime);  
  
    tm* tLocalTime = localtime(&tNowTime);  
  
    string sDateStr = ValueToStr(tLocalTime->tm_year+1900) + "-" +  
        ValueToStr(tLocalTime->tm_mon+1) + "-" +  
        ValueToStr(tLocalTime->tm_mday);  
  
    return Open("Log/Log_" + sDateStr + ".log");  
}  
  
void Log::Enable()  
{  
    enabled = true;  
}  
  
void Log::Disable()  
{  
    enabled = false;  
}  
  
string Log::GetTimeStr()  
{  
    time_t tNowTime;  
    time(&tNowTime);  
  
    tm* tLocalTime = localtime(&tNowTime);  
  
    //"2011-07-18 23:03:01 ";  
    string strFormat = "%Y-%m-%d %H:%M:%S: ";  
  
    char strDateTime[30] = {'\0'};  
    strftime(strDateTime, 30, strFormat.c_str(), tLocalTime);  
  
    string strRes = strDateTime;  
  
    return strRes;  
}  
