#include "histogramcollection.h"

#include "histogramtablesformatter.h"

void HistogramCollection::Save(HistogramTablesFormatter &histogramTables)
{
	histogramTables.InitializeEmptyTables();
	for(size_t p=0;p<_polarizationCount;++p)
	{
		LogHistogram totalHistogram;
		GetTotalHistogramForCrossCorrelations(p, totalHistogram);
		const unsigned totalIndex = histogramTables.StoreOrQueryTypeIndex(HistogramTablesFormatter::TotalHistogram, p);
		for(LogHistogram::iterator i=totalHistogram.begin();i!=totalHistogram.end();++i)
		{
			histogramTables.StoreValue(totalIndex, i.binStart(), i.binEnd(), i.unnormalizedCount());
		}
		
		LogHistogram rfiHistogram;
		GetRFIHistogramForCrossCorrelations(p, rfiHistogram);
		const unsigned rfiIndex = histogramTables.StoreOrQueryTypeIndex(HistogramTablesFormatter::RFIHistogram, p);
		for(LogHistogram::iterator i=rfiHistogram.begin();i!=rfiHistogram.end();++i)
		{
			histogramTables.StoreValue(rfiIndex, i.binStart(), i.binEnd(), i.unnormalizedCount());
		}
	}
}

void HistogramCollection::Load(HistogramTablesFormatter &histogramTables)
{
	Clear();
	for(unsigned p=0;p<_polarizationCount;++p)
	{
		const unsigned totalHistogramIndex = histogramTables.QueryTypeIndex(HistogramTablesFormatter::TotalHistogram, p);
		std::vector<HistogramTablesFormatter::HistogramItem> totalHistogram;
		histogramTables.QueryHistogram(totalHistogramIndex, totalHistogram);
		GetTotalHistogram(0, 1, p).SetData(totalHistogram);

		const unsigned rfiHistogramIndex = histogramTables.QueryTypeIndex(HistogramTablesFormatter::RFIHistogram, p);
		std::vector<HistogramTablesFormatter::HistogramItem> rfiHistogram;
		histogramTables.QueryHistogram(rfiHistogramIndex, rfiHistogram);
		GetRFIHistogram(0, 1, p).SetData(rfiHistogram);
	}
}

void HistogramCollection::Add(const unsigned antenna1, const unsigned antenna2, const unsigned polarization, Image2DCPtr image, Mask2DCPtr flagMask)
{
	LogHistogram &totalHistogram = GetTotalHistogram(antenna1, antenna2, polarization);
	LogHistogram &rfiHistogram = GetRFIHistogram(antenna1, antenna2, polarization);
	
	for(size_t y=0;y<image->Height();++y)
	{
		for(size_t x=0;x<image->Width();++x)
		{
			const double amplitude = image->Value(x, y);
			totalHistogram.Add(amplitude);
			if(flagMask->Value(x, y))
				rfiHistogram.Add(amplitude);
		}
	}
}

void HistogramCollection::Add(const unsigned antenna1, const unsigned antenna2, const unsigned polarization, Image2DCPtr image, Mask2DCPtr flagMask, Mask2DCPtr correlatorMask)
{
	LogHistogram &totalHistogram = GetTotalHistogram(antenna1, antenna2, polarization);
	LogHistogram &rfiHistogram = GetRFIHistogram(antenna1, antenna2, polarization);
	
	for(size_t y=0;y<image->Height();++y)
	{
		for(size_t x=0;x<image->Width();++x)
		{
			if(!correlatorMask->Value(x, y))
			{
				const double amplitude = image->Value(x, y);
				totalHistogram.Add(amplitude);
				if(flagMask->Value(x, y))
					rfiHistogram.Add(amplitude);
			}
		}
	}
}

void HistogramCollection::Add(const unsigned antenna1, const unsigned antenna2, const unsigned polarization, Image2DCPtr real, Image2DCPtr imaginary, Mask2DCPtr flagMask, Mask2DCPtr correlatorMask)
{
	LogHistogram &totalHistogram = GetTotalHistogram(antenna1, antenna2, polarization);
	LogHistogram &rfiHistogram = GetRFIHistogram(antenna1, antenna2, polarization);
	
	for(size_t y=0;y<real->Height();++y)
	{
		for(size_t x=0;x<real->Width();++x)
		{
			if(!correlatorMask->Value(x, y))
			{
				const double r = real->Value(x, y), i = imaginary->Value(x, y);
				const double amplitude = sqrt(r*r + i*i);
				totalHistogram.Add(amplitude);
				if(flagMask->Value(x, y))
					rfiHistogram.Add(amplitude);
			}
		}
	}
}

