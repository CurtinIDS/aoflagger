#ifndef MSIO_QUALITY_DATA_H
#define MSIO_QUALITY_DATA_H

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/TableRecord.h>

#define QUALITY_TABLES_VERSION      1
#define QUALITY_TABLES_VERSION_STR "1"

class QualityTablesFormatter {
	public:
		enum StatisticKind
		{
			CountStatistic,
			SumStatistic,
			MeanStatistic,
			RFICountStatistic,
			RFISumStatistic,
			RFIMeanStatistic,
			RFIRatioStatistic,
			RFIPercentageStatistic,
			FlaggedCountStatistic,
			FlaggedRatioStatistic,
			SumP2Statistic,
			SumP3Statistic,
			SumP4Statistic,
			VarianceStatistic,
			VarianceOfVarianceStatistic,
			StandardDeviationStatistic,
			SkewnessStatistic,
			KurtosisStatistic, 
			SignalToNoiseStatistic,
			DSumStatistic,
			DMeanStatistic,
			DSumP2Statistic,
			DSumP3Statistic,
			DSumP4Statistic,
			DVarianceStatistic,
			DVarianceOfVarianceStatistic,
			DStandardDeviationStatistic,
			DCountStatistic,
			BadSolutionCountStatistic,
			CorrectCountStatistic,
			CorrectedMeanStatistic,
			CorrectedSumP2Statistic,
			CorrectedDCountStatistic,
			CorrectedDMeanStatistic,
			CorrectedDSumP2Statistic,
			FTSumStatistic,
			FTSumP2Statistic,
			EndPlaceHolderStatistic
		};
		
		enum StatisticDimension
		{
			TimeDimension,
			FrequencyDimension,
			BaselineDimension,
			BaselineTimeDimension
		};
		
		enum QualityTable
		{
			KindNameTable,
			TimeStatisticTable,
			FrequencyStatisticTable,
			BaselineStatisticTable,
			BaselineTimeStatisticTable
		};
		
		struct TimePosition
		{
			double time;
			double frequency;
		};
		
		struct FrequencyPosition
		{
			double frequency;
		};
		
		struct BaselinePosition
		{
			unsigned antenna1;
			unsigned antenna2;
			double frequency;
		};
		
		struct BaselineTimePosition
		{
			double time;
			unsigned antenna1;
			unsigned antenna2;
			double frequency;
		};
		
		QualityTablesFormatter(const std::string &measurementSetName) :
			_measurementSet(0),
			_measurementSetName(measurementSetName),
			_kindNameTable(0),
			_timeTable(0),
			_frequencyTable(0),
			_baselineTable(0),
			_baselineTimeTable(0)
		{
		}
		
		~QualityTablesFormatter()
		{
			Close();
		}
		
		void Close()
		{
			if(_kindNameTable != 0)
				delete _kindNameTable;
			_kindNameTable = 0;
			if(_timeTable != 0)
				delete _timeTable;
			_timeTable = 0;
			if(_frequencyTable != 0)
				delete _frequencyTable;
			_frequencyTable = 0;
			if(_baselineTable != 0)
				delete _baselineTable;
			_baselineTable = 0;
			if(_baselineTimeTable != 0)
				delete _baselineTimeTable;
			_baselineTimeTable = 0;
			
			closeMainTable();
		}
		
		bool TableExists(enum QualityTable table) const
		{
			return _measurementSet->isReadable(TableToFilename(table));
		}
		
		static const std::string &KindToName(const enum StatisticKind kind)
		{
			return _kindToNameTable[(int) kind];
		}
		
		static enum StatisticKind NameToKind(const std::string &kindName);
		
		static const std::string &TableToName(const enum QualityTable table)
		{
			return _tableToNameTable[(int) table];
		}
		
		const std::string TableToFilename(const enum QualityTable table) const
		{
			return _measurementSetName + '/' + TableToName(table);
		}
		
		enum QualityTable DimensionToTable(const enum StatisticDimension dimension) const
		{
			return _dimensionToTableTable[(int) dimension];
		}
		
		bool IsStatisticAvailable(enum StatisticDimension dimension, enum StatisticKind kind)
		{
			QualityTable table = DimensionToTable(dimension);
			if(!TableExists(KindNameTable) || !TableExists(table))
				return false;
			unsigned kindIndex;
			if(!QueryKindIndex(kind, kindIndex))
				return false;
			return hasOneEntry(table, kindIndex);
		}
		
		void InitializeEmptyStatistic(enum StatisticDimension dimension, enum StatisticKind kind, unsigned polarizationCount)
		{
			if(!TableExists(KindNameTable))
				createKindNameTable();
			
			QualityTable table = DimensionToTable(dimension);
			if(!TableExists(table))
				InitializeEmptyTable(table, polarizationCount);
			else
			{
				removeStatisticFromStatTable(table, kind);
			}
		}
		
		void InitializeEmptyTable(enum QualityTable table, unsigned polarizationCount)
		{
			if(TableExists(table))
				removeEntries(table);
			else
				createTable(table, polarizationCount);
		}
		
		void RemoveTable(enum QualityTable table)
		{
			if(TableExists(table))
			{
				Close();
				openMainTable(true);
				if(_measurementSet->keywordSet().isDefined(TableToName(table)))
					_measurementSet->rwKeywordSet().removeField(TableToName(table));
				if(_measurementSet->isReadable(TableToFilename(table)))
					casacore::Table::deleteTable(TableToFilename(table));
			}
		}
		
		void RemoveAllQualityTables()
		{
			RemoveTable(BaselineTimeStatisticTable);
			RemoveTable(BaselineStatisticTable);
			RemoveTable(FrequencyStatisticTable);
			RemoveTable(TimeStatisticTable);
			RemoveTable(KindNameTable);
		}
		
		unsigned StoreKindName(enum StatisticKind kind)
		{
			return StoreKindName(KindToName(kind));
		}

		unsigned StoreKindName(const std::string &name);
		
		void StoreTimeValue(double time, double frequency, const class StatisticalValue &value);
		void StoreFrequencyValue(double frequency, const class StatisticalValue &value);
		void StoreBaselineValue(unsigned antenna1, unsigned antenna2, double frequency, const class StatisticalValue &value);
		void StoreBaselineTimeValue(unsigned antenna1, unsigned antenna2, double time, double frequency, const class StatisticalValue &value);
		
		unsigned QueryKindIndex(enum StatisticKind kind);
		bool QueryKindIndex(enum StatisticKind kind, unsigned &destKindIndex);
		unsigned StoreOrQueryKindIndex(enum StatisticKind kind)
		{
			unsigned kindIndex;
			if(QueryKindIndex(kind, kindIndex))
				return kindIndex;
			else
				return StoreKindName(kind);
		}
		
		unsigned QueryStatisticEntryCount(enum StatisticDimension dimension, unsigned kindIndex);
		
		void QueryTimeStatistic(unsigned kindIndex, std::vector<std::pair<TimePosition, class StatisticalValue> > &entries);
		void QueryFrequencyStatistic(unsigned kindIndex, std::vector<std::pair<FrequencyPosition, class StatisticalValue> > &entries);
		void QueryBaselineStatistic(unsigned kindIndex, std::vector<std::pair<BaselinePosition, class StatisticalValue> > &entries);
		void QueryBaselineTimeStatistic(unsigned kindIndex, std::vector<std::pair<BaselineTimePosition, class StatisticalValue> > &entries);
		
		unsigned GetPolarizationCount();
	private:
		QualityTablesFormatter(const QualityTablesFormatter &) { } // don't allow copies
		void operator=(const QualityTablesFormatter &) { } // don't allow assignment
		
		const static std::string _kindToNameTable[];
		const static std::string _tableToNameTable[];
		const static enum QualityTable _dimensionToTableTable[];
		
		const static std::string ColumnNameAntenna1;
		const static std::string ColumnNameAntenna2;
		const static std::string ColumnNameFrequency;
		const static std::string ColumnNameKind;
		const static std::string ColumnNameName;
		const static std::string ColumnNameTime;
		const static std::string ColumnNameValue;
		
		casacore::Table *_measurementSet;
		const std::string _measurementSetName;
		
		casacore::Table *_kindNameTable;
		casacore::Table *_timeTable;
		casacore::Table *_frequencyTable;
		casacore::Table *_baselineTable;
		casacore::Table *_baselineTimeTable;
		
		bool hasOneEntry(enum QualityTable table, unsigned kindIndex);
		void removeStatisticFromStatTable(enum QualityTable table, enum StatisticKind kind);
		void removeKindNameEntry(enum StatisticKind kind);
		void removeEntries(enum QualityTable table);
		
		/**
			* Add the time column to the table descriptor. Used by create..Table() methods.
			* It holds "Measure"s of time, which is what casacore defines as a value including
			* a unit and a reference frame.
			*/
		void addTimeColumn(casacore::TableDesc &tableDesc);
		
		/**
		 * Add the frequency column to the table descriptor. Used by create..Table() methods.
		 * It holds "Quantum"s of frequency, which is what casacore defines as a value including
		 * a unit (Hertz).
		 */
		void addFrequencyColumn(casacore::TableDesc &tableDesc);
		
		/**
		 * Add value column to the table descriptor. Used by create..Table() methods.
		 * It consist of an array of statistics, each element holds a polarization.
		 */
		void addValueColumn(casacore::TableDesc &tableDesc, unsigned polarizationCount);
		
		void createTable(enum QualityTable table, unsigned polarizationCount)
		{
			switch(table)
			{
				case KindNameTable:              createKindNameTable(); break;
				case TimeStatisticTable:         createTimeStatisticTable(polarizationCount); break;
				case FrequencyStatisticTable:    createFrequencyStatisticTable(polarizationCount); break;
				case BaselineStatisticTable:     createBaselineStatisticTable(polarizationCount); break;
				case BaselineTimeStatisticTable: createBaselineTimeStatisticTable(polarizationCount); break;
				default: break;
			}
		}
		
		/**
		 * Will add an empty table to the measurement set named "QUALITY_KIND_NAME" and
		 * initialize its default column.
		 * This table can hold a list of quality statistic types that are referred to in
		 * the statistic value tables.
		 */
		void createKindNameTable();
		/**
		 * Will add an empty table to the measurement set named "QUALITY_TIME_STATISTIC" and
		 * initialize its default column.
		 * This table can hold several statistic kinds per time step. 
		 * @param polarizationCount specifies the nr polarizations. This is required for the
		 * shape of the value column.
		 */
		void createTimeStatisticTable(unsigned polarizationCount);
		/**
		 * Will add an empty table to the measurement set named "QUALITY_FREQUENCY_STATISTIC" and
		 * initialize its default column.
		 * This table can hold several statistic kinds per time step. 
		 * @param polarizationCount specifies the nr polarizations. This is required for the
		 * shape of the value column.
		 */
		void createFrequencyStatisticTable(unsigned polarizationCount);
		/**
		 * Will add an empty table to the measurement set named "QUALITY_BASELINE_STATISTIC" and
		 * initialize its default column.
		 * This table can hold several statistic kinds per time step. 
		 * @param polarizationCount specifies the nr polarizations. This is required for the
		 * shape of the value column.
		 */
		void createBaselineStatisticTable(unsigned polarizationCount);
		void createBaselineTimeStatisticTable(unsigned polarizationCount);
		unsigned findFreeKindIndex(casacore::Table &kindTable);
		
		void openMainTable(bool needWrite);
		void closeMainTable()
		{
			if(_measurementSet != 0)
				delete _measurementSet;
			_measurementSet = 0;
		}
		
		void openTable(QualityTable table, bool needWrite, casacore::Table **tablePtr);
		void openKindNameTable(bool needWrite)
		{
			openTable(KindNameTable, needWrite, &_kindNameTable);
		}
		void openTimeTable(bool needWrite)
		{
			openTable(TimeStatisticTable, needWrite, &_timeTable);
		}
		void openFrequencyTable(bool needWrite)
		{
			openTable(FrequencyStatisticTable, needWrite, &_frequencyTable);
		}
		void openBaselineTable(bool needWrite)
		{
			openTable(BaselineStatisticTable, needWrite, &_baselineTable);
		}
		void openBaselineTimeTable(bool needWrite)
		{
			openTable(BaselineTimeStatisticTable, needWrite, &_baselineTimeTable);
		}
		casacore::Table &getTable(QualityTable table, bool needWrite)
		{
			casacore::Table **tablePtr = 0;
			switch(table)
			{
				case KindNameTable: tablePtr = &_kindNameTable; break;
				case TimeStatisticTable: tablePtr = &_timeTable; break;
				case FrequencyStatisticTable: tablePtr = &_frequencyTable; break;
				case BaselineStatisticTable: tablePtr = &_baselineTable; break;
				case BaselineTimeStatisticTable: tablePtr = &_baselineTimeTable; break;
			}
			openTable(table, needWrite, tablePtr);
			return **tablePtr;
		}
};

#endif
