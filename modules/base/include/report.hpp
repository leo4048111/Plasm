#pragma once

#include "base.hpp"

#include <string>
#include <map>

_START_PSM_NM_

typedef struct _Report {
    ::std::string filename;        // Full name of the file
    ::std::string dump_path;      // Path to the dump file
    double start_time;            // Timestamp when the simulation starts
    double end_time;              // Timestamp when the simulation ends
    double duration;             // Duration of the process
    double memory;               // Memory usage
    ::std::map<::std::string, ::std::string> findings;  // Problems found
    ::std::map<::std::string, ::std::string> infos;     // Informational messages
    ::std::map<::std::string, ::std::string> errors;    // Errors encountered
    ::std::map<::std::string, ::std::string> fails;     // Failures
} Report;

_END_PSM_NM_