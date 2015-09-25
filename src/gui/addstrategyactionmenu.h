#ifndef ADDSTRATEGYACTIONMENU_H
#define ADDSTRATEGYACTIONMENU_H

#include <gtkmm/menu.h>

#include "../strategy/control/actionfactory.h"

#include "editstrategywindow.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class AddStrategyActionMenu : public Gtk::Menu {
	public:
		AddStrategyActionMenu(class EditStrategyWindow &editStrategyWindow) :
		_editStrategyWindow(editStrategyWindow)
		{
			std::vector<std::string> actions = rfiStrategy::ActionFactory::GetActionList();

			for(std::vector<std::string>::const_iterator i=actions.begin();i!=actions.end();++i)
			{
				Gtk::MenuItem *item = new Gtk::MenuItem(*i);
				append(*item);
				item->signal_activate().connect(sigc::bind<const std::string>(sigc::mem_fun(*this, &AddStrategyActionMenu::onActionSelected), *i));
				item->show();

				_items.push_back(item);
			}
		}
		
		~AddStrategyActionMenu()
		{
			for(std::vector<Gtk::MenuItem *>::const_iterator i=_items.begin();i!=_items.end();++i)
				delete *i;
		}
		
	private:
		void onActionSelected(const std::string str)
		{
			_editStrategyWindow.AddAction(rfiStrategy::ActionFactory::CreateAction(str));
		}

		class EditStrategyWindow &_editStrategyWindow;
		std::vector<Gtk::MenuItem *> _items;
};

#endif
