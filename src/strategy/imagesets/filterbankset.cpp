#include "filterbankset.h"

#include "../../msio/date.h"
#include "../../msio/system.h"

#include <fstream>

#include <sys/types.h>
#include <sys/sysctl.h>

namespace rfiStrategy {

FilterBankSet::FilterBankSet(const std::string &location) :
	_location(location),
	_timeOfSample(0.0), _timeStart(0.0), _fch1(0.0), _foff(0.0),
	_channelCount(0), _ifCount(0), _bitCount(0), _sampleCount(0),
	_nBeams(0), _iBeam(0),
	_machineId(0),
	_intervalCount(0),
	_headerEnd(0)
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
		else if(keyword == "telescope_id")
			_telescopeId = readInt(file);
		else if(keyword == "nbeams")
			_nBeams = readInt(file);
		else if(keyword == "ibeam")
			_iBeam = readInt(file);
		else if(keyword == "src_raj" || keyword == "src_dej" || keyword == "az_start" || keyword == "za_start" ||
			keyword == "refdm" || keyword == "period")
			readDouble(file);
		else if(keyword == "data_type" || keyword == "barycentric" || keyword == "pulsarcentric")
			readInt(file);
	}
	_headerEnd = file.tellg();
	if(_sampleCount == 0)
	{
		file.seekg(0, std::ios::end);
		std::streampos endPos = file.tellg();
		size_t dataSize = endPos - _headerEnd;
		_sampleCount = (dataSize * 8) / _channelCount / _bitCount;
	}
	AOLogger::Debug <<
		"tsamp=" << _timeOfSample << ", tstart=" << _timeStart << ", fch1=" << _fch1 << ", foff=" << _foff << '\n' <<
		"nChans=" << _channelCount << ", nIFs=" << _ifCount << ", nBits=" << _bitCount << ", nSamples=" << _sampleCount
		<< "\nmachine_ID=" << _machineId << ", telescope_ID=" << _telescopeId << '\n';
	
	_timeStart = Date::MJDToAipsMJD(_timeStart);
	
	double sizeOfImage = double(_channelCount) * _sampleCount * _bitCount / 8.0;
	long int pageCount = sysconf(_SC_PHYS_PAGES), pageSize = sysconf(_SC_PAGE_SIZE);
	double memSize = System::TotalMemory();
	_intervalCount = ceil(sizeOfImage / (memSize / 16.0));
	if(_intervalCount < 1) _intervalCount = 1;
	if(_intervalCount*8 > _sampleCount) _intervalCount = _sampleCount/8;
	AOLogger::Debug << round(sizeOfImage*1e-8)*0.1 << " GB/image required of total of " << round(memSize*1e-8)*0.1 << " GB of mem, splitting in " << _intervalCount << " intervals\n";
}

void FilterBankSet::AddReadRequest(const ImageSetIndex& index)
{
	_requests.push_back(new BaselineData(index));
}

BaselineData* FilterBankSet::GetNextRequested()
{
	if(_bitCount != 32)
		throw std::runtime_error("Only support for 32-bit filterbank sets has been added as of yet");
	
	BaselineData* baseline = _requests.front();
	_requests.pop_front();
	const size_t intervalIndex = reinterpret_cast<const FilterBankSetIndex&>(baseline->Index())._intervalIndex;
	
	const size_t
		startIndex = (_sampleCount * intervalIndex) / _intervalCount,
		endIndex = (_sampleCount * (intervalIndex+1)) / _intervalCount;
	
	std::ifstream file(_location.c_str());
	file.seekg(_headerEnd + std::streampos(startIndex * sizeof(float) * _channelCount));
	
	Image2DPtr image = Image2D::CreateUnsetImagePtr(endIndex - startIndex, _channelCount);
	Mask2DPtr mask = Mask2D::CreateUnsetMaskPtr(endIndex - startIndex, _channelCount);
	std::vector<float> buffer(_channelCount);
	for(size_t x=0; x!=endIndex - startIndex; ++x)
	{
		file.read(reinterpret_cast<char*>(&buffer[0]), _channelCount*sizeof(float));
		for(size_t y=0; y!=_channelCount; ++y)
		{
			image->SetValue(x, y, buffer[y]);
			mask->SetValue(x, y, !std::isfinite(buffer[y]));
		}
	}
	TimeFrequencyData tfData(TimeFrequencyData::AmplitudePart, StokesIPolarisation, image);
	tfData.SetGlobalMask(mask);
	TimeFrequencyMetaDataPtr metaData(new TimeFrequencyMetaData());
	AntennaInfo antenna;
	antenna.diameter = 0;
	antenna.id = 0;
	antenna.mount = "unknown";
	antenna.name = "unknown";
	antenna.position = EarthPosition();
	antenna.station = "unknown";
	metaData->SetAntenna1(antenna);
	metaData->SetAntenna2(antenna);
	BandInfo band;
	band.windowIndex = 0;
	for(size_t ch=0; ch!=_channelCount; ++ch)
	{
		ChannelInfo channel;
		channel.frequencyHz = 1e6 * (_fch1 + _foff * ch);
		channel.effectiveBandWidthHz = 1e6 * std::fabs(_foff);
		channel.frequencyIndex = ch;
		channel.channelWidthHz = 1e6 * std::fabs(_foff);
		channel.resolutionHz = 1e6 * std::fabs(_foff);
		band.channels.push_back(channel); 
	}
	metaData->SetBand(band);
	std::vector<double> observationTimes(endIndex - startIndex);
	for(size_t t=startIndex; t!=endIndex; ++t)
		observationTimes[t-startIndex] = (_timeStart + _timeOfSample*t);
	metaData->SetObservationTimes(observationTimes);
	metaData->SetValueDescription("Power");
	
	baseline->SetData(tfData);
	baseline->SetMetaData(metaData);
	
	return baseline;
}

void FilterBankSet::AddWriteFlagsTask(const ImageSetIndex& index, std::vector<Mask2DCPtr>& flags)
{
	if(_bitCount != 32)
		throw std::runtime_error("Only support for 32-bit filterbank sets has been added as of yet");
	
	const size_t intervalIndex = reinterpret_cast<const FilterBankSetIndex&>(index)._intervalIndex;
	
	const size_t
		startIndex = (_sampleCount * intervalIndex) / _intervalCount,
		endIndex = (_sampleCount * (intervalIndex+1)) / _intervalCount;
	
	std::fstream file(_location.c_str());
	file.seekg(_headerEnd + std::streampos(startIndex * sizeof(float) * _channelCount));
	
	std::vector<float> buffer(_channelCount);
	for(size_t x=0; x!=endIndex - startIndex; ++x)
	{
		std::streampos pos = file.tellg();
		file.read(reinterpret_cast<char*>(&buffer[0]), _channelCount*sizeof(float));
		for(size_t y=0; y!=_channelCount; ++y)
		{
			if(flags[0]->Value(x, y))
				buffer[y] = std::numeric_limits<float>::quiet_NaN();
		}
		file.seekp(pos);
		file.write(reinterpret_cast<char*>(&buffer[0]), _channelCount*sizeof(float));
	}
}

void FilterBankSet::Initialize()
{
}

void FilterBankSet::PerformReadRequests()
{
}

void FilterBankSet::PerformWriteDataTask(const ImageSetIndex& index, std::vector<Image2DCPtr> realImages, std::vector<Image2DCPtr> imaginaryImages)
{
	throw std::runtime_error("Can't write data back to filter bank format: not implemented");
}

std::string FilterBankSetIndex::Description() const
{
	std::ostringstream str;
	str << "Filterbank set -- interval " << (_intervalIndex+1) << '/' << static_cast<FilterBankSet&>(imageSet())._intervalCount;
	return str.str();
}

void FilterBankSetIndex::Next()
{
	++_intervalIndex;
	if(_intervalIndex == static_cast<FilterBankSet&>(imageSet())._intervalCount)
	{
		_intervalIndex = 0;
		_isValid = false;
	}
}

void FilterBankSetIndex::Previous()
{
	if(_intervalIndex == 0)
	{
		_intervalIndex = static_cast<FilterBankSet&>(imageSet())._intervalCount-1;
		_isValid = false;
	}
	else
		--_intervalIndex;
}

} //namespace
