#pragma once

#include "common.h"

namespace timeutil {

	typedef uint64 Time;  // milliseconds from 1971

	inline Time AddSeconds( Time timeValue, int second ) {
		return timeValue + second * 1000;
	}

	inline Time AddMilliseconds( Time timeValue, int milliseconds ) {
		return timeValue + milliseconds;
	}
}