#ifndef GUI_QUALITY__OPEN_OPTIONS_WINDOW_H
#define GUI_QUALITY__OPEN_OPTIONS_WINDOW_H

#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/window.h>

class OpenOptionsWindow : public Gtk::Window {
	public:
		OpenOptionsWindow() :
			_downsampleTimeButton("Lower time resolution (faster plots)"),
			_downsampleFreqButton("Lower frequency resolution (faster plots)"),
			_correctHistograms("Correct histograms for frequence response"),
			_cancelButton("_Cancel", true),
			_openButton("_Open", true)
		{
			_timeBox.pack_start(_downsampleTimeButton);
			_downsampleTimeButton.set_active(true);
			
			_timeBox.pack_start(_timeDownsampleEntry);
			_timeDownsampleEntry.set_text("1000");
			
			_box.pack_start(_timeBox);
			
			_freqBox.pack_start(_downsampleFreqButton);
			_downsampleFreqButton.set_active(true);
			
			_freqBox.pack_start(_freqDownsampleEntry);
			_freqDownsampleEntry.set_text("1000");
			
			_box.pack_start(_freqBox);
			
			_box.pack_start(_correctHistograms);
			
			_buttonBox.pack_start(_cancelButton);
			
			_buttonBox.pack_start(_openButton);
			_openButton.signal_clicked().connect(sigc::mem_fun(*this, &OpenOptionsWindow::onOpen));
			
			_box.pack_start(_buttonBox);
			
			add(_box);
			_box.show_all();
		}
		
    ~OpenOptionsWindow()
    {
		}
		
		void ShowForFile(const std::vector<std::string>&files)
		{
			_files = files;
			present();
		}
		
		void ShowForFile(const std::string& filename)
		{
			_files.clear();
			_files.push_back(filename);
			present();
		}
		
		sigc::signal<void, const std::vector<std::string>&, bool, bool, size_t, size_t, bool> &SignalOpen() { return _signalOpen; }
	private:
		void onOpen()
		{
			hide();
			size_t timeRes = atol(_timeDownsampleEntry.get_text().c_str());
			size_t freqRes = atol(_freqDownsampleEntry.get_text().c_str());
			_signalOpen.emit(_files, _downsampleTimeButton.get_active(), _downsampleFreqButton.get_active(), timeRes, freqRes,
			_correctHistograms.get_active()
			);
			_files.clear();
		}
		
		Gtk::VBox _box;
		Gtk::HBox _timeBox;
		Gtk::CheckButton _downsampleTimeButton;
		Gtk::Entry _timeDownsampleEntry;
		Gtk::HBox _freqBox;
		Gtk::CheckButton _downsampleFreqButton;
		Gtk::Entry _freqDownsampleEntry;
		Gtk::CheckButton _correctHistograms;
		Gtk::ButtonBox _buttonBox;
		Gtk::Button _cancelButton, _openButton;
		sigc::signal<void, const std::vector<std::string>&, bool, bool, size_t, size_t, bool> _signalOpen;
		
		std::vector<std::string> _files;
};

#endif
