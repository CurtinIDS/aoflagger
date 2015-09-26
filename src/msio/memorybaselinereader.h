#ifndef MEMORY_BASELINE_READER_H
#define MEMORY_BASELINE_READER_H

#include <map>
#include <vector>
#include <stdexcept>

#include "../structures/antennainfo.h"
#include "baselinereader.h"
#include "../structures/image2d.h"
#include "../structures/mask2d.h"

class MemoryBaselineReader : public BaselineReader {
	public:
		explicit MemoryBaselineReader(const std::string &msFile)
			: BaselineReader(msFile), _isRead(false), _areFlagsChanged(false)
		{
		}
		
		~MemoryBaselineReader()
		{
			if(_areFlagsChanged) writeFlags();
			clear();
		}

		virtual void PerformReadRequests();
		
		virtual void PerformFlagWriteRequests();
		
		virtual void PerformDataWriteTask(std::vector<Image2DCPtr> /*_realImages*/, std::vector<Image2DCPtr> /*_imaginaryImages*/, int /*antenna1*/, int /*antenna2*/, int /*spectralWindow*/, unsigned /*sequenceId*/)
		{
			throw std::runtime_error("The full mem reader can not write data back to file: use the indirect reader");
		}
		
		static bool IsEnoughMemoryAvailable(const std::string &msFile);
		
		virtual size_t GetMinRecommendedBufferSize(size_t /*threadCount*/) { return 1; }
		virtual size_t GetMaxRecommendedBufferSize(size_t /*threadCount*/) { return 2; }
	private:
		void readSet();
		void writeFlags();
		void clear();
		
		bool _isRead, _areFlagsChanged;
		
		class BaselineID
		{
		public:
			unsigned antenna1, antenna2, spw, sequenceId;
			
			bool operator<(const BaselineID &other) const
			{
				if(antenna1<other.antenna1) return true;
				else if(antenna1==other.antenna1)
				{
					if(antenna2<other.antenna2) return true;
					else if(antenna2==other.antenna2)
					{
						if(spw < other.spw) return true;
						else if(spw==other.spw)
							return sequenceId < other.sequenceId;
					}
				}
				return false;
			}
		};
		
		std::map<BaselineID, BaselineReader::Result*> _baselines;
};

#endif // DIRECTBASELINEREADER_H
