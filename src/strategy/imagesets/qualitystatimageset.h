#ifndef QUALITY_STAT_IMAGE_SET_H
#define QUALITY_STAT_IMAGE_SET_H

#include "singleimageset.h"

#include "../../quality/qualitytablesformatter.h"
#include "../../quality/statisticsderivator.h"
#include "../../util/aologger.h"

#include <ms/MeasurementSets/MeasurementSet.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/ScalarColumn.h>

#include <boost/filesystem/path.hpp>

namespace rfiStrategy
{

class QualityStatImageSet : public SingleImageSet
{
public:
	QualityStatImageSet(const std::string& filename) :
		_filename(filename),
		_statisticKind(QualityTablesFormatter::StandardDeviationStatistic)
	{
		if(!_filename.empty() && (*_filename.rbegin()) == '/')
			_filename.resize(_filename.size()-1);
		boost::filesystem::path p(_filename);
		if(p.filename() == "QUALITY_TIME_STATISTIC")
			_filename = p.parent_path().string();
	}
	
	virtual std::string Name()
	{ return File(); }
	virtual std::string File()
	{ return _filename; }
	
	virtual BaselineData *Read()
	{
		QualityTablesFormatter formatter(_filename);
		StatisticsCollection statCollection;
		statCollection.Load(formatter);

		StatisticsDerivator derivator(statCollection);
		
		std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> data =
			derivator.CreateTFData(_statisticKind);
			
		TimeFrequencyData& tfData = data.first;
		TimeFrequencyMetaDataCPtr& metaData = data.second;
		Mask2DPtr mask = Mask2D::CreateUnsetMaskPtr(tfData.ImageWidth(), tfData.ImageHeight());
		for(size_t y=0; y!=tfData.ImageHeight(); ++y)
		{
			for(size_t x=0; x!=tfData.ImageWidth(); ++x)
			{
				for(size_t i=0; i!=tfData.ImageCount(); ++i)
					mask->SetValue(x, y, !std::isfinite(tfData.GetImage(i)->Value(x, y)));
			}
		}
		tfData.SetGlobalMask(mask);

		BaselineData* baselineData = new BaselineData(tfData, metaData);
		return baselineData;
	}
	
	virtual ImageSet *Copy()
	{
		return new QualityStatImageSet(_filename);
	}
	
	virtual void Initialize()
	{ }
	
	virtual void Write(const std::vector<Mask2DCPtr>& flags)
	{
		std::vector<Mask2DCPtr> flagsCopy(flags);
		casa::MeasurementSet ms(_filename, casa::Table::Update);
		if(ms.nrow() > 0)
		{
			casa::ArrayColumn<bool> flagColumn(ms, casa::MeasurementSet::columnName(casa::MSMainEnums::FLAG));
			casa::ROScalarColumn<double> timeColumn(ms, casa::MeasurementSet::columnName(casa::MSMainEnums::TIME));
			casa::Array<bool> flagArray(flagColumn.get(0));
			const size_t
				polCount = flagArray.shape()[0],
				channelCount = flagArray.shape()[1];
			AOLogger::Debug << "Saving flags to measurement set (" << channelCount << " ch x " << polCount << " pol)...\n";
			if(flagsCopy.size() == 1 && polCount>1)
			{
				do { flagsCopy.push_back(flagsCopy[0]);
				} while(flagsCopy.size() != polCount);
			}
			if(flagsCopy.size() != polCount)
				throw std::runtime_error("Polarization counts don't match");
			if(flagsCopy[0]->Height() != channelCount)
			{
				std::ostringstream s;
				s << "Channel counts don't match (" << flagsCopy[0]->Height() << " in mask, " << channelCount << " in data)";
				throw std::runtime_error(s.str());
			}
			size_t timeIndex = size_t(-1);
			double time = -1.0;
			bool* timestepFlags = new bool[channelCount*polCount];
			for(size_t row=0; row!=ms.nrow(); ++row)
			{
				if(time != timeColumn(row))
				{
					time = timeColumn(row);
					timeIndex++;
					
					bool* iter = timestepFlags;
					for(size_t ch=0; ch!=channelCount; ++ch)
					{
						for(size_t p=0; p!=polCount; ++p)
						{
							*iter = flagsCopy[p]->Value(timeIndex, ch);
							++iter;
						}
					}
				}
				flagColumn.get(row, flagArray);
				casa::Array<bool>::contiter iter = flagArray.cbegin();
				for(size_t i=0; i!=channelCount*polCount; ++i)
				{
					*iter = *iter || timestepFlags[i];
					++iter;
				}
				flagColumn.put(row, flagArray);
			}
			delete[] timestepFlags;
		}
	}
	
private:
	std::string _filename;
	QualityTablesFormatter::StatisticKind _statisticKind;
};

} // namespace

#endif
