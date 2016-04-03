#ifndef GUI_QUALITY__SUMMARYPAGE_H
#define GUI_QUALITY__SUMMARYPAGE_H

#include <gtkmm/box.h>
#include <gtkmm/textbuffer.h>
#include <gtkmm/textview.h>

#include "../../quality/statisticscollection.h"
#include "../../quality/statisticsderivator.h"

#include "plotsheet.h"

class SummaryPage : public PlotSheet {
	public:
		SummaryPage()
		{
			add(_textView);
			_textView.show();
			
			CloseStatistics();
		}
		
		virtual void SetStatistics(const StatisticsCollection* statCollection, const std::vector<class AntennaInfo>&) override final
		{
			_statCollection = statCollection;
			updateText();
		}
		void CloseStatistics()
		{
			_statCollection = 0;
			_textView.get_buffer()->set_text("No open measurement set");
		}
		bool HasStatistics() const
		{
			return _statCollection != 0;
		}
		
	private:
		void updateText()
		{
			DefaultStatistics statistics(_statCollection->PolarizationCount());
			Glib::RefPtr<Gtk::TextBuffer> buffer = _textView.get_buffer();
			
			buffer->set_text("Summary of statistics\n\n");
			
			_statCollection->GetGlobalCrossBaselineStatistics(statistics);
			buffer->insert(buffer->end(), "Statistics of cross-correlated baselines\n");
			addText(statistics);
			
			buffer->insert(buffer->end(), "\nAverages over cross-correlated baselines\n");
			std::ostringstream s;
			addBaselineAverages(s);
			buffer->insert(buffer->end(), s.str());

			_statCollection->GetGlobalAutoBaselineStatistics(statistics);
			buffer->insert(buffer->end(), "\nStatistics of auto-correlated baselines\n");
			addText(statistics);
		}
		
		void addText(DefaultStatistics &statistics)
		{
			Glib::RefPtr<Gtk::TextBuffer > buffer = _textView.get_buffer();
			
			unsigned long totalRFICount = 0;
			unsigned long totalCount = 0;
			const unsigned polarizationCount = statistics.PolarizationCount();
			std::vector<double>
				stdDev(polarizationCount), dStdDev(polarizationCount),
				variance(polarizationCount), dVariance(polarizationCount);
			for(unsigned p=0;p<polarizationCount;++p)
			{
				totalRFICount += statistics.rfiCount[p];
				totalCount += statistics.rfiCount[p] + statistics.count[p];
				variance[p] = StatisticsDerivator::VarianceAmplitude(statistics.count[p], statistics.sum[p], statistics.sumP2[p]);
				dVariance[p] = StatisticsDerivator::VarianceAmplitude(statistics.dCount[p], statistics.dSum[p], statistics.dSumP2[p]);
				stdDev[p] = StatisticsDerivator::StandardDeviationAmplitude(statistics.count[p], statistics.sum[p], statistics.sumP2[p]);
				dStdDev[p] = StatisticsDerivator::StandardDeviationAmplitude(statistics.dCount[p], statistics.dSum[p], statistics.dSumP2[p]);
			}
			
			double rfiRatioValue = round(((double) totalRFICount * 10000.0 / (double) totalCount)) * 0.01;
			
			double countExp = floor(log10(totalCount));
			double countMantissa = totalCount / exp10(countExp);
			
			std::ostringstream s;
			s << "Sample count = " << round(countMantissa*100.0)*0.01 << " x 10^" << countExp << "\n";
			s << "Total RFI ratio = " << rfiRatioValue << "%\n";
			s << "Standard deviation amplitude = ";
			addValues(&stdDev[0], polarizationCount, s);
			s << " Jy\nDifferential stddev amplitude = ";
			addValues(&dStdDev[0], polarizationCount, s);
			s << " Jy\nVariance amplitude = ";
			addValues(&variance[0], polarizationCount, s);
			s << " Jy\nDifferential variance amplitude = ";
			addValues(&dVariance[0], polarizationCount, s);
			s << " Jy\n";
			buffer->insert(buffer->end(), s.str());
		}
		
		void addBaselineAverages(std::ostringstream &s)
		{
			const BaselineStatisticsMap &map = _statCollection->BaselineStatistics();
			std::vector<std::pair <unsigned, unsigned> > list = map.BaselineList();
			std::vector<double>
				totalStdDev(map.PolarizationCount()),
				totalSNR(map.PolarizationCount());
			std::vector<size_t> count(map.PolarizationCount());
			for(size_t p=0;p<map.PolarizationCount();++p)
			{
				totalStdDev[p] = 0.0;
				totalSNR[p] = 0.0;
				count[p] = 0;
			}
			for(std::vector<std::pair <unsigned, unsigned> >::const_iterator i=list.begin(); i!=list.end(); ++i)
			{
				unsigned a1=i->first, a2=i->second;
				if(a1 != a2)
				{
					const DefaultStatistics &stat = map.GetStatistics(a1, a2);
					for(size_t p=0;p<map.PolarizationCount();++p)
					{
						const double
							thisStdDev = StatisticsDerivator::GetStatisticAmplitude(QualityTablesFormatter::StandardDeviationStatistic, stat, p),
							thisSNR = StatisticsDerivator::GetStatisticAmplitude(QualityTablesFormatter::SignalToNoiseStatistic, stat, p);
						if(std::isfinite(thisStdDev) && std::isfinite(thisSNR))
						{
							totalStdDev[p] += thisStdDev;
							totalSNR[p] += thisSNR;
							++count[p];
						}
					}
				}
			}
			for(size_t p=0;p<map.PolarizationCount();++p)
			{
				totalStdDev[p] /= (double) count[p];
				totalSNR[p] /= (double) count[p];
			}
			s << "Average standard deviation = ";
			addValues(&totalStdDev[0], map.PolarizationCount(), s);
			s << " Jy\nAverage signal to noise ratio = ";
			addValues(&totalSNR[0], map.PolarizationCount(), s);
			s << " Jy\n(calculated with BaselineMean/BaselineDStdDev)\n";
		}

		void addValues(const double *values, unsigned polarizationCount, std::ostringstream &s)
		{
			s << '[' << values[0];
			for(unsigned p=1;p<polarizationCount;++p)
			{
				s << ", " << values[p];
			}
			s << ']';
		}

		Gtk::TextView _textView;
		
		const StatisticsCollection *_statCollection;
};

#endif
