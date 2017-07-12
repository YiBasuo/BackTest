#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdlib>
#include <string>

/********************************Timer Constants*************************************/
class DataFileName
{
public:
	static DataFileName& GetInstance();

	void SetDataFileName(const std::string& datafile);
	std::string GetFilename() const;
	std::string GetTestInstrument() const;
	std::string GetTestDate() const;
private:
	DataFileName() {};
	DataFileName(const DataFileName&);
	DataFileName& operator=(const DataFileName&);

private:
	std::string filename;
	std::string test_instrument;
	std::string test_date;
};

//static const std::string test_instrument_c = "cu1708";
//static const std::string test_date_c = "20170629";

static const std::string night_start_time_c = "20:59:00";
static const std::string night_end_time_c = "01:00:01";

static const std::string pre_break_start_time_c = "09:00:00";
static const std::string pre_break_end_time_c = "10:15:01";

static const std::string post_break_start_time_c = "10:30:00";
static const std::string post_break_end_time_c = "11:30:01";

static const std::string pm_start_c = "13:30:00";
static const std::string pm_end_time_c = "15:00:01";

static const int day_seconds_adjustor_c = 86400;

/************************************************************************************/

// Used in Utility
// to check string size before convert it to time_t
static const size_t max_time_string_size_c = 30;

/************************************************************************************/

// Used in DataGenerator
// to accomplish Integrity Check
typedef size_t interval_t;
static const interval_t market_closed_gap_c = 300;

/************************************************************************************/

// Used in Account
// to calculate assets change
static const int contract_size_c = 5;
// to limit the number of positions holding at the same time
static const int maximum_position_limit_c = 100000000;

/************************************************************************************/

// Used in Reading Data
static std::string default_data_file_summary_c = "DataFileSummary.txt";

static std::string data_folder_path_c = "Data/";
/************************************************************************************/

// Used in Log
static std::string log_folder_path_c = "Log/";

/************************************************************************************/

// Used in Report
static std::string report_folder_path_c = "Report/";

#endif /*CONSTANTS*/
