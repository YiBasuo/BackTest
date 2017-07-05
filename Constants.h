#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdlib>
#include <string>

/********************************Timer Constants*************************************/
static const std::string test_date_c = "2017-06-29";

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
static const int maximum_position_limit_c = 1;

/************************************************************************************/

// Used in Log
static std::string log_folder_path_c = "Log/";

/************************************************************************************/

// Used in Report
static std::string report_file_name_c = "Report/report.txt";

#endif /*CONSTANTS*/
