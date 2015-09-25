#ifndef PLOTMANAGER_H
#define PLOTMANAGER_H

#include <boost/function.hpp>

#include <set>
#include <vector>

#include "plot2d.h"

class PlotManager
{
	public:
		~PlotManager()
		{
			Clear();
		}
		Plot2D &NewPlot2D(const std::string &plotTitle)
		{
			std::string title = plotTitle;
			if(_plotTitles.find(title) != _plotTitles.end())
			{
				char addChar = 'B';
				std::string tryTitle;
				do {
					tryTitle = title + " (" + addChar + ')';
					++addChar;
				} while(_plotTitles.find(tryTitle) != _plotTitles.end() && addChar <= 'Z');
				if(addChar > 'Z')
					tryTitle = title + " (..)";
				title = tryTitle;
			}
			Plot2D *plot = new Plot2D();
			plot->SetTitle(title);
			_plotTitles.insert(title);
			_items.push_back(plot);
			return *plot;
		}
		
		void Update() { _onUpdate(); }
		
		boost::function<void()>& OnUpdate() { return _onUpdate; }
		
		const std::vector<Plot2D*>& Items() const { return _items; }
		
		void Clear()
		{
			for(std::vector<Plot2D*>::const_iterator i=_items.begin(); i!=_items.end(); ++i)
				delete *i;
			_items.clear();
			Update();
		}
	private:
		std::vector<Plot2D*> _items;
		std::set<std::string> _plotTitles;
		
		boost::function<void()> _onUpdate;
};

#endif
