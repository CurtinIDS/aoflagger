#ifndef TIMEFREQUENCYSTATISTICS_H
#define TIMEFREQUENCYSTATISTICS_H

#include <string>

#include "../../structures/timefrequencydata.h"

class TimeFrequencyStatistics
{
	public:
		TimeFrequencyStatistics(const TimeFrequencyData &data);

		num_t GetFlaggedRatio();

		static std::string FormatRatio(num_t ratio);

	private:
		TimeFrequencyData _data;
};

#endif // TIMEFREQUENCYSTATISTICS_H
