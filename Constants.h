#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdlib>
#include <string>

/********************************Timer Constants*************************************/
static std::string test_date_c = "2017-06-29";

static std::string night_start_time_c = "20:59:00";
static std::string night_end_time_c = "01:00:01";

static std::string pre_break_start_time_c = "09:00:00";
static std::string pre_break_end_time_c = "10:15:01";

static std::string post_break_start_time_c = "10:30:00";
static std::string post_break_end_time_c = "11:30:01";

static std::string pm_start_c = "13:30:00";
static std::string pm_end_time_c = "15:00:01";

static int day_seconds_adjustor_c = 86400;

/************************************************************************************/

static const size_t max_time_string_size_c = 30;

typedef size_t interval_t ;
static const interval_t data_time_duration_c = 86400;

static const interval_t market_closed_gap_c = 300;

static const int contract_size_c = 5;

static std::string log_folder_path_c = "Log/";

#endif /*CONSTANTS*/
