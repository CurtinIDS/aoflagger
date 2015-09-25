#include "../actions/action.h"

#ifndef RFIACTIONCONTAINER_H
#define RFIACTIONCONTAINER_H 

#include <vector>

namespace rfiStrategy {

	class ActionContainer : public Action
	{
		public:
			typedef std::vector<class Action*>::const_iterator const_iterator;
			typedef std::vector<class Action*>::iterator iterator;

			virtual ~ActionContainer();
			void Add(class Action *newAction);
			void RemoveAndDelete(class Action *action);
			void RemoveWithoutDelete(class Action *action);
			void RemoveAll();
			size_t GetChildCount() const throw() { return _childActions.size(); }
			Action &GetChild(size_t index) const { return *_childActions[index]; }
			Action &GetFirstChild() const { return *_childActions.front(); }
			Action &GetLastChild() const { return *_childActions.back(); }
			void MoveChildUp(size_t childIndex)
			{
				if(childIndex > 0)
				{
					class Action *movedAction = _childActions[childIndex];
					_childActions[childIndex] = _childActions[childIndex-1];
					_childActions[childIndex-1] = movedAction;
				}
			}
			void MoveChildDown(size_t childIndex)
			{
				if(childIndex < _childActions.size()-1)
				{
					class Action *movedAction = _childActions[childIndex];
					_childActions[childIndex] = _childActions[childIndex+1];
					_childActions[childIndex+1] = movedAction;
				}
			}
			void InitializeAll();
			void FinishAll();

			iterator begin() { return _childActions.begin(); }
			iterator end() { return _childActions.end(); }
			const_iterator begin() const { return _childActions.begin(); }
			const_iterator end() const { return _childActions.end(); }
		private:
			std::vector<class Action*> _childActions;
	};
}

#endif // RFIACTIONCONTAINER_H
