#ifndef PLOTACTION_H
#define PLOTACTION_H 

#include "action.h"

namespace rfiStrategy {

	class PlotAction : public Action
	{
		public:
			enum PlotKind {
				AntennaFlagCountPlot, FrequencyFlagCountPlot, FrequencyPowerPlot, TimeFlagCountPlot,
				BaselineSpectrumPlot, PolarizationStatisticsPlot, BaselineRMSPlot, IterationsPlot
			};

			PlotAction() : _plotKind(FrequencyPowerPlot), _logYAxis(false) { }
			virtual ~PlotAction() { }
			virtual std::string Description()
			{
				switch(_plotKind)
				{
					case AntennaFlagCountPlot:
					return "Plot antenna flag counts";
					case FrequencyFlagCountPlot:
					return "Plot frequency flag counts";
					case FrequencyPowerPlot:
					return "Plot frequency power";
					case TimeFlagCountPlot:
					return "Plot time flag counts";
					case BaselineSpectrumPlot:
					return "Plot spectrum per baseline";
					case PolarizationStatisticsPlot:
					return "Plot polarization flag counts";
					case BaselineRMSPlot:
					return "Plot baseline RMS";
					case IterationsPlot:
					return "Plot iteration convergence";
					default:
					return "Unknown plot action";
				}
			}
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &listener);
			virtual ActionType Type() const { return PlotActionType; }

			enum PlotKind PlotKind() const { return _plotKind; }
			void SetPlotKind(enum PlotKind plotKind) { _plotKind = plotKind; }

			bool LogarithmicYAxis() const { return _logYAxis; }
			void SetLogarithmicYAxis(bool logYAxis) { _logYAxis = logYAxis; }
		private:
			enum PlotKind _plotKind;
			bool _logYAxis;
			boost::mutex _plotMutex;

			void plotAntennaFlagCounts(class ArtifactSet &artifacts);
			void plotFrequencyFlagCounts(class ArtifactSet &artifacts);
			void plotFrequencyPower(class ArtifactSet &artifacts);
			void plotTimeFlagCounts(class ArtifactSet &artifacts);
			void plotSpectrumPerBaseline(class ArtifactSet &artifacts);
			void plotPolarizationFlagCounts(class ArtifactSet &artifacts);
			void plotBaselineRMS(class ArtifactSet &artifacts);
			void plotIterations(class ArtifactSet &artifacts);
	};

}

#endif // PLOTACTION_H

