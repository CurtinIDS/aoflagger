#include <limits>

#include "timefrequencyplotpage.h"

#include "../../quality/statisticscollection.h"
#include "../../quality/statisticsderivator.h"

TimeFrequencyPlotPage::TimeFrequencyPlotPage() :
	_statCollection(0)
{
	GrayScaleWidget().OnMouseMovedEvent().connect(sigc::mem_fun(*this, &TimeFrequencyPlotPage::onMouseMoved));
	GrayScaleWidget().SetXAxisDescription("Time index");
	GrayScaleWidget().SetYAxisDescription("Frequency index");
}

TimeFrequencyPlotPage::~TimeFrequencyPlotPage()
{
}

std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> TimeFrequencyPlotPage::ConstructImage(QualityTablesFormatter::StatisticKind kind)
{
	if(HasStatistics())
	{
		StatisticsDerivator derivator(*_statCollection);
		
		std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> data = derivator.CreateTFData(kind);
		if(data.second == 0)
		{
			GrayScaleWidget().SetXAxisDescription("Time index");
			GrayScaleWidget().SetYAxisDescription("Frequency index");
		} else {
			GrayScaleWidget().SetXAxisDescription("Time");
			GrayScaleWidget().SetYAxisDescription("Frequency (MHz)");
		}
		return data;
	} else {
		return std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr>(TimeFrequencyData(), TimeFrequencyMetaDataCPtr());
	}
}

void TimeFrequencyPlotPage::onMouseMoved(size_t x, size_t y)
{
	std::stringstream text;

	const QualityTablesFormatter::StatisticKind kind = GetSelectedStatisticKind();
	const std::string &kindName = QualityTablesFormatter::KindToName(kind);
	
	text << kindName << " = " << GrayScaleWidget().Image()->Value(x, y) << " (" << x << ", " << y << ")";
	_signalStatusChange(text.str());
}
