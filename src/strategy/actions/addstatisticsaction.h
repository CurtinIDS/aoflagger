#ifndef RFIADDSTATISTICSACTION_H
#define RFIADDSTATISTICSACTION_H

#include "../control/actioncontainer.h"
#include "../control/artifactset.h"

#include <string>

#include "../../util/progresslistener.h"

#include "../algorithms/rfistatistics.h"
#include "../algorithms/noisestatistics.h"

namespace rfiStrategy {

	class AddStatisticsAction : public Action
	{
		public:
			AddStatisticsAction() : _comparison(false), _separateBaselineStatistics(false), _performClassification(true), _writeImmediately(false)
			{
			}
			
			virtual ~AddStatisticsAction()
			{
				Sync();
			}

			virtual std::string Description()
			{
				return "Add to statistics";
			}
			
			virtual void Sync()
			{
				statistics.Save();
			}
			
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &)
			{
				statistics.SetSeparateBaselineStatistics(_separateBaselineStatistics);
				statistics.SetPerformClassification(_performClassification);
				statistics.SetWriteImmediately(_writeImmediately);
				if(_comparison)
					statistics.Add(artifacts.ContaminatedData(), artifacts.MetaData(), artifacts.OriginalData().GetSingleMask());
				else
					statistics.Add(artifacts.ContaminatedData(), artifacts.MetaData());
			}
			virtual ActionType Type() const { return AddStatisticsActionType; }
			
			void SetFilePrefix(const std::string &filePrefix) { statistics.SetFilePrefix(filePrefix); }
			const std::string &FilePrefix() const { return statistics.FilePrefix(); }

			bool CompareOriginalAndAlternative() const { return _comparison; }
			void SetCompareOriginalAndAlternative(bool compare) { _comparison = compare; }

			bool SeparateBaselineStatistics() const { return _separateBaselineStatistics; }
			void SetSeparateBaselineStatistics(bool separateBaselineStatistics)
			{
				_separateBaselineStatistics = separateBaselineStatistics;
			}

			bool PerformClassification() const { return _performClassification; }
			void SetPerformClassification(bool performClassification)
			{
				_performClassification = performClassification;
			}

			bool WriteImmediately() const { return _writeImmediately; }
			void SetWriteImmediately(bool writeImmediately)
			{
				_writeImmediately = writeImmediately;
			}
		private:
			RFIStatistics statistics;
			bool _comparison;
			bool _separateBaselineStatistics;
			bool _performClassification;
			bool _writeImmediately;
	};
}

#endif // RFIADDSTATISTICSACTION_H
