#pragma once

#include <cstdlib>

#define PSM_NM psm

#define _START_PSM_NM_ \
    namespace psm      \
    {

#define _END_PSM_NM_ \
    }

#define PSM_BAIL() do { ::std::quick_exit(-1); } while(0)

