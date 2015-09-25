#ifndef PLOT_PLOT2D_H
#define PLOT_PLOT2D_H

class Plotable
{
	public:
		virtual ~Plotable() { }
		virtual void Render(Gtk::DrawingArea &drawingArea) = 0;
};

#endif // PLOT_PLOT2D_H
