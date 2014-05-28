#include "filterbankset.h"

#include <fstream>

namespace rfiStrategy {

FilterBankSet::FilterBankSet(const std::string &location) :
	_location(location)
{
	std::ifstream file(_location.c_str());
	if(!file.good())
		throw std::runtime_error(std::string("Error opening filterbank file ") + _location);
	
	std::string keyword = readString(file);
	if(keyword != "HEADER_START")
		throw std::runtime_error("Filterbank file does not start with 'HEADER_START' -- corrupt file?");
	
	while(file.good() && keyword!="HEADER_END")
	{
		keyword = readString(file);
		
		if(keyword == "tsamp")
			_timeOfSample = readDouble(file);
		else if(keyword == "tstart")
			_timeStart = readDouble(file);
		else if(keyword == "fch1")
			_fch1 = readDouble(file);
		else if(keyword == "foff")
			_foff = readDouble(file);
		else if(keyword == "nchans")
			_channelCount = readInt(file);
		else if(keyword == "nifs")
			_ifCount = readInt(file);
		else if(keyword == "nbits")
			_bitCount = readInt(file);
		else if(keyword == "nsamples")
			_sampleCount = readInt(file);
		else if(keyword == "machine_id")
			_machineId = readInt(file);
	}
	std::cout <<
		"tsamp=" << _timeOfSample << ", tstart=" << _timeStart << ", fch1=" << _fch1 << ", foff=" << _foff << '\n' <<
		"nChans=" << _channelCount << ", nIFs=" << _ifCount << ", nBits=" << _bitCount << ", nSamples=" << _sampleCount
		<< "\nmachine_ID=" << _machineId << '\n';
}

void FilterBankSet::AddReadRequest(const ImageSetIndex& index)
{

}

void FilterBankSet::AddWriteFlagsTask(const ImageSetIndex& index, std::vector<Mask2DCPtr>& flags)
{
}

BaselineData* FilterBankSet::GetNextRequested()
{
	return 0; //TODO
}

void FilterBankSet::Initialize()
{
}

void FilterBankSet::PerformReadRequests()
{
}

void FilterBankSet::PerformWriteDataTask(const ImageSetIndex& index, std::vector<Image2DCPtr> realImages, std::vector<Image2DCPtr> imaginaryImages)
{
}

void FilterBankSet::PerformWriteFlagsTask()
{
}

void FilterBankSet::WriteFlags(const ImageSetIndex& index, TimeFrequencyData& data)
{
}

std::string FilterBankSetIndex::Description() const
{
	return "?";
}

void FilterBankSetIndex::Next()
{
}

void FilterBankSetIndex::Previous()
{
}

} //namespace
