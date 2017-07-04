#ifndef LOG_H_
#define LOG_H_

#include <fstream>  
#include <string>  
#include <sstream>  
#include <ctime>  
  
class Log  
{  
public:  
    Log();  
    ~Log();  
  
    bool Open(std::string strFileName);  
    void Close();  
  
    bool CommonLogInit();  
  
    void Enable();  
    void Disable();  
  
    std::string GetTimeStr();  

    template <typename T> Log& operator<<(const T& value)  
    {  
        if (enabled)  
        {  
            logFout << value;  
        }  
        return (*this);  
    }  
    Log& operator<<(std::ostream& (*_Pfn)(std::ostream&))  
    {  
        if (enabled)  
        {  
            (*_Pfn)(logFout);  
        }  
        return (*this);  
    }  

private:  
    template<typename T> std::string ValueToStr(T value)  
    {  
        std::ostringstream ost;  
        ost << value;  
        return ost.str();  
    }  
private:  
    std::ofstream logFout;  
  
    bool enabled;  
};  

#endif
