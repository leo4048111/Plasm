#pragma once

#include "base.hpp"

#include <string>
#include <map>

_START_PSM_NM_

typedef struct _Report {
    ::std::string filename{""};         // Full name of the file
    ::std::string dump_path{""};        // Path to the dump file
    double start_time{0.f};              // Timestamp when the simulation starts
    double end_time{0.f};                // Timestamp when the simulation ends
    double duration{0.f};                // Duration of the process
    double memory{0.f};                  // Memory usage
    ::std::string initial_hash{"0x0"};     // Initial hash of the file
    ::std::string expect_hash{"0x0"};      // Hash of the file after the process
    bool hash_mismatch{false};      // Hash mismatch flag
    ::std::string error_info{""};       // Error information
} Report;

_END_PSM_NM_