#ifndef INDIRECTBASELINEREADER_H
#define INDIRECTBASELINEREADER_H

#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <stdexcept>

#include "baselinereader.h"
#include "directbaselinereader.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class IndirectBaselineReader : public BaselineReader {
	public:
		explicit IndirectBaselineReader(const std::string &msFile);
		~IndirectBaselineReader();

		virtual void PerformReadRequests();
		virtual void PerformFlagWriteRequests();
		virtual void PerformDataWriteTask(std::vector<Image2DCPtr> _realImages, std::vector<Image2DCPtr> _imaginaryImages, int antenna1, int antenna2, int spectralWindow, unsigned sequenceId);
		
		virtual size_t GetMinRecommendedBufferSize(size_t /*threadCount*/) { return 1; }
		virtual size_t GetMaxRecommendedBufferSize(size_t /*threadCount*/) { return 2; }
		void SetReadUVW(bool readUVW) { _readUVW = readUVW; }
	private:
		class ReorderInfo
		{
		public:
			std::auto_ptr<std::ofstream> dataFile;
			std::auto_ptr<std::ofstream> flagFile;
		};
		class UpdateInfo
		{
		public:
			std::auto_ptr<std::ifstream> dataFile;
			std::auto_ptr<std::ifstream> flagFile;
		};
		class SeqIndexLookupTable
		{
		public:
			SeqIndexLookupTable(size_t antennaCount, size_t spectralWindowCount, size_t sequenceCount) :
				_antennaCount(antennaCount), _table(sequenceCount)
			{
				size_t maxBaselineCount = antennaCount * antennaCount;
				for(size_t i=0; i!=sequenceCount; ++i)
				{
					std::vector<std::vector<size_t> > &spwTable = _table[i];
					spwTable.resize(spectralWindowCount);
					for(size_t j=0; j!=spectralWindowCount; ++j)
					{
						std::vector<size_t> &baselTable = spwTable[j];
						baselTable.resize(maxBaselineCount);
					}
				}
			}
			size_t &Value(size_t antenna1, size_t antenna2, size_t spectralWindow, size_t sequenceId)
			{
				return _table[sequenceId][spectralWindow][antenna1 * _antennaCount + antenna2];
			}
		private:
			size_t _antennaCount;
			std::vector<std::vector<std::vector<size_t> > > _table;
		};
		void reorderedMS();
		void reorderFull();
		void makeLookupTables(size_t &fileSize);
		void updateOriginalMSData();
		void updateOriginalMSFlags();
		void performFlagWriteTask(std::vector<Mask2DCPtr> flags, unsigned antenna1, unsigned antenna2, unsigned spw, unsigned sequenceId);
		
		template<bool UpdateData, bool UpdateFlags>
		void updateOriginalMS();
		
		void removeTemporaryFiles();
		
		static void preAllocate(const char *filename, size_t fileSize);
		static const char* DataFilename()
		{
			return "aoflagger-data.tmp";
		}
		static const char* FlagFilename()
		{
			return "aoflagger-flags.tmp";
		}
		static const char* MetaFilename()
		{
			return "ao-msinfo.tmp";
		}

		DirectBaselineReader _directReader;
		SeqIndexLookupTable *_seqIndexTable;
		std::vector<size_t> _filePositions;
		bool _msIsReordered;
		bool _removeReorderedFiles;
		bool _reorderedDataFilesHaveChanged;
		bool _reorderedFlagFilesHaveChanged;
		bool _readUVW;
};

#endif // INDIRECTBASELINEREADER_H
