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
#ifndef STATISTICALFLAGGINGFRAME_H
#define STATISTICALFLAGGINGFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>

#include "../../strategy/actions/statisticalflagaction.h"

#include "../editstrategywindow.h"

class StatisticalFlaggingFrame : public Gtk::Frame {
	public:
		StatisticalFlaggingFrame(rfiStrategy::StatisticalFlagAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Statistical flagging"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_dilluteTimeSizeLabel("Dillution time size:"),
		_dilluteTimeSizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_dilluteFrequencySizeLabel("Dillution frequency size:"),
		_dilluteFrequencySizeScale(Gtk::ORIENTATION_HORIZONTAL),
		_minTimeRatioLabel("Minimum time ratio:"),
		_minTimeRatioScale(Gtk::ORIENTATION_HORIZONTAL),
		_minFreqRatioLabel("Minimum frequency ratio:"),
		_minFreqRatioScale(Gtk::ORIENTATION_HORIZONTAL),
		_applyButton("Apply")
		{
			_box.pack_start(_dilluteTimeSizeLabel);

			_dilluteTimeSizeScale.set_range(0, 100);
			_dilluteTimeSizeScale.set_value(_action.EnlargeTimeSize());
			_box.pack_start(_dilluteTimeSizeScale);

			_box.pack_start(_dilluteFrequencySizeLabel);

			_dilluteFrequencySizeScale.set_range(0, 100);
			_dilluteFrequencySizeScale.set_value(_action.EnlargeFrequencySize());
			_box.pack_start(_dilluteFrequencySizeScale);

			_box.pack_start(_minTimeRatioLabel);

			_minTimeRatioScale.set_range(0, 100);
			_minTimeRatioScale.set_value(_action.MinimumGoodTimeRatio()*100.0);
			_box.pack_start(_minTimeRatioScale);

			_box.pack_start(_minFreqRatioLabel);

			_minFreqRatioScale.set_range(0, 100);
			_minFreqRatioScale.set_value(_action.MinimumGoodFrequencyRatio()*100.0);
			_box.pack_start(_minFreqRatioScale);

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &StatisticalFlaggingFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::StatisticalFlagAction &_action;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Label _dilluteTimeSizeLabel;
		Gtk::Scale _dilluteTimeSizeScale;
		Gtk::Label _dilluteFrequencySizeLabel;
		Gtk::Scale _dilluteFrequencySizeScale;
		
		Gtk::Label _minTimeRatioLabel;
		Gtk::Scale _minTimeRatioScale;
		Gtk::Label _minFreqRatioLabel;
		Gtk::Scale _minFreqRatioScale;
		
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetEnlargeTimeSize((size_t) _dilluteTimeSizeScale.get_value());
			_action.SetEnlargeFrequencySize((size_t) _dilluteFrequencySizeScale.get_value());
			_action.SetMinimumGoodTimeRatio(_minTimeRatioScale.get_value()/100.0);
			_action.SetMinimumGoodFrequencyRatio(_minFreqRatioScale.get_value()/100.0);
			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // STATISTICALFLAGGINGFRAME_H
