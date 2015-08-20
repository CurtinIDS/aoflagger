/***************************************************************************
 *   Copyright (C) 2008 by A.R. Offringa   *
 *   offringa@astro.rug.nl   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "qualitytablesformatter.h"

#include <stdexcept>
#include <set>

#include <casacore/ms/MeasurementSets/MSColumns.h>

#include <casacore/tables/Tables/ArrColDesc.h>
#include <casacore/tables/Tables/ScaColDesc.h>
#include <casacore/tables/Tables/SetupNewTab.h>

#include <casacore/measures/TableMeasures/TableMeasDesc.h>

#include <casacore/measures/Measures/MEpoch.h>

#include "statisticalvalue.h"

const std::string QualityTablesFormatter::_kindToNameTable[] =
{
	"Count", "Sum", "Mean", "RFICount", "RFISum", "RFIMean", "RFIRatio", "RFIPercentage", "FlaggedCount", "FlaggedRatio", "SumP2", "SumP3", "SumP4", "Variance", "VarianceOfVariance", "StandardDeviation", "Skewness", "Kurtosis", "SignalToNoise", "DSum", "DMean", "DSumP2", "DSumP3", "DSumP4", "DVariance", "DVarianceOfVariance", "DStandardDeviation", "DCount", "BadSolutionCount", "CorrectCount", "CorrectedMean", "CorrectedSumP2", "CorrectedDCount", "CorrectedDMean", "CorrectedDSumP2", "FTSum", "FTSumP2"
};

const std::string QualityTablesFormatter::_tableToNameTable[] =
{
	"QUALITY_KIND_NAME",
	"QUALITY_TIME_STATISTIC",
	"QUALITY_FREQUENCY_STATISTIC",
	"QUALITY_BASELINE_STATISTIC",
	"QUALITY_BASELINE_TIME_STATISTIC"
};

const enum QualityTablesFormatter::QualityTable QualityTablesFormatter::_dimensionToTableTable[] =
{
	TimeStatisticTable,
	FrequencyStatisticTable,
	BaselineStatisticTable,
	BaselineTimeStatisticTable
};

const std::string QualityTablesFormatter::ColumnNameAntenna1  = "ANTENNA1";
const std::string QualityTablesFormatter::ColumnNameAntenna2  = "ANTENNA2";
const std::string QualityTablesFormatter::ColumnNameFrequency = "FREQUENCY";
const std::string QualityTablesFormatter::ColumnNameKind      = "KIND";
const std::string QualityTablesFormatter::ColumnNameName      = "NAME";
const std::string QualityTablesFormatter::ColumnNameTime      = "TIME";
const std::string QualityTablesFormatter::ColumnNameValue     = "VALUE";

enum QualityTablesFormatter::StatisticKind QualityTablesFormatter::NameToKind(const std::string &kindName)
{
	for(unsigned i=0;i<37;++i)
	{
		if(kindName == _kindToNameTable[i])
			return (QualityTablesFormatter::StatisticKind) i;
	}
	throw std::runtime_error("Statistics kind not known");
}

unsigned QualityTablesFormatter::QueryKindIndex(enum StatisticKind kind)
{
	unsigned kindIndex;
	if(!QueryKindIndex(kind, kindIndex))
		throw std::runtime_error("getKindIndex(): Requested statistic kind not available.");
	return kindIndex;
}

bool QualityTablesFormatter::QueryKindIndex(enum StatisticKind kind, unsigned &destKindIndex)
{
	openKindNameTable(false);
	casacore::ROScalarColumn<int> kindColumn(*_kindNameTable, ColumnNameKind);
	casacore::ROScalarColumn<casacore::String> nameColumn(*_kindNameTable, ColumnNameName);
	const casacore::String nameToFind(KindToName(kind));
	
	const unsigned nrRow = _kindNameTable->nrow();
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(nameColumn(i) == nameToFind)
		{
			destKindIndex = kindColumn(i);
			return true;
		}
	}
	return false;
}

bool QualityTablesFormatter::hasOneEntry(enum QualityTable table, unsigned kindIndex)
{
	casacore::Table &casaTable = getTable(table, false);
	casacore::ROScalarColumn<int> kindColumn(casaTable, ColumnNameKind);
	
	const unsigned nrRow = casaTable.nrow();
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(kindColumn(i) == (int) kindIndex)
			return true;
	}
	return false;
}

/**
 * Will add an empty table to the measurement set named "QUALITY_KIND_NAME" and
 * initialize its default column.
 * This table can hold a list of quality statistic types that are referred to in
 * the statistic value tables.
 */
void QualityTablesFormatter::createKindNameTable()
{
	casacore::TableDesc tableDesc("QUALITY_KIND_NAME_TYPE", QUALITY_TABLES_VERSION_STR, casacore::TableDesc::Scratch);
	tableDesc.comment() = "Couples the KIND column in the other quality tables to the name of a statistic (e.g. Mean)";
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameKind, "Index of the statistic kind"));
	tableDesc.addColumn(casacore::ScalarColumnDesc<casacore::String>(ColumnNameName, "Name of the statistic"));

	casacore::SetupNewTable newTableSetup(TableToFilename(KindNameTable), tableDesc, casacore::Table::New);
	casacore::Table newTable(newTableSetup);
	openMainTable(true);
	_measurementSet->rwKeywordSet().defineTable(TableToName(KindNameTable), newTable);
}

/**
 * Add the time column to the table descriptor. Used by create..Table() methods.
 * It holds "Measure"s of time, which is what casacore defines as a value including
 * a unit and a reference frame.
 */
void QualityTablesFormatter::addTimeColumn(casacore::TableDesc &tableDesc)
{
	casacore::ScalarColumnDesc<double> timeDesc(ColumnNameTime, "Central time of statistic");
	tableDesc.addColumn(timeDesc);
	
	casacore::TableMeasRefDesc measRef(casacore::MEpoch::UTC);
	casacore::TableMeasValueDesc measVal(tableDesc, ColumnNameTime);
	casacore::TableMeasDesc<casacore::MEpoch> mepochCol(measVal, measRef);
	mepochCol.write(tableDesc);
}

/**
 * Add the frequency column to the table descriptor. Used by create..Table() methods.
 * It holds "Quantum"s of frequency, which is what casacore defines as a value including
 * a unit (Hertz).
 */
void QualityTablesFormatter::addFrequencyColumn(casacore::TableDesc &tableDesc)
{
	casacore::ScalarColumnDesc<double> freqDesc(ColumnNameFrequency, "Central frequency of statistic bin");
	tableDesc.addColumn(freqDesc);
	
	casacore::Unit hertzUnit("Hz");
	
	casacore::TableQuantumDesc quantDesc(tableDesc, ColumnNameFrequency, hertzUnit);
	quantDesc.write(tableDesc);
}

/**
 * Add value column to the table descriptor. Used by create..Table() methods.
 * It consist of an array of statistics, each element holds a polarization.
 */
void QualityTablesFormatter::addValueColumn(casacore::TableDesc &tableDesc, unsigned polarizationCount)
{
	casacore::IPosition shape(1);
	shape[0] = polarizationCount;
	casacore::ArrayColumnDesc<casacore::Complex> valDesc(ColumnNameValue, "Value of statistic", shape, casacore::ColumnDesc::Direct);
	tableDesc.addColumn(valDesc);
}

/**
 * Will add an empty table to the measurement set named "QUALITY_TIME_STATISTIC" and
 * initialize its default column.
 * This table can hold several statistic kinds per time step. 
 * @param polarizationCount specifies the nr polarizations. This is required for the
 * shape of the value column.
 */
void QualityTablesFormatter::createTimeStatisticTable(unsigned polarizationCount)
{
	casacore::TableDesc tableDesc("QUALITY_TIME_STATISTIC_TYPE", QUALITY_TABLES_VERSION_STR, casacore::TableDesc::Scratch);
	tableDesc.comment() = "Statistics over time";
	
	addTimeColumn(tableDesc);
	addFrequencyColumn(tableDesc);
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameKind, "Index of the statistic kind"));
	addValueColumn(tableDesc, polarizationCount);

	casacore::SetupNewTable newTableSetup(TableToFilename(TimeStatisticTable), tableDesc, casacore::Table::New);
	casacore::Table newTable(newTableSetup);
	openMainTable(true);
	_measurementSet->rwKeywordSet().defineTable(TableToName(TimeStatisticTable), newTable);
}

/**
 * Will add an empty table to the measurement set named "QUALITY_FREQUENCY_STATISTIC" and
 * initialize its default column.
 * This table can hold several statistic kinds per time step. 
 * @param polarizationCount specifies the nr polarizations. This is required for the
 * shape of the value column.
 */
void QualityTablesFormatter::createFrequencyStatisticTable(unsigned polarizationCount)
{
	casacore::TableDesc tableDesc("QUALITY_FREQUENCY_STATISTIC_TYPE", QUALITY_TABLES_VERSION_STR, casacore::TableDesc::Scratch);
	tableDesc.comment() = "Statistics over frequency";
	
	addFrequencyColumn(tableDesc);
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameKind, "Index of the statistic kind"));
	addValueColumn(tableDesc, polarizationCount);

	casacore::SetupNewTable newTableSetup(TableToFilename(FrequencyStatisticTable), tableDesc, casacore::Table::New);
	casacore::Table newTable(newTableSetup);
	openMainTable(true);
	_measurementSet->rwKeywordSet().defineTable(TableToName(FrequencyStatisticTable), newTable);
}

/**
 * Will add an empty table to the measurement set named "QUALITY_BASELINE_STATISTIC" and
 * initialize its default column.
 * This table can hold several statistic kinds per time step. 
 * @param polarizationCount specifies the nr polarizations. This is required for the
 * shape of the value column.
 */
void QualityTablesFormatter::createBaselineStatisticTable(unsigned polarizationCount)
{
	casacore::TableDesc tableDesc("QUALITY_BASELINE_STATISTIC_TYPE", QUALITY_TABLES_VERSION_STR, casacore::TableDesc::Scratch);
	tableDesc.comment() = "Statistics per baseline";
	
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameAntenna1, "Index of first antenna"));
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameAntenna2, "Index of second antenna"));
	addFrequencyColumn(tableDesc);
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameKind, "Index of the statistic kind"));
	addValueColumn(tableDesc, polarizationCount);

	casacore::SetupNewTable newTableSetup(TableToFilename(BaselineStatisticTable), tableDesc, casacore::Table::New);
	casacore::Table newTable(newTableSetup);
	openMainTable(true);
	_measurementSet->rwKeywordSet().defineTable(TableToName(BaselineStatisticTable), newTable);
}

void QualityTablesFormatter::createBaselineTimeStatisticTable(unsigned polarizationCount)
{
	casacore::TableDesc tableDesc("QUALITY_BASELINE_TIME_STATISTIC_TYPE", QUALITY_TABLES_VERSION_STR, casacore::TableDesc::Scratch);
	tableDesc.comment() = "Statistics per baseline";
	
	addTimeColumn(tableDesc);
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameAntenna1, "Index of first antenna"));
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameAntenna2, "Index of second antenna"));
	addFrequencyColumn(tableDesc);
	tableDesc.addColumn(casacore::ScalarColumnDesc<int>(ColumnNameKind, "Index of the statistic kind"));
	addValueColumn(tableDesc, polarizationCount);

	casacore::SetupNewTable newTableSetup(TableToFilename(BaselineTimeStatisticTable), tableDesc, casacore::Table::New);
	casacore::Table newTable(newTableSetup);
	openMainTable(true);
	_measurementSet->rwKeywordSet().defineTable(TableToName(BaselineTimeStatisticTable), newTable);
}

unsigned QualityTablesFormatter::StoreKindName(const std::string &name)
{
	// This should be done atomically, but two quality writers in the same table would be
	// a weird thing to do anyway, plus I don't know how the casa tables can be made atomic
	// (and still have good performance).
	
	openKindNameTable(true);
	
	unsigned kindIndex = findFreeKindIndex(*_kindNameTable);
	
	unsigned newRow = _kindNameTable->nrow();
	_kindNameTable->addRow();
	casacore::ScalarColumn<int> kindColumn(*_kindNameTable, ColumnNameKind);
	casacore::ScalarColumn<casacore::String> nameColumn(*_kindNameTable, ColumnNameName);
	kindColumn.put(newRow, kindIndex);
	nameColumn.put(newRow, name);
	return kindIndex;
}

unsigned QualityTablesFormatter::findFreeKindIndex(casacore::Table &kindTable)
{
	int maxIndex = 0;
	
	casacore::ROScalarColumn<int> kindColumn(kindTable, ColumnNameKind);
	
	const unsigned nrRow = kindTable.nrow();
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(kindColumn(i) > maxIndex)
			maxIndex = kindColumn(i);
	}
	return maxIndex + 1;
}

void QualityTablesFormatter::openTable(QualityTable table, bool needWrite, casacore::Table **tablePtr)
{
	if(*tablePtr == 0)
	{
		openMainTable(false);
		*tablePtr = new casacore::Table(_measurementSet->keywordSet().asTable(TableToName(table)));
		if(needWrite)
			(*tablePtr)->reopenRW();
	} else {
		if(needWrite && !(*tablePtr)->isWritable())
			(*tablePtr)->reopenRW();
	}
}

void QualityTablesFormatter::StoreTimeValue(double time, double frequency, const StatisticalValue &value)
{
	openTimeTable(true);
	
	unsigned newRow = _timeTable->nrow();
	_timeTable->addRow();
	
	// TODO maybe the columns need to be cached to avoid having to look them up for each store...
	casacore::ScalarColumn<double> timeColumn(*_timeTable, ColumnNameTime);
	casacore::ScalarColumn<double> frequencyColumn(*_timeTable, ColumnNameFrequency);
	casacore::ScalarColumn<int> kindColumn(*_timeTable, ColumnNameKind);
	casacore::ArrayColumn<casacore::Complex> valueColumn(*_timeTable, ColumnNameValue);
	
	timeColumn.put(newRow, time);
	frequencyColumn.put(newRow, frequency);
	kindColumn.put(newRow, value.KindIndex());
	casacore::Vector<casacore::Complex> data(value.PolarizationCount());
	for(unsigned i=0;i<value.PolarizationCount();++i)
		data[i] = value.Value(i);
	valueColumn.put(newRow, data);
}

void QualityTablesFormatter::StoreFrequencyValue(double frequency, const StatisticalValue &value)
{
	openFrequencyTable(true);
	
	unsigned newRow = _frequencyTable->nrow();
	_frequencyTable->addRow();
	
	casacore::ScalarColumn<double> frequencyColumn(*_frequencyTable, ColumnNameFrequency);
	casacore::ScalarColumn<int> kindColumn(*_frequencyTable, ColumnNameKind);
	casacore::ArrayColumn<casacore::Complex> valueColumn(*_frequencyTable, ColumnNameValue);
	
	frequencyColumn.put(newRow, frequency);
	kindColumn.put(newRow, value.KindIndex());
	casacore::Vector<casacore::Complex> data(value.PolarizationCount());
	for(unsigned i=0;i<value.PolarizationCount();++i)
		data[i] = value.Value(i);
	valueColumn.put(newRow, data);
}

void QualityTablesFormatter::StoreBaselineValue(unsigned antenna1, unsigned antenna2, double frequency, const StatisticalValue &value)
{
	openBaselineTable(true);
	
	unsigned newRow = _baselineTable->nrow();
	_baselineTable->addRow();
	
	casacore::ScalarColumn<int> antenna1Column(*_baselineTable, ColumnNameAntenna1);
	casacore::ScalarColumn<int> antenna2Column(*_baselineTable, ColumnNameAntenna2);
	casacore::ScalarColumn<double> frequencyColumn(*_baselineTable, ColumnNameFrequency);
	casacore::ScalarColumn<int> kindColumn(*_baselineTable, ColumnNameKind);
	casacore::ArrayColumn<casacore::Complex> valueColumn(*_baselineTable, ColumnNameValue);
	
	antenna1Column.put(newRow, antenna1);
	antenna2Column.put(newRow, antenna2);
	frequencyColumn.put(newRow, frequency);
	kindColumn.put(newRow, value.KindIndex());
	casacore::Vector<casacore::Complex> data(value.PolarizationCount());
	for(unsigned i=0;i<value.PolarizationCount();++i)
		data[i] = value.Value(i);
	valueColumn.put(newRow, data);
}

void QualityTablesFormatter::StoreBaselineTimeValue(unsigned antenna1, unsigned antenna2, double time, double frequency, const StatisticalValue &value)
{
	openBaselineTimeTable(true);
	
	unsigned newRow = _baselineTimeTable->nrow();
	_baselineTimeTable->addRow();
	
	casacore::ScalarColumn<double> timeColumn(*_baselineTimeTable, ColumnNameTime);
	casacore::ScalarColumn<int> antenna1Column(*_baselineTimeTable, ColumnNameAntenna1);
	casacore::ScalarColumn<int> antenna2Column(*_baselineTimeTable, ColumnNameAntenna2);
	casacore::ScalarColumn<double> frequencyColumn(*_baselineTimeTable, ColumnNameFrequency);
	casacore::ScalarColumn<int> kindColumn(*_baselineTimeTable, ColumnNameKind);
	casacore::ArrayColumn<casacore::Complex> valueColumn(*_baselineTimeTable, ColumnNameValue);
	
	timeColumn.put(newRow, time);
	antenna1Column.put(newRow, antenna1);
	antenna2Column.put(newRow, antenna2);
	frequencyColumn.put(newRow, frequency);
	kindColumn.put(newRow, value.KindIndex());
	casacore::Vector<casacore::Complex> data(value.PolarizationCount());
	for(unsigned i=0;i<value.PolarizationCount();++i)
		data[i] = value.Value(i);
	valueColumn.put(newRow, data);
}

void QualityTablesFormatter::removeStatisticFromStatTable(enum QualityTable qualityTable, enum StatisticKind kind)
{
	unsigned kindIndex;
	if(QueryKindIndex(kind, kindIndex))
	{
		casacore::Table &table = getTable(qualityTable, true);
		casacore::ScalarColumn<int> kindColumn(table, ColumnNameKind);

		unsigned nrRow = table.nrow();

		for(unsigned i=0;i<nrRow;++i)
		{
			while(i<nrRow && kindColumn(i) == (int) kindIndex)
			{
				table.removeRow(i);
				--nrRow;
			}
		}
	}
}

void QualityTablesFormatter::removeKindNameEntry(enum StatisticKind kind)
{
	openKindNameTable(true);
	casacore::ScalarColumn<casacore::String> nameColumn(*_kindNameTable, ColumnNameName);
	
	const unsigned nrRow = _kindNameTable->nrow();
	const casacore::String kindName(KindToName(kind));
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(nameColumn(i) == kindName)
		{
			_kindNameTable->removeRow(i);
			break;
		}
	}
}

void QualityTablesFormatter::removeEntries(enum QualityTable table)
{
	casacore::Table &casaTable = getTable(table, true);
	const unsigned nrRow = casaTable.nrow();
	for(int i=nrRow-1;i>=0;--i)
	{
		casaTable.removeRow(i);
	}
}

unsigned QualityTablesFormatter::QueryStatisticEntryCount(enum StatisticDimension dimension, unsigned kindIndex)
{
	casacore::Table &casaTable(getTable(DimensionToTable(dimension), false));
	casacore::ROScalarColumn<int> kindColumn(casaTable, ColumnNameKind);
	
	const unsigned nrRow = casaTable.nrow();
	size_t count = 0;
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(kindColumn(i) == (int) kindIndex)
			++count;
	}
	return count;
}

unsigned QualityTablesFormatter::GetPolarizationCount()
{
	casacore::Table &table(getTable(TimeStatisticTable, false));
	casacore::ROArrayColumn<casacore::Complex> valueColumn(table, ColumnNameValue);
	return valueColumn.columnDesc().shape()[0];
}

void QualityTablesFormatter::QueryTimeStatistic(unsigned kindIndex, std::vector<std::pair<TimePosition, StatisticalValue> > &entries)
{
	casacore::Table &table(getTable(TimeStatisticTable, false));
	const unsigned nrRow = table.nrow();
	
	casacore::ROScalarColumn<double> timeColumn(table, ColumnNameTime);
	casacore::ROScalarColumn<double> frequencyColumn(table, ColumnNameFrequency);
	casacore::ROScalarColumn<int> kindColumn(table, ColumnNameKind);
	casacore::ROArrayColumn<casacore::Complex> valueColumn(table, ColumnNameValue);
	
	int polarizationCount = valueColumn.columnDesc().shape()[0];
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(kindColumn(i) == (int) kindIndex)
		{
			StatisticalValue value(polarizationCount);
			value.SetKindIndex(kindIndex);
			casacore::Array<casacore::Complex> valueArray = valueColumn(i);
			casacore::Array<casacore::Complex>::iterator iter = valueArray.begin();
			for(int p=0;p<polarizationCount;++p)
			{
				value.SetValue(p, *iter);
				++iter;
			}
			TimePosition position;
			position.time = timeColumn(i);
			position.frequency = frequencyColumn(i);
			entries.push_back(std::pair<TimePosition, StatisticalValue>(position, value));
		}
	}
}

void QualityTablesFormatter::QueryFrequencyStatistic(unsigned kindIndex, std::vector<std::pair<FrequencyPosition, StatisticalValue> > &entries)
{
	casacore::Table &table(getTable(FrequencyStatisticTable, false));
	const unsigned nrRow = table.nrow();
	
	casacore::ROScalarColumn<double> frequencyColumn(table, ColumnNameFrequency);
	casacore::ROScalarColumn<int> kindColumn(table, ColumnNameKind);
	casacore::ROArrayColumn<casacore::Complex> valueColumn(table, ColumnNameValue);
	
	int polarizationCount = valueColumn.columnDesc().shape()[0];
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(kindColumn(i) == (int) kindIndex)
		{
			StatisticalValue value(polarizationCount);
			value.SetKindIndex(kindIndex);
			casacore::Array<casacore::Complex> valueArray = valueColumn(i);
			casacore::Array<casacore::Complex>::iterator iter = valueArray.begin();
			for(int p=0;p<polarizationCount;++p)
			{
				value.SetValue(p, *iter);
				++iter;
			}
			FrequencyPosition position;
			position.frequency = frequencyColumn(i);
			entries.push_back(std::pair<FrequencyPosition, StatisticalValue>(position, value));
		}
	}
}

void QualityTablesFormatter::QueryBaselineStatistic(unsigned kindIndex, std::vector<std::pair<BaselinePosition, StatisticalValue> > &entries)
{
	casacore::Table &table(getTable(BaselineStatisticTable, false));
	const unsigned nrRow = table.nrow();
	
	casacore::ROScalarColumn<int> antenna1Column(table, ColumnNameAntenna1);
	casacore::ROScalarColumn<int> antenna2Column(table, ColumnNameAntenna2);
	casacore::ROScalarColumn<double> frequencyColumn(table, ColumnNameFrequency);
	casacore::ROScalarColumn<int> kindColumn(table, ColumnNameKind);
	casacore::ROArrayColumn<casacore::Complex> valueColumn(table, ColumnNameValue);
	
	int polarizationCount = valueColumn.columnDesc().shape()[0];
	
	for(unsigned i=0;i<nrRow;++i)
	{
		if(kindColumn(i) == (int) kindIndex)
		{
			StatisticalValue value(polarizationCount);
			value.SetKindIndex(kindIndex);
			casacore::Array<casacore::Complex> valueArray = valueColumn(i);
			casacore::Array<casacore::Complex>::iterator iter = valueArray.begin();
			for(int p=0;p<polarizationCount;++p)
			{
				value.SetValue(p, *iter);
				++iter;
			}
			BaselinePosition position;
			position.antenna1 = antenna1Column(i);
			position.antenna2 = antenna2Column(i);
			position.frequency = frequencyColumn(i);
			entries.push_back(std::pair<BaselinePosition, StatisticalValue>(position, value));
		}
	}
}

void QualityTablesFormatter::openMainTable(bool needWrite)
{
	if(_measurementSet == 0)
	{
		if(needWrite)
			_measurementSet = new casacore::Table(_measurementSetName, casacore::Table::Update);
		else
			_measurementSet = new casacore::Table(_measurementSetName);
	}
	else if(needWrite)
	{
		if(!_measurementSet->isWritable())
			_measurementSet->reopenRW();
	}
}
