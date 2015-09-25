#include "strategy.h"

#include "foreachmsaction.h"
#include "writeflagsaction.h"

#include "../control/strategyiterator.h"

namespace rfiStrategy {

	ArtifactSet *Strategy::JoinThread()
	{
		ArtifactSet *artifact = 0;
		if(_thread != 0)
		{
			_thread->join();
			delete _thread;
			artifact = new ArtifactSet(*_threadFunc->_artifacts);
			delete _threadFunc->_artifacts;
			delete _threadFunc;
		}
		_thread = 0;
		return artifact;
	}

	void Strategy::StartPerformThread(const ArtifactSet &artifacts, ProgressListener &progress)
	{
		JoinThread();
		_threadFunc = new PerformFunc(this, new ArtifactSet(artifacts), &progress);
		_thread = new boost::thread(*_threadFunc);
	}

	void Strategy::PerformFunc::operator()()
	{
		// Perform will catch exceptions
		_strategy->Perform(*_artifacts, *_progress);
	}

	void Strategy::SetThreadCount(ActionContainer &strategy, size_t threadCount)
	{
		StrategyIterator i = StrategyIterator::NewStartIterator(strategy);
		while(!i.PastEnd())
		{
			if(i->Type() == ForEachBaselineActionType)
			{
				ForEachBaselineAction &fobAction = static_cast<ForEachBaselineAction&>(*i);
				fobAction.SetThreadCount(threadCount);
			}
			if(i->Type() == WriteFlagsActionType)
			{
				WriteFlagsAction &writeAction = static_cast<WriteFlagsAction&>(*i);
				writeAction.SetMaxBufferItems(threadCount*5);
				writeAction.SetMinBufferItemsForWriting(threadCount*4);
			}
			++i;
		}
	}

	void Strategy::SetDataColumnName(Strategy &strategy, const std::string &dataColumnName)
	{
		StrategyIterator i = StrategyIterator::NewStartIterator(strategy);
		while(!i.PastEnd())
		{
			if(i->Type() == ForEachMSActionType)
			{
				ForEachMSAction &action = static_cast<ForEachMSAction&>(*i);
				action.SetDataColumnName(dataColumnName);
			}
			++i;
		}
	}

	void Strategy::SyncAll(ActionContainer &root)
	{
		StrategyIterator i = StrategyIterator::NewStartIterator(root);
		while(!i.PastEnd())
		{
			i->Sync();
			++i;
		}
	}
}
