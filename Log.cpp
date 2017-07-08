#include <sstream>
#include <ctime>
#include "Utility.h"
#include "Constants.h"
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
  
	string defaultLogFile = log_folder_path_c + "/Log_"
		+ DataFileName::GetInstance().GetTestInstrument() + "_" + DataFileName::GetInstance().GetTestDate() + ".log";
  
    return Open(defaultLogFile);  
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
