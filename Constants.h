#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <cstdlib>
#include <string>


static const size_t max_time_string_size_c = 30;

typedef size_t interval_t ;
static const interval_t data_time_duration_c = 86400;

static const interval_t market_closed_gap_c = 300;

static const int contract_size_c = 5;

static std::string log_folder_path_c = "Log/";

#endif /*CONSTANTS*/
