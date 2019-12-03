#pragma once

#include "common.h"
#include "util_string.h"

#include <chrono>
#include <sys/timeb.h>

namespace timeutil
{
	typedef uint64 Time;  // milliseconds from 1971

	inline Time AddSeconds(Time timeValue, int second)
	{
		return timeValue + (uint64)second * 1000;
	}

	inline Time AddMilliseconds(Time timeValue, int milliseconds)
	{
		return timeValue + milliseconds;
	}

	inline std::string ToString(uint64 timeTick)
	{
        const auto milli = timeTick % 1000;
        time_t tick = (time_t)(timeTick / 1000);
        struct tm* tm = localtime(&tick);
        return strutil::format("%d-%02d-%02d %02d:%02d:%02d.%03d",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, milli);
	}

}
