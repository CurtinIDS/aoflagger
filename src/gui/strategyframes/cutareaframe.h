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
#ifndef CUTAREAFRAME_H
#define CUTAREAFRAME_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/radiobutton.h>

#include "../../strategy/actions/cutareaaction.h"

#include "../editstrategywindow.h"

class CutAreaFrame : public Gtk::Frame {
	public:
		CutAreaFrame(rfiStrategy::CutAreaAction &action, EditStrategyWindow &editStrategyWindow)
		: Gtk::Frame("Cut area"),
		_editStrategyWindow(editStrategyWindow), _action(action),
		_startTimeLabel("Start time steps to cut:"),
		_startTimeScale(),
		_endTimeLabel("End time steps to cut:"),
		_endTimeScale(),
		_topChannelsLabel("Top channels to cut:"),
		_topChannelsScale(),
		_bottomChannelsLabel("Bottom channels to cut:"),
		_bottomChannelsScale(),
		_applyButton("Apply")
		{
			_box.pack_start(_startTimeLabel);
			_startTimeScale.set_range(0, 64);
			_box.pack_start(_startTimeScale);
			_startTimeScale.set_value(_action.StartTimeSteps());

			_box.pack_start(_endTimeLabel);
			_endTimeScale.set_range(0, 64);
			_box.pack_start(_endTimeScale);
			_endTimeScale.set_value(_action.EndTimeSteps());

			_box.pack_start(_topChannelsLabel);
			_topChannelsScale.set_range(0, 64);
			_box.pack_start(_topChannelsScale);
			_topChannelsScale.set_value(_action.TopChannels());

			_box.pack_start(_bottomChannelsLabel);
			_bottomChannelsScale.set_range(0, 64);
			_box.pack_start(_bottomChannelsScale);
			_bottomChannelsScale.set_value(_action.BottomChannels());

			_buttonBox.pack_start(_applyButton);
			_applyButton.signal_clicked().connect(sigc::mem_fun(*this, &CutAreaFrame::onApplyClicked));

			_box.pack_start(_buttonBox);

			add(_box);
			_box.show_all();
		}
	private:
		EditStrategyWindow &_editStrategyWindow;
		rfiStrategy::CutAreaAction &_action;

		Gtk::Label _startTimeLabel;
		Gtk::Scale _startTimeScale;
		Gtk::Label _endTimeLabel;
		Gtk::Scale _endTimeScale;
		Gtk::Label _topChannelsLabel;
		Gtk::Scale _topChannelsScale;
		Gtk::Label _bottomChannelsLabel;
		Gtk::Scale _bottomChannelsScale;

		Gtk::VBox _box;
		Gtk::ButtonBox _buttonBox;
		Gtk::Button _applyButton;

		void onApplyClicked()
		{
			_action.SetStartTimeSteps((int) _startTimeScale.get_value());
			_action.SetEndTimeSteps((int) _endTimeScale.get_value());
			_action.SetTopChannels((int) _topChannelsScale.get_value());
			_action.SetBottomChannels((int) _bottomChannelsScale.get_value());

			_editStrategyWindow.UpdateAction(&_action);
		}
};

#endif // CUTAREAFRAME_H
