#include "statisticscollection.h"

template<bool IsDiff>
void StatisticsCollection::addTimeAndBaseline(unsigned antenna1, unsigned antenna2, double time, double centralFrequency, int polarization, const float *reals, const float *imags, const bool *isRFI, const bool* origFlags, unsigned nsamples, unsigned step, unsigned stepRFI, unsigned stepFlags)
{
	unsigned long rfiCount = 0;
	unsigned long count = 0;
	long double sum_R = 0.0, sum_I = 0.0;
	long double sumP2_R = 0.0, sumP2_I = 0.0;
	for(unsigned j=0;j<nsamples;++j)
	{
		if (!*origFlags) {
			if(std::isfinite(*reals) && std::isfinite(*imags))
			{
				if(*isRFI)
				{
					++rfiCount;
				} else {
					const long double rVal = *reals;
					const long double iVal = *imags;
					++count;
					sum_R += rVal;
					sum_I += iVal;
					sumP2_R += rVal*rVal;
					sumP2_I += iVal*iVal;
				}
			}
		}
		reals += step;
		imags += step;
		isRFI += stepRFI;
		origFlags += stepFlags;
	}
	
	if(antenna1 != antenna2)
	{
		DefaultStatistics &timeStat = getTimeStatistic(time, centralFrequency);
		addToStatistic<IsDiff>(timeStat, polarization, count, sum_R, sum_I, sumP2_R, sumP2_I, rfiCount);
	}
	DefaultStatistics &baselineStat = getBaselineStatistic(antenna1, antenna2, centralFrequency);
	addToStatistic<IsDiff>(baselineStat, polarization, count, sum_R, sum_I, sumP2_R, sumP2_I, rfiCount);
}

template<bool IsDiff>
void StatisticsCollection::addFrequency(unsigned band, int polarization, const float *reals, const float *imags, const bool *isRFI, const bool *origFlags, unsigned nsamples, unsigned step, unsigned stepRFI, unsigned stepFlags, bool shiftOneUp)
{
	std::vector<DefaultStatistics *> &bandStats = _bands.find(band)->second;
	const unsigned fAdd = shiftOneUp ? 1 : 0;
	for(unsigned j=0;j<nsamples;++j)
	{
		if (!*origFlags)
		{
			if(std::isfinite(*reals) && std::isfinite(*imags))
			{
				DefaultStatistics &freqStat = *bandStats[j + fAdd];
				if(*isRFI)
				{
					addToStatistic<IsDiff>(freqStat, polarization, 0, 0.0, 0.0, 0.0, 0.0, 1);
				} else {
					const long double r = *reals, i = *imags;
					addToStatistic<IsDiff>(freqStat, polarization, 1, r, i, r*r, i*i, 0);
				}
			}
		}
		isRFI += stepRFI;
		origFlags += stepFlags;
		reals += step;
		imags += step;
	}
}

void StatisticsCollection::Add(unsigned antenna1, unsigned antenna2, double time, unsigned band, int polarization, const float *reals, const float *imags, const bool *isRFI, const bool* origFlags, unsigned nsamples, unsigned step, unsigned stepRFI, unsigned stepFlags)
{
	if(nsamples == 0) return;
	
	const double centralFrequency = _centralFrequencies.find(band)->second;
	
	addTimeAndBaseline<false>(antenna1, antenna2, time, centralFrequency, polarization, reals, imags, isRFI, origFlags, nsamples, step, stepRFI, stepFlags);
	if(antenna1 != antenna2)
		addFrequency<false>(band, polarization, reals, imags, isRFI, origFlags, nsamples, step, stepRFI, stepFlags, false);
	
	// Allocate vector with length nsamples, so there is
	// a diff element, even if nsamples=1.
	std::vector<float> diffReals(nsamples);
	std::vector<float> diffImags(nsamples);
	bool *diffRFIFlags  = new bool[nsamples];
	bool *diffOrigFlags = new bool[nsamples];
	for (unsigned i=0;i<nsamples-1;++i)
	{
		diffReals[i] = (reals[(i+1)*step] - reals[i*step]) * M_SQRT1_2;
		diffImags[i] = (imags[(i+1)*step] - imags[i*step]) * M_SQRT1_2;
		diffRFIFlags[i] = isRFI[i*stepRFI] | isRFI[(i+1)*stepRFI];
		diffOrigFlags[i] = origFlags[i*stepFlags] | origFlags[(i+1)*stepFlags];
	}
	addTimeAndBaseline<true>(antenna1, antenna2, time, centralFrequency, polarization, &(diffReals[0]), &(diffImags[0]), diffRFIFlags, diffOrigFlags, nsamples-1, 1, 1, 1);
	if(antenna1 != antenna2)
	{
		addFrequency<true>(band, polarization, &(diffReals[0]), &(diffImags[0]), diffRFIFlags, diffOrigFlags, nsamples-1, 1, 1, 1, false);
		addFrequency<true>(band, polarization, &(diffReals[0]), &(diffImags[0]), diffRFIFlags, diffOrigFlags, nsamples-1, 1, 1, 1, true);
	}
	delete[] diffRFIFlags;
	delete[] diffOrigFlags;
}

void StatisticsCollection::AddToTimeFrequency(unsigned antenna1, unsigned antenna2, double time, unsigned band, int polarization, const float* reals, const float* imags, const bool* isRFI, const bool* origFlags, unsigned nsamples, unsigned step, unsigned stepRFI, unsigned stepFlags)
{
	if(nsamples == 0) return;
	
	if(antenna1 == antenna2) return;
	
	addToTimeFrequency<false>(time, &_bandFrequencies[band][0], polarization, reals, imags, isRFI, origFlags, nsamples, step, stepRFI, stepFlags, false);
	
	// Allocate vector with length nsamples, so there is
	// a diff element, even if nsamples=1.
	std::vector<float> diffReals(nsamples);
	std::vector<float> diffImags(nsamples);
	bool *diffRFIFlags  = new bool[nsamples];
	bool *diffOrigFlags = new bool[nsamples];
	for (unsigned i=0;i<nsamples-1;++i)
	{
		diffReals[i] = (reals[(i+1)*step] - reals[i*step]) * M_SQRT1_2;
		diffImags[i] = (imags[(i+1)*step] - imags[i*step]) * M_SQRT1_2;
		diffRFIFlags[i] = isRFI[i*stepRFI] | isRFI[(i+1)*stepRFI];
		diffOrigFlags[i] = origFlags[i*stepFlags] | origFlags[(i+1)*stepFlags];
	}
	addToTimeFrequency<true>(time, &_bandFrequencies[band][0], polarization, &(diffReals[0]), &(diffImags[0]), diffRFIFlags, diffOrigFlags, nsamples-1, 1, 1, 1, false);
	addToTimeFrequency<true>(time, &_bandFrequencies[band][0], polarization, &(diffReals[0]), &(diffImags[0]), diffRFIFlags, diffOrigFlags, nsamples-1, 1, 1, 1, true);
	delete[] diffRFIFlags;
	delete[] diffOrigFlags;
}

void StatisticsCollection::AddImage(unsigned antenna1, unsigned antenna2, const double *times, unsigned band, int polarization, const Image2DCPtr &realImage, const Image2DCPtr &imagImage, const Mask2DCPtr &rfiMask, const Mask2DCPtr &correlatorMask)
{
	if(realImage->Width() == 0 || realImage->Height() == 0) return;

	const double centralFrequency = _centralFrequencies.find(band)->second;
	DefaultStatistics &baselineStat = getBaselineStatistic(antenna1, antenna2, centralFrequency);
	std::vector<DefaultStatistics *> &bandStats = _bands.find(band)->second;
	std::vector<DefaultStatistics *> timeStats(realImage->Width());
	
	for(size_t t=0; t!=realImage->Width(); ++t)
		timeStats[t] = &getTimeStatistic(times[t], centralFrequency);
	
	for(size_t f=0; f<realImage->Height(); ++f)
	{
		DefaultStatistics &freqStat = *bandStats[f];
		const bool
			*origFlags = correlatorMask->ValuePtr(0, f),
			*nextOrigFlags = origFlags + correlatorMask->Stride(),
			*isRFI = rfiMask->ValuePtr(0, f),
			*isNextRFI = isRFI + rfiMask->Stride();
		const float
			*reals = realImage->ValuePtr(0, f),
			*imags = imagImage->ValuePtr(0, f),
			*nextReal = reals + realImage->Stride(),
			*nextImag = imags + imagImage->Stride();
		for(size_t t=0; t<realImage->Width(); ++t) 
		{
			if (!*origFlags && std::isfinite(*reals) && std::isfinite(*imags))
			{
				long double real = *reals, imag = *imags;
				
				if(*isRFI)
				{
					if(antenna1 != antenna2)
					{
						++timeStats[t]->rfiCount[polarization];
						++freqStat.rfiCount[polarization];
					}
					++baselineStat.rfiCount[polarization];
				} else {
					long double
						realSq = real*real,
						imagSq = imag*imag;
					
					if(antenna1 != antenna2)
					{
						addSingleNonRFISampleToStatistic<false>(*timeStats[t], polarization, real, imag, realSq, imagSq);
						addSingleNonRFISampleToStatistic<false>(freqStat, polarization, real, imag, realSq, imagSq);
					}
					addSingleNonRFISampleToStatistic<false>(baselineStat, polarization, real, imag, realSq, imagSq);
				}
				
				if(f != realImage->Height()-1)
				{
					DefaultStatistics &nextFreqStat = *bandStats[f+1];
					if(!*nextOrigFlags && std::isfinite(*nextReal) && std::isfinite(*nextImag))
					{
						real = (*nextReal - *reals) * M_SQRT1_2;
						imag = (*nextImag - *imags) * M_SQRT1_2;
						
						if(!(*isRFI || *isNextRFI))
						{
							long double
								realSq = real*real,
								imagSq = imag*imag;
							
							if(antenna1 != antenna2)
							{
								addSingleNonRFISampleToStatistic<true>(*timeStats[t], polarization, real, imag, realSq, imagSq);
								addSingleNonRFISampleToStatistic<true>(freqStat, polarization, real, imag, realSq, imagSq);
								addSingleNonRFISampleToStatistic<true>(nextFreqStat, polarization, real, imag, realSq, imagSq);
							}

							addSingleNonRFISampleToStatistic<true>(baselineStat, polarization, real, imag, realSq, imagSq);
						}
					}
				}
			}
			
			++origFlags;
			++isRFI;
			++reals;
			++imags;
			
			++nextOrigFlags;
			++isNextRFI;
			++nextReal;
			++nextImag;
		}
	}
}

void StatisticsCollection::lowerResolution(StatisticsCollection::DoubleStatMap& map, size_t maxSteps) const
{
	if(map.size() > maxSteps)
	{
		DoubleStatMap newMap;
		double gridStep, gridStart;
		if(maxSteps > 1)
		{
			double oldGridStep =  (map.rbegin()->first - map.begin()->first) / (map.size() - 1);
			gridStep = (map.rbegin()->first - map.begin()->first + oldGridStep) / maxSteps;
			gridStart = map.begin()->first - 0.5*oldGridStep;
		}
		else {
			gridStep = map.rbegin()->first - map.begin()->first;
			gridStart = map.begin()->first;
		}
		size_t gridIndex = 0;
		for(DoubleStatMap::iterator i=map.begin();i!=map.end();)
		{
			DefaultStatistics integratedStat(_polarizationCount);
			double cellMid = (gridIndex+0.5)*gridStep + gridStart, cellEnd =(gridIndex+1)*gridStep + gridStart;
			size_t count = 0;
			while(i!=map.end() && i->first<cellEnd)
			{
				++count;
				integratedStat += i->second;
				++i;
			}
			++gridIndex;
			// If the last items are not yet gridded, they might be just over the border due to rounding errors; put them in the last bucket:
			if(gridIndex == maxSteps)
			{
				while(i != map.end())
				{
					++count;
					integratedStat += i->second;
					++i;
				}
			}
			if(count > 0)
				newMap.insert(std::pair<double, DefaultStatistics>(cellMid, integratedStat));
		}
		map = newMap;
	}
}

template<bool IsDiff>
void StatisticsCollection::addToTimeFrequency(double time, const double* frequencies, int polarization, const float* reals, const float* imags, const bool* isRFI, const bool* origFlags, unsigned nsamples, unsigned step, unsigned stepRFI, unsigned stepFlags, bool shiftOneUp)
{
	const unsigned fAdd = shiftOneUp ? 1 : 0;
	for(unsigned j=0;j<nsamples;++j)
	{
		if (!*origFlags)
		{
			if(std::isfinite(*reals) && std::isfinite(*imags))
			{
				DefaultStatistics &timeStat = getTimeStatistic(time, frequencies[j+fAdd]);
				if(*isRFI)
				{
					addToStatistic<IsDiff>(timeStat, polarization, 0, 0.0, 0.0, 0.0, 0.0, 1);
				} else {
					const long double r = *reals, i = *imags;
					addToStatistic<IsDiff>(timeStat, polarization, 1, r, i, r*r, i*i, 0);
				}
			}
		}
		isRFI += stepRFI;
		origFlags += stepFlags;
		reals += step;
		imags += step;
	}
}

void StatisticsCollection::saveTime(QualityTablesFormatter &qd) const
{
	initializeEmptyStatistics(qd, QualityTablesFormatter::TimeDimension);
	
	Indices indices;
	indices.fill(qd);
		
	StatisticSaver saver;
	saver.dimension = QualityTablesFormatter::TimeDimension;
	saver.qualityData = &qd;
	
	for(std::map<double, DoubleStatMap>::const_iterator j=_timeStatistics.begin();j!=_timeStatistics.end();++j)
	{
		saver.frequency = j->first;
		const DoubleStatMap &map = j->second;
		
		for(DoubleStatMap::const_iterator i=map.begin();i!=map.end();++i)
		{
			saver.time = i->first;
			const DefaultStatistics &stat = i->second;
			
			saveEachStatistic(saver, stat, indices);
		}
	}
}

void StatisticsCollection::saveFrequency(QualityTablesFormatter &qd) const
{
	if(!_frequencyStatistics.empty())
	{
		initializeEmptyStatistics(qd, QualityTablesFormatter::FrequencyDimension);
		
		Indices indices;
		indices.fill(qd);
			
		StatisticSaver saver;
		saver.dimension = QualityTablesFormatter::FrequencyDimension;
		saver.qualityData = &qd;
		
		for(DoubleStatMap::const_iterator i=_frequencyStatistics.begin();i!=_frequencyStatistics.end();++i)
		{
			saver.frequency = i->first;
			const DefaultStatistics &stat = i->second;
			
			saveEachStatistic(saver, stat, indices);
		}
	}
}

void StatisticsCollection::saveBaseline(QualityTablesFormatter &qd) const
{
	if(!_baselineStatistics.empty())
	{
		initializeEmptyStatistics(qd, QualityTablesFormatter::BaselineDimension);
		
		Indices indices;
		indices.fill(qd);
		
		StatisticSaver saver;
		saver.dimension = QualityTablesFormatter::BaselineDimension;
		saver.frequency = centralFrequency();
		saver.qualityData = &qd;
		
		for(std::map<double, BaselineStatisticsMap>::const_iterator j=_baselineStatistics.begin();j!=_baselineStatistics.end();++j)
		{
			saver.frequency = j->first;
			const BaselineStatisticsMap &map = j->second;
			
			const std::vector<std::pair<unsigned, unsigned> > baselines = map.BaselineList();
		
			for(std::vector<std::pair<unsigned, unsigned> >::const_iterator i=baselines.begin();i!=baselines.end();++i)
			{
				saver.antenna1 = i->first;
				saver.antenna2 = i->second;
				
				const DefaultStatistics &stat = map.GetStatistics(saver.antenna1, saver.antenna2);
				
				saveEachStatistic(saver, stat, indices);
			}
		}
	}
}

