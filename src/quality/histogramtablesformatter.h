#ifndef HISTOGRAM_TABLES_FORMATTER_H
#define HISTOGRAM_TABLES_FORMATTER_H

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/TableRecord.h>

#include <vector>

class HistogramTablesFormatter {
	public:
		enum TableKind { HistogramCountTable, HistogramTypeTable };
		
		struct HistogramItem
		{
			double binStart;
			double binEnd;
			double count;
		};
		
		enum HistogramType
		{
			TotalHistogram,
			RFIHistogram
		};
	
		HistogramTablesFormatter(const std::string &measurementSetName) :
			_measurementSet(0),
			_measurementSetName(measurementSetName),
			_typeTable(0),
			_countTable(0)
		{
		}
		
		~HistogramTablesFormatter()
		{
			Close();
		}
		
		void Close()
		{
			if(_countTable != 0)
			{
				delete _countTable;
				_countTable = 0;
			}
			if(_typeTable != 0)
			{
				delete _typeTable;
				_typeTable = 0;
			}
			closeMainTable();
		}
		
		std::string CountTableName() const
		{
			return "QUALITY_HISTOGRAM_COUNT";
		}
		
		std::string TypeTableName() const
		{
			return "QUALITY_HISTOGRAM_TYPE";
		}
		
		std::string TableName(enum TableKind table) const
		{
			switch(table)
			{
				case HistogramCountTable:
					return CountTableName();
				case HistogramTypeTable:
					return TypeTableName();
				default:
					return "";
			}
		}
		
		std::string TypeToName(HistogramType type) const
		{
			switch(type)
			{
				case TotalHistogram: return "Total";
				case RFIHistogram: return "RFI";
				default: return std::string();
			}
		}
		
		std::string TableFilename(enum TableKind table) const
		{
			return _measurementSetName + '/' + TableName(table);
		}
		
		bool TableExists(enum TableKind table) const
		{
			return _measurementSet->isReadable(TableFilename(table));
		}
		
		bool IsAvailable(unsigned typeIndex)
		{
			if(!TableExists(HistogramCountTable) || !TableExists(HistogramTypeTable))
				return false;
			return hasOneEntry(typeIndex);
		}
		
		void InitializeEmptyTables()
		{
			if(TableExists(HistogramCountTable))
				removeEntries(HistogramCountTable);
			else
				createCountTable();
			
			if(TableExists(HistogramTypeTable))
				removeEntries(HistogramTypeTable);
			else
				createTypeTable();
		}
		
		void RemoveTable(enum TableKind table)
		{
			if(TableExists(table))
			{
				Close();
				openMainTable(true);
				if(_measurementSet->keywordSet().isDefined(TableName(table)))
					_measurementSet->rwKeywordSet().removeField(TableName(table));
				if(_measurementSet->isReadable(TableFilename(table)))
					casacore::Table::deleteTable(TableFilename(table));
			}
		}
		
		void StoreValue(unsigned typeIndex, double binStart, double binEnd, double count);
		
		void QueryHistogram(unsigned typeIndex, std::vector<HistogramItem> &histogram);
		
		unsigned QueryTypeIndex(enum HistogramType type, unsigned polarizationIndex);
		bool QueryTypeIndex(enum HistogramType type, unsigned polarizationIndex, unsigned &destTypeIndex);
		unsigned StoreOrQueryTypeIndex(enum HistogramType type, unsigned polarizationIndex)
		{
			unsigned typeIndex;
			if(QueryTypeIndex(type, polarizationIndex, typeIndex))
				return typeIndex;
			else
				return StoreType(type, polarizationIndex);
		}
		unsigned StoreType(enum HistogramType type, unsigned polarizationIndex);
		bool HistogramsExist()
		{
			return TableExists(HistogramCountTable) && TableExists(HistogramTypeTable);
		}
		void RemoveAll()
		{
			RemoveTable(HistogramCountTable);
			RemoveTable(HistogramTypeTable);
		}
	private:
		HistogramTablesFormatter(const HistogramTablesFormatter &) { } // don't allow copies
		void operator=(const HistogramTablesFormatter &) { } // don't allow assignment
		
		const static std::string ColumnNameType;
		const static std::string ColumnNameName;
		const static std::string ColumnNamePolarization;
		
		const static std::string ColumnNameBinStart;
		const static std::string ColumnNameBinEnd;
		const static std::string ColumnNameCount;
		
		casacore::Table *_measurementSet;
		const std::string _measurementSetName;
		
		casacore::Table *_typeTable;
		casacore::Table *_countTable;
		
		bool hasOneEntry(unsigned typeIndex);
		void removeTypeEntry(enum HistogramType type, unsigned polarizationIndex);
		void removeEntries(enum TableKind table);
		
		void addTimeColumn(casacore::TableDesc &tableDesc);
		void addFrequencyColumn(casacore::TableDesc &tableDesc);
		void addValueColumn(casacore::TableDesc &tableDesc);
		
		void createTable(enum TableKind table)
		{
			switch(table)
			{
				case HistogramTypeTable:  createTypeTable(); break;
				case HistogramCountTable: createCountTable(); break;
				default: break;
			}
		}
		
		void createTypeTable();
		void createCountTable();
		unsigned findFreeTypeIndex(casacore::Table &typeTable);
		
		void openMainTable(bool needWrite);
		void closeMainTable()
		{
			if(_measurementSet != 0)
			{
				delete _measurementSet;
				_measurementSet = 0;
			}
		}
		
		void openTable(TableKind table, bool needWrite, casacore::Table **tablePtr);
		void openTypeTable(bool needWrite)
		{
			openTable(HistogramTypeTable, needWrite, &_typeTable);
		}
		void openCountTable(bool needWrite)
		{
			openTable(HistogramCountTable, needWrite, &_countTable);
		}
		casacore::Table &getTable(TableKind table, bool needWrite)
		{
			casacore::Table **tablePtr = 0;
			switch(table)
			{
				case HistogramTypeTable: tablePtr = &_typeTable; break;
				case HistogramCountTable: tablePtr = &_countTable; break;
			}
			openTable(table, needWrite, tablePtr);
			return **tablePtr;
		}
};

#endif
