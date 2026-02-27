#include "containerui/timestamp.hpp"
#include <ctime>

namespace container_ui
{

uint64_t get_timestamp()
{
    timespec t;
    int const rc = clock_gettime(CLOCK_MONOTONIC, &t);
    if (rc != 0) {
        return 0;
    }

    return t.tv_sec;
}

}