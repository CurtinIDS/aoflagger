#include "timefrequencystatistics.h"

#include <cmath>
#include <string>
#include <sstream>

TimeFrequencyStatistics::TimeFrequencyStatistics(const TimeFrequencyData &data)
  : _data(data)
{
}

num_t TimeFrequencyStatistics::GetFlaggedRatio()
{
	size_t total = 0, flagged = 0;

	for(size_t i=0;i<_data.MaskCount();++i)
	{
		Mask2DCPtr mask = _data.GetMask(i);
		flagged += mask->GetCount<true>();
		total += mask->Width() * mask->Height();
	}
	if(total != 0)
		return (num_t) flagged / (num_t) total;
	else
		return 0;
}

std::string TimeFrequencyStatistics::FormatRatio(num_t ratio)
{
	std::stringstream s;
	if(ratio > 0.01)
		s << (round(ratio*10000.0)/100.0) << "%";
	else if(ratio > 0.001)
		s << (round(ratio*100000.0)/1000.0) << "%";
	else if(ratio > 0.0001)
		s << (round(ratio*1000000.0)/10000.0) << "%";
	else
		s << (ratio*100.0) << "%";
	return s.str();
}
