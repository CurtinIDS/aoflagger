#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <gtkmm/drawingarea.h>

#include "plotable.h"

class PlotWidget : public Gtk::DrawingArea {
	public:
		PlotWidget() : _plot(0)
		{
			signal_draw().connect(sigc::mem_fun(*this, &PlotWidget::onDraw) );
			set_size_request(400, 300);
		}
		
		~PlotWidget()
		{
		}

		Plotable &Plot() const
		{
			return *_plot; 
		}
		void SetPlot(Plotable &plot)
		{
			_plot = &plot;
			redraw();
		}
		void Clear()
		{
			_plot = 0;
			redraw();
		}
		void Update()
		{
			redraw();
			Glib::RefPtr<Gdk::Window> window = get_window();
			if(window)
				window->invalidate(false);
		}
	private:
		Plotable *_plot;

		bool onDraw(const Cairo::RefPtr<Cairo::Context >& cr)
		{
			redraw();
			return true;
		}

		void redraw()
		{
			if(_plot != 0)
				_plot->Render(*this);
			else {
				Glib::RefPtr<Gdk::Window> window = get_window();
				if(window)
				{
					Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
					cr->set_source_rgba(1, 1, 1, 1);
					cr->paint();
					cr->fill();
				}
			}
		}
};

#endif
