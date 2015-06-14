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

#include <iostream>
#include <sstream>

#include <gtkmm/filechooserdialog.h>

#include "plotpropertieswindow.h"
#include "plot2d.h"
#include "plotwidget.h"

#include "../imagewidget.h"

PlotPropertiesWindow::PlotPropertiesWindow(Plot2D &plot, const std::string &title) :
	Gtk::Window(),
	_plot(plot),
	_applyButton("_Apply", true),
	_exportButton("_Export", true),
	_closeButton("_Close", true),
	
	_vRangeFrame("Vertical scale"),
	_minMaxVRangeButton("From min to max"),
	_winsorizedVRangeButton("Winsorized min and max"),
	_specifiedVRangeButton("Specified:"),
	_vRangeMinLabel("Scale minimum:"),
	_vRangeMaxLabel("Scale maximum:"),
	_vRangeMinEntry(),
	_vRangeMaxEntry(),
	
	_hRangeFrame("Horizontal scale"),
	_automaticHRangeButton("Automatic"),
	_hRangeMinLabel("Scale minimum:"),
	_hRangeMaxLabel("Scale maximum:"),
	_hRangeMinEntry(),
	_hRangeMaxEntry(),
	
	_optionsFrame("Options"),
	_normalOptionsButton("Normal scale"),
	_logScaleButton("Logarithmic scale"),
	_zeroSymmetricButton("Symmetric around zero"),
	
	_axesDescriptionFrame("Axes"),
	_hAxisDescriptionButton("Horizontal description"),
	_vAxisDescriptionButton("Vertical description"),
	_hAxisDescriptionEntry(),
	_vAxisDescriptionEntry(),
	
	_showAxes("Show axes"),
	_showAxisDescriptionsButton("Show axis descriptions")
{
	set_title(title);

	initVRangeWidgets();
	initHRangeWidgets();
	initOptionsWidgets();
	initAxesDescriptionWidgets();
	
	_framesHBox.pack_start(_framesRightVBox);
	
	_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onApplyClicked));
	_applyButton.set_image_from_icon_name("gtk-ok");
	_bottomButtonBox.pack_start(_applyButton);

	_exportButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onExportClicked));
	_exportButton.set_image_from_icon_name("document-save");
	_bottomButtonBox.pack_start(_exportButton);

	_closeButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onCloseClicked));
	_closeButton.set_image_from_icon_name("window-close");
	_bottomButtonBox.pack_start(_closeButton);

	_topVBox.pack_start(_framesHBox);
	
	_showAxes.set_active(_plot.ShowAxes());
	_topVBox.pack_start(_showAxes);

	_showAxisDescriptionsButton.set_active(_plot.ShowAxisDescriptions());
	_topVBox.pack_start(_showAxisDescriptionsButton);

	_topVBox.pack_start(_bottomButtonBox);

	add(_topVBox);
	_topVBox.show_all();
}

void PlotPropertiesWindow::initVRangeWidgets()
{
	_vRangeFrame.add(_vRangeBox);
	
	Gtk::RadioButton::Group group;
	
	_vRangeBox.pack_start(_minMaxVRangeButton);
	_minMaxVRangeButton.set_group(group);
	_minMaxVRangeButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onVRangeChanged));

	//_vRangeBox.pack_start(_winsorizedVRangeButton);
	_winsorizedVRangeButton.set_group(group);
	_winsorizedVRangeButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onVRangeChanged));
	_vRangeBox.pack_start(_specifiedVRangeButton);
	
	_specifiedVRangeButton.set_group(group);
	_specifiedVRangeButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onVRangeChanged));
	
	switch(_plot.VRangeDetermination())
	{
		default:
		case Plot2D::MinMaxRange: _minMaxVRangeButton.set_active(true); break;
		case Plot2D::WinsorizedRange: _winsorizedVRangeButton.set_active(true); break;
		case Plot2D::SpecifiedRange: _specifiedVRangeButton.set_active(true); break;
	}
	onVRangeChanged();

	updateVMinMaxEntries();

	_vRangeBox.pack_start(_vRangeMinLabel);
	_vRangeBox.pack_start(_vRangeMinEntry);
	
	_vRangeBox.pack_start(_vRangeMaxLabel);
	_vRangeBox.pack_start(_vRangeMaxEntry);
	
	_framesHBox.pack_start(_vRangeFrame);
}

void PlotPropertiesWindow::initHRangeWidgets()
{
	_hRangeFrame.add(_hRangeBox);
	
	Gtk::RadioButton::Group group;
	
	_hRangeBox.pack_start(_automaticHRangeButton);
	_automaticHRangeButton.set_active(_plot.HRangeDetermination() != Plot2D::SpecifiedRange);
	_automaticHRangeButton.signal_clicked().connect(sigc::mem_fun(*this, &PlotPropertiesWindow::onHRangeChanged));

	onHRangeChanged();

	updateHMinMaxEntries();

	_hRangeBox.pack_start(_hRangeMinLabel);
	_hRangeBox.pack_start(_hRangeMinEntry);
	
	_hRangeBox.pack_start(_hRangeMaxLabel);
	_hRangeBox.pack_start(_hRangeMaxEntry);
	
	_framesHBox.pack_start(_hRangeFrame);
}

void PlotPropertiesWindow::initOptionsWidgets()
{
	Gtk::RadioButton::Group group;
	
	_optionsBox.pack_start(_normalOptionsButton);
	_normalOptionsButton.set_group(group);

	_optionsBox.pack_start(_logScaleButton);
	_logScaleButton.set_group(group);

	_optionsBox.pack_start(_zeroSymmetricButton);
	_zeroSymmetricButton.set_group(group);
	
	if(_plot.LogarithmicYAxis())
		_logScaleButton.set_active(true);
	else
		_normalOptionsButton.set_active(true);

	_optionsFrame.add(_optionsBox);
	
	_framesRightVBox.pack_start(_optionsFrame);
}

void PlotPropertiesWindow::initAxesDescriptionWidgets()
{
	_axesDescriptionBox.pack_start(_hAxisDescriptionButton);
	_axesDescriptionBox.pack_start(_hAxisDescriptionEntry);
	_axesDescriptionBox.pack_start(_vAxisDescriptionButton);
	_axesDescriptionBox.pack_start(_vAxisDescriptionEntry);
	
	_axesDescriptionFrame.add(_axesDescriptionBox);
	
	_framesRightVBox.pack_start(_axesDescriptionFrame);
}

void PlotPropertiesWindow::updateHMinMaxEntries()
{
	std::stringstream minStr;
	minStr << _plot.MinX();
	_hRangeMinEntry.set_text(minStr.str());
	
	std::stringstream maxStr;
	maxStr << _plot.MaxX();
	_hRangeMaxEntry.set_text(maxStr.str());
}

void PlotPropertiesWindow::updateVMinMaxEntries()
{
	std::stringstream minStr;
	minStr << _plot.MinY();
	_vRangeMinEntry.set_text(minStr.str());
	
	std::stringstream maxStr;
	maxStr << _plot.MaxY();
	_vRangeMaxEntry.set_text(maxStr.str());
}

void PlotPropertiesWindow::onApplyClicked()
{
	if(_minMaxVRangeButton.get_active())
		_plot.SetVRangeDetermination(Plot2D::MinMaxRange);
	else if(_winsorizedVRangeButton.get_active())
		_plot.SetVRangeDetermination(Plot2D::WinsorizedRange);
	else if(_specifiedVRangeButton.get_active())
	{
		_plot.SetVRangeDetermination(Plot2D::SpecifiedRange);
		_plot.SetMinY(atof(_vRangeMinEntry.get_text().c_str()));
		_plot.SetMaxY(atof(_vRangeMaxEntry.get_text().c_str()));
	}
	
	if(_automaticHRangeButton.get_active())
		_plot.SetHRangeDetermination(Plot2D::MinMaxRange);
	else {
		_plot.SetHRangeDetermination(Plot2D::SpecifiedRange);
		_plot.SetMinX(atof(_hRangeMinEntry.get_text().c_str()));
		_plot.SetMaxX(atof(_hRangeMaxEntry.get_text().c_str()));
	}
	
	if(_normalOptionsButton.get_active())
		_plot.SetLogarithmicYAxis(false);
	else if(_logScaleButton.get_active())
		_plot.SetLogarithmicYAxis(true);
	
	if(_hAxisDescriptionButton.get_active())
		_plot.SetCustomHorizontalAxisDescription(_hAxisDescriptionEntry.get_text().c_str());
	else
		_plot.SetAutomaticHorizontalAxisDescription();
		
	if(_vAxisDescriptionButton.get_active())
		_plot.SetCustomVerticalAxisDescription(_vAxisDescriptionEntry.get_text().c_str());
	else
		_plot.SetAutomaticVerticalAxisDescription();
		
	_plot.SetShowAxes(_showAxes.get_active());
	_plot.SetShowAxisDescriptions(_showAxisDescriptionsButton.get_active());
	
	
	if(OnChangesApplied)
		OnChangesApplied();
	
	updateHMinMaxEntries();
	updateVMinMaxEntries();
}

void PlotPropertiesWindow::onCloseClicked()
{
	hide();
}

void PlotPropertiesWindow::onExportClicked()
{
	Gtk::FileChooserDialog dialog("Specify image filename", Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	dialog.add_button("_Save", Gtk::RESPONSE_OK);

	Glib::RefPtr<Gtk::FileFilter> pdfFilter = Gtk::FileFilter::create();
	std::string pdfName = "Portable Document Format (*.pdf)";
	pdfFilter->set_name(pdfName);
	pdfFilter->add_pattern("*.pdf");
	pdfFilter->add_mime_type("application/pdf");
	dialog.add_filter(pdfFilter);

	Glib::RefPtr<Gtk::FileFilter> svgFilter = Gtk::FileFilter::create();
	std::string svgName = "Scalable Vector Graphics (*.svg)";
	svgFilter->set_name(svgName);
	svgFilter->add_pattern("*.svg");
	svgFilter->add_mime_type("image/svg+xml");
	dialog.add_filter(svgFilter);

	Glib::RefPtr<Gtk::FileFilter> pngFilter = Gtk::FileFilter::create();
	std::string pngName = "Portable Network Graphics (*.png)";
	pngFilter->set_name(pngName);
	pngFilter->add_pattern("*.png");
	pngFilter->add_mime_type("image/png");
	dialog.add_filter(pngFilter);

	int result = dialog.run();

	if(result == Gtk::RESPONSE_OK)
	{
		const Glib::RefPtr<Gtk::FileFilter> filter = dialog.get_filter();
		if(filter->get_name() == pdfName)
			_plot.SavePdf(dialog.get_filename());
		else if(filter->get_name() == svgName)
			_plot.SaveSvg(dialog.get_filename());
		else
			_plot.SavePng(dialog.get_filename());
	}
}
