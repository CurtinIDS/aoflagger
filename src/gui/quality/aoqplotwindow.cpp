#include "aoqplotwindow.h"

#include <limits>

#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>

#include "../../structures/measurementset.h"

#include "../../quality/histogramtablesformatter.h"
#include "../../quality/histogramcollection.h"
#include "../../quality/statisticscollection.h"

#include "../../remote/clusteredobservation.h"
#include "../../remote/processcommander.h"

#include "antennaeplotpage.h"
#include "baselineplotpage.h"
#include "blengthplotpage.h"
#include "frequencyplotpage.h"
#include "histogrampage.h"
#include "summarypage.h"
#include "timefrequencyplotpage.h"
#include "timeplotpage.h"

AOQPlotWindow::AOQPlotWindow() :
	_activeSheetIndex(-1),
	_pageMenuButton("Sheet"),
	_baselineMI(_pageGroup, "Baselines"),
	_antennaeMI(_pageGroup, "Antennae"),
	_bLengthMI(_pageGroup, "Baseline lengths"),
	_timeMI(_pageGroup, "Time"),
	_frequencyMI(_pageGroup, "Frequency"),
	_timeFrequencyMI(_pageGroup, "Time-frequency"),
	_summaryMI(_pageGroup, "Summary"),
	_histogramMI(_pageGroup, "Histograms"),
	
	_isOpen(false)
{
	set_default_icon_name("aoqplot");
	
	_toolbar.append(_pageMenuButton);
	_pageMenuButton.set_menu(_pageMenu);
	_pageMenu.append(_baselineMI);
	_baselineMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_antennaeMI);
	_antennaeMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_bLengthMI);
	_bLengthMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_timeMI);
	_timeMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_frequencyMI);
	_frequencyMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_timeFrequencyMI);
	_timeFrequencyMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_summaryMI);
	_summaryMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.append(_histogramMI);
	_histogramMI.signal_toggled().connect(sigc::mem_fun(*this, &AOQPlotWindow::onChangeSheet));
	_pageMenu.show_all_children();
	
	_vBox.pack_start(_toolbar, Gtk::PACK_SHRINK);
	
	_vBox.pack_end(_statusBar, Gtk::PACK_SHRINK);
	_statusBar.push("Quality plot util is ready. Author: André Offringa (offringa@gmail.com)");
	
	add(_vBox);
	
	_openOptionsWindow.SignalOpen().connect(sigc::mem_fun(*this, &AOQPlotWindow::onOpenOptionsSelected));
	signal_hide().connect(sigc::mem_fun(*this, &AOQPlotWindow::onHide));
}

void AOQPlotWindow::Open(const std::vector<std::string> &files)
{
	show_all();
	_openOptionsWindow.ShowForFile(files);
}

void AOQPlotWindow::onOpenOptionsSelected(const std::vector<std::string>& files, bool downsampleTime, bool downsampleFreq, size_t timeCount, size_t freqCount, bool correctHistograms)
{
	readStatistics(files, downsampleTime, downsampleFreq, timeCount, freqCount, correctHistograms);
	_activeSheetIndex = -1;
	onChangeSheet();
	show();
}

void AOQPlotWindow::close()
{
	if(_isOpen)
	{
		_activeSheet->CloseStatistics();
		delete _statCollection;
		delete _histCollection;
		delete _fullStats;
		_isOpen = false;
	}
}

void AOQPlotWindow::readDistributedObservation(const std::string& filename, bool correctHistograms)
{
	aoRemote::ClusteredObservation *observation = aoRemote::ClusteredObservation::Load(filename);
	_statCollection = new StatisticsCollection();
	_histCollection = new HistogramCollection();
	aoRemote::ProcessCommander commander(*observation);
	commander.PushReadAntennaTablesTask();
	commander.PushReadQualityTablesTask(_statCollection, _histCollection, correctHistograms);
	commander.Run();
	if(!commander.Errors().empty())
	{
		std::stringstream s;
		s << commander.Errors().size() << " error(s) occured while querying the nodes or measurement sets in the given observation. This might be caused by a failing node, an unreadable measurement set, or maybe the quality tables are not available. The errors reported are:\n\n";
		size_t count = 0;
		for(std::vector<std::string>::const_iterator i=commander.Errors().begin();i!=commander.Errors().end() && count < 30;++i)
		{
			s << "- " << *i << '\n';
			++count;
		}
		if(commander.Errors().size() > 30)
		{
			s << "... and " << (commander.Errors().size()-30) << " more.\n";
		}
		s << "\nThe program will continue, but this might mean that the statistics are incomplete. If this is the case, fix the issues and reopen the observation.";
		std::cerr << s.str() << std::endl;
		Gtk::MessageDialog dialog(*this, s.str(), false, Gtk::MESSAGE_ERROR);
		dialog.run();
	}
	
	_antennas = commander.Antennas();
	
	delete observation;
}

void AOQPlotWindow::readMetaInfoFromMS(const string& filename)
{
	MeasurementSet *ms = new MeasurementSet(filename);
	_polarizationCount = ms->PolarizationCount();
	unsigned antennaCount = ms->AntennaCount();
	_antennas.clear();
	for(unsigned a=0;a<antennaCount;++a)
		_antennas.push_back(ms->GetAntennaInfo(a));
	delete ms;
}

void AOQPlotWindow::readAndCombine(const string& filename)
{
	std::cout << "Adding " << filename << " to statistics...\n";
	QualityTablesFormatter qualityTables(filename);
	StatisticsCollection statCollection(_polarizationCount);
	statCollection.Load(qualityTables);
	_statCollection->Add(statCollection);
	
	HistogramTablesFormatter histogramTables(filename);
	if(histogramTables.HistogramsExist())
	{
		HistogramCollection histCollection(_polarizationCount);
		histCollection.Load(histogramTables);
		_histCollection->Add(histCollection);
	}
}

void AOQPlotWindow::readStatistics(const std::vector<std::string>& files, bool downsampleTime, bool downsampleFreq, size_t timeSize, size_t freqSize, bool correctHistograms)
{
	close();
	
	if(!files.empty())
	{
		const std::string& firstFile = *files.begin();
		if(aoRemote::ClusteredObservation::IsClusteredFilename(firstFile))
		{
			if(files.size() != 1)
				throw std::runtime_error("You are trying to open multiple distributed or clustered sets. Can only open multiple files if they are not distributed.");
			readDistributedObservation(firstFile, correctHistograms);
		}
		else {
			readMetaInfoFromMS(firstFile);
			
			_statCollection = new StatisticsCollection(_polarizationCount);
			_histCollection = new HistogramCollection(_polarizationCount);
			
			for(size_t i=0; i!=files.size(); ++i)
			{
				std::cout << " (" << (i+1) << "/" << files.size() << ") ";
				readAndCombine(files[i]);
			}
		}
		setShowHistograms(!_histCollection->Empty());
		if(downsampleTime)
		{
			std::cout << "Lowering time resolution..." << std::endl;
			_statCollection->LowerTimeResolution(timeSize);
		}

		if(downsampleFreq)
		{
			std::cout << "Lowering frequency resolution..." << std::endl;
			_statCollection->LowerFrequencyResolution(freqSize);
		}

		std::cout << "Integrating baseline statistics to one channel..." << std::endl;
		_statCollection->IntegrateBaselinesToOneChannel();
		
		std::cout << "Regridding time statistics..." << std::endl;
		_statCollection->RegridTime();
		
		std::cout << "Copying statistics..." << std::endl;
		_fullStats = new StatisticsCollection(*_statCollection);
		
		std::cout << "Integrating time statistics to one channel..." << std::endl;
		_statCollection->IntegrateTimeToOneChannel();
		
		std::cout << "Opening statistics panel..." << std::endl;
		_isOpen = true;
	}
}

void AOQPlotWindow::onStatusChange(const std::string &newStatus)
{
	_statusBar.pop();
	_statusBar.push(newStatus);
}

void AOQPlotWindow::Save(const AOQPlotWindow::PlotSavingData& data)
{
	const std::string& prefix = data.filenamePrefix;
	QualityTablesFormatter::StatisticKind kind = data.statisticKind;
	
	std::cout << "Saving " << prefix << "-antennas.pdf...\n";
	AntennaePlotPage antPage;
	antPage.SetStatistics(_statCollection, _antennas);
	antPage.SavePdf(prefix+"-antennas.pdf", kind);
	
	std::cout << "Saving " << prefix << "-baselines.pdf...\n";
	BaselinePlotPage baselPage;
	baselPage.SetStatistics(_statCollection, _antennas);
	baselPage.SavePdf(prefix+"-baselines.pdf", kind);
	
	std::cout << "Saving " << prefix << "-baselinelengths.pdf...\n";
	BLengthPlotPage blenPage;
	blenPage.SetStatistics(_statCollection, _antennas);
	blenPage.SavePdf(prefix+"-baselinelengths.pdf", kind);
	
	std::cout << "Saving " << prefix << "-timefrequency.pdf...\n";
	TimeFrequencyPlotPage tfPage;
	tfPage.SetStatistics(_fullStats, _antennas);
	tfPage.SavePdf(prefix+"-timefrequency.pdf", kind);
	
	std::cout << "Saving " << prefix << "-time.pdf...\n";
	TimePlotPage timePage;
	timePage.SetStatistics(_statCollection, _antennas);
	timePage.SavePdf(prefix+"-time.pdf", kind);
	
	std::cout << "Saving " << prefix << "-frequency.pdf...\n";
	FrequencyPlotPage freqPage;
	freqPage.SetStatistics(_statCollection, _antennas);
	freqPage.SavePdf(prefix+"-frequency.pdf", kind);
}

void AOQPlotWindow::onChangeSheet()
{
	int selectedSheet = -1;
	if(_baselineMI.get_active())
		selectedSheet = 0;
	else if(_antennaeMI.get_active())
		selectedSheet = 1;
	else if(_bLengthMI.get_active())
		selectedSheet = 2;
	else if(_timeMI.get_active())
		selectedSheet = 3;
	else if(_frequencyMI.get_active())
		selectedSheet = 4;
	else if(_timeFrequencyMI.get_active())
		selectedSheet = 5;
	else if(_summaryMI.get_active())
		selectedSheet = 6;
	else if(_histogramMI.get_active())
		selectedSheet = 7;
	
	if(selectedSheet != _activeSheetIndex)
	{
		switch(selectedSheet)
		{
			case 0: _activeSheet.reset(new BaselinePlotPage()); break;
			case 1: _activeSheet.reset(new AntennaePlotPage()); break;
			case 2: _activeSheet.reset(new BLengthPlotPage()); break;
			case 3: _activeSheet.reset(new TimePlotPage()); break;
			case 4: _activeSheet.reset(new FrequencyPlotPage()); break;
			case 5: _activeSheet.reset(new TimeFrequencyPlotPage()); break;
			case 6: _activeSheet.reset(new SummaryPage()); break;
			case 7: _activeSheet.reset(new HistogramPage()); break;
		}
		switch(selectedSheet)
		{
			case 0: SetStatus("Baseline statistics"); break;
			case 1: SetStatus("Antennae statistics"); break;
			case 2: SetStatus("Baseline length statistics");  break;
			case 3: SetStatus("Time statistics"); break;
			case 4: SetStatus("Frequency statistics"); break;
			case 5: SetStatus("Time-frequency statistics");  break;
			case 6: SetStatus("Summary"); break;
			case 7: SetStatus("Histograms"); break;
		}
		
		_activeSheetIndex = selectedSheet;
		if(selectedSheet == 5)
			_activeSheet->SetStatistics(_fullStats, _antennas);
		else
			_activeSheet->SetStatistics(_statCollection, _antennas);
		_activeSheet->SetHistograms(_histCollection);
		_activeSheet->SignalStatusChange().connect(sigc::mem_fun(*this, &AOQPlotWindow::onStatusChange));
		_activeSheet->InitializeToolbar(_toolbar);
		_toolbar.show_all();
		_vBox.pack_start(*_activeSheet);
		_activeSheet->show_all();
	}
}
