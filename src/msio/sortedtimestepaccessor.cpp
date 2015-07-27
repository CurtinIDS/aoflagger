#include "sortedtimestepaccessor.h"

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

void SortedTimestepAccessor::Open()
{
	assertNotOpen();

	_totalChannelCount = 0;
	_highestFrequency = 0.0;
	_lowestFrequency = 0.0;

	for(SetInfoVector::iterator i=_sets.begin(); i!=_sets.end(); ++i)
	{
		SetInfo &set = *i;

		casacore::MeasurementSet ms(set.path);
		casacore::Table mainTable(set.path, casacore::Table::Update);

		// Create the sorted table and iterator
		casacore::Block<casacore::String> names(4);
		names[0] = "TIME";
		names[1] = "ANTENNA1";
		names[2] = "ANTENNA2";
		names[3] = "DATA_DESC_ID";
		casacore::Table sortab = mainTable.sort(names);
		names.resize(3, true, true);
		set.tableIter = new casacore::TableIterator(sortab, names, casacore::TableIterator::Ascending, casacore::TableIterator::NoSort);

		// Check number of polarizations
		casacore::Table polTable = ms.polarization();
		casacore::ROArrayColumn<int> corTypeColumn(polTable, "CORR_TYPE");
		if(_polarizationCount==0 && i==_sets.begin())
			_polarizationCount = corTypeColumn(0).shape()[0];
		else if(_polarizationCount != corTypeColumn(0).shape()[0])
			throw TimestepAccessorException("Number of polarizations don't match!");

		// Find lowest and highest frequency and check order
		set.lowestFrequency = 0.0;
		set.highestFrequency = 0.0;
		casacore::Table spectralWindowTable = ms.spectralWindow();
		casacore::ROArrayColumn<double> frequencyCol(spectralWindowTable, "CHAN_FREQ");
		for(unsigned b=0;b<spectralWindowTable.nrow();++b)
		{
			casacore::Array<double> frequencyArray = frequencyCol(b);
			casacore::Array<double>::const_iterator frequencyIterator = frequencyArray.begin();
			while(frequencyIterator != frequencyArray.end())
			{
				double frequency = *frequencyIterator;
				if(set.lowestFrequency == 0.0) set.lowestFrequency = frequency;
				if(frequency < set.lowestFrequency || frequency <= set.highestFrequency)
					throw TimestepAccessorException("Channels or bands are not ordered in increasing frequency");
				set.highestFrequency = frequency;
				++frequencyIterator;
			}
		}
		if(set.lowestFrequency < _highestFrequency)
			throw TimestepAccessorException("Sub-bands are not given in order of increasing frequency");
		if(_lowestFrequency == 0.0) _lowestFrequency = set.lowestFrequency;
		_highestFrequency = set.highestFrequency;

		set.bandCount = spectralWindowTable.nrow();
		set.channelsPerBand = casacore::ROArrayColumn<casacore::Complex>(mainTable, "DATA")(0).shape()[1];
		_totalChannelCount += set.bandCount * set.channelsPerBand;
	}
	_isOpen = true;
}

void SortedTimestepAccessor::Close()
{
	assertOpen();

	for(SetInfoVector::iterator i=_sets.begin(); i!=_sets.end(); ++i)
	{
		delete i->tableIter;
	}
	_isOpen = false;
}

bool SortedTimestepAccessor::ReadNext(SortedTimestepAccessor::TimestepIndex &index, SortedTimestepAccessor::TimestepData &data)
{
	assertOpen();

	index.FreeTables();

	double timeStep = 0.0;
	unsigned valIndex = 0;
	casacore::Table **tablePtr = index.tables;

	for(SetInfoVector::iterator i=_sets.begin(); i!=_sets.end(); ++i)
	{
		SetInfo &set = *i;

		if(set.tableIter->pastEnd())
			return false;
		*tablePtr = new casacore::Table(set.tableIter->table());
		casacore::Table &table(**tablePtr);

		// Check timestep & read u,v coordinates & antenna's
		casacore::ROScalarColumn<double> timeColumn = casacore::ROScalarColumn<double>(table, "TIME");
		if(timeStep == 0.0) {
			casacore::ROArrayColumn<double> uvwColumn = casacore::ROArrayColumn<double>(table, "UVW");
			timeStep = timeColumn(0);
			casacore::Array<double> uvwArray = uvwColumn(0);
			casacore::Array<double>::const_iterator uvwIterator = uvwArray.begin();
			data.u = *uvwIterator;
			++uvwIterator;
			data.v = *uvwIterator;
			casacore::ROScalarColumn<int>
				antenna1Column = casacore::ROScalarColumn<int>(table, "ANTENNA1"),
				antenna2Column = casacore::ROScalarColumn<int>(table, "ANTENNA2");
			data.antenna1 = antenna1Column(0);
			data.antenna2 = antenna2Column(0);
		}
		else if(timeStep != timeColumn(0))
			throw TimestepAccessorException("Sets do not have same time steps");

		// Copy data from tables in arrays
		casacore::ROArrayColumn<casacore::Complex> dataColumn(table, "DATA");
		for(unsigned band=0;band<set.bandCount;++band)
		{
			casacore::Array<casacore::Complex> dataArray = dataColumn(band);
			casacore::Array<casacore::Complex>::const_iterator dataIterator = dataArray.begin();
			for(unsigned f=0;f<set.channelsPerBand;++f)
			{
				for(unsigned p=0;p<_polarizationCount;++p)
				{
					data.realData[p][valIndex] = (*dataIterator).real();
					data.imagData[p][valIndex] = (*dataIterator).imag();
					++dataIterator;
				}
				++valIndex;
			}
		}

		set.tableIter->next();
		++tablePtr;
	}
	return true;
}

void SortedTimestepAccessor::Write(SortedTimestepAccessor::TimestepIndex &index, const SortedTimestepAccessor::TimestepData &data)
{
	assertOpen();

	casacore::Table **tablePtr = index.tables;
	unsigned valIndex = 0;

	for(SetInfoVector::iterator i=_sets.begin(); i!=_sets.end(); ++i)
	{
		const SetInfo &set = *i;

		casacore::Table &table = **tablePtr;

		// Copy data from arrays in tables
		casacore::ArrayColumn<casacore::Complex> dataColumn(table, "DATA");
		for(unsigned band=0;band<set.bandCount;++band)
		{
			casacore::Array<casacore::Complex> dataArray = dataColumn(band);
			casacore::Array<casacore::Complex>::iterator dataIterator = dataArray.begin();
			for(unsigned f=0;f<set.channelsPerBand;++f)
			{
				for(unsigned p=0;p<_polarizationCount;++p)
				{
					(*dataIterator).real() = data.realData[p][valIndex];
					(*dataIterator).imag() = data.imagData[p][valIndex];
					++dataIterator;
				}
				++valIndex;
			}
			dataColumn.basePut(band, dataArray);
		}
		set.tableIter->next();
	}
	index.FreeTables();
}
