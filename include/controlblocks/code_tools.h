
#pragma once

#include <iostream>

#define CB_CALL(res)                                                           \
    if (!res)                                                                  \
    {                                                                          \
        std::cerr << "CALL FAIL: " << __LINE__ << std::endl;                   \
        return false;                                                          \
    }

#define CB_CHECK(check)                                                        \
    if (!check)                                                                \
    {                                                                          \
        std::cerr << "CHECK FAIL: " << __LINE__ << std::endl;                  \
        return false;                                                          \
    }

#define CB_CHECKMSG(check, msg)                                                \
    if (!check)                                                                \
    {                                                                          \
        std::cerr << "CHECK FAIL: " << __LINE__ << " " << msg << std::endl;    \
        return false;                                                          \
    }
