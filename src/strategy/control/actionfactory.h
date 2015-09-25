#ifndef RFISTRATEGYACTIONFACTORY_H
#define RFISTRATEGYACTIONFACTORY_H

#include <string>
#include <vector>

#include "../actions/action.h"

namespace rfiStrategy {

	class ActionFactory {
		public:
			static const std::vector<std::string> GetActionList();
			static class Action *CreateAction(const std::string &action);
			static const char *GetDescription(ActionType actionType);
		private:
			ActionFactory();
			~ActionFactory();
	};
}

#endif // RFISTRATEGYACTIONFACTORY_H
