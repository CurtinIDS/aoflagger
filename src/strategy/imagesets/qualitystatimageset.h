#ifndef QUALITY_STAT_IMAGE_SET_H
#define QUALITY_STAT_IMAGE_SET_H

#include "singleimageset.h"

#include "../../quality/qualitytablesformatter.h"
#include "../../quality/statisticsderivator.h"

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
			
		TimeFrequencyMetaDataCPtr& metaData = data.second;
		
		return new BaselineData(data.first, metaData);
	}
	
	virtual ImageSet *Copy()
	{
		return new QualityStatImageSet(_filename);
	}
	
	virtual void Initialize()
	{ }
	
private:
	std::string _filename;
	QualityTablesFormatter::StatisticKind _statisticKind;
};

} // namespace

#endif
