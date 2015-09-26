#ifndef FOREACHMSACTION_H
#define FOREACHMSACTION_H

#include "../control/actionblock.h"

#include "../../structures/types.h"

#include <set>

namespace rfiStrategy {

	class ForEachMSAction  : public ActionBlock {
		public:
			ForEachMSAction() : _readUVW(false), _dataColumnName("DATA"), _subtractModel(false), _skipIfAlreadyProcessed(false), _loadOptimizedStrategy(false), _baselineIOMode(AutoReadMode),
			_threadCount(0)
			{
			}
			~ForEachMSAction()
			{
			}
			virtual std::string Description()
			{
				return "For each measurement set";
			}
			virtual void Initialize();
			virtual void Perform(ArtifactSet &artifacts, ProgressListener &progress);
			virtual ActionType Type() const { return ForEachMSActionType; }
			virtual unsigned int Weight() const { return ActionBlock::Weight() * _filenames.size(); }
			void AddDirectory(const std::string &name);
			void writeHistory(const std::string &filename);

			std::vector<std::string> &Filenames() { return _filenames; }
			const std::vector<std::string> &Filenames() const { return _filenames; }

			BaselineIOMode IOMode() const { return _baselineIOMode; }
			void SetIOMode(BaselineIOMode ioMode) { _baselineIOMode = ioMode; }

			bool ReadUVW() const { return _readUVW; }
			void SetReadUVW(bool readUVW) { _readUVW = readUVW; }

			const std::string &DataColumnName() const { return _dataColumnName; }
			void SetDataColumnName(const std::string &name) { _dataColumnName = name; }

			bool SubtractModel() const { return _subtractModel; }
			void SetSubtractModel(bool subtractModel) { _subtractModel = subtractModel; }

			std::string CommandLineForHistory() const { return _commandLineForHistory; }
			void SetCommandLineForHistory(const std::string cmd) { _commandLineForHistory = cmd; }
			
			bool SkipIfAlreadyProcessed() const { return _skipIfAlreadyProcessed; }
			void SetSkipIfAlreadyProcessed(bool value) { _skipIfAlreadyProcessed = value; }
			
			bool LoadOptimizedStrategy() const { return _loadOptimizedStrategy; }
			void SetLoadOptimizedStrategy(bool value) { _loadOptimizedStrategy = value; }
			
			size_t LoadStrategyThreadCount() const { return _threadCount; }
			void SetLoadStrategyThreadCount(size_t threadCount) { _threadCount = threadCount; }
			
			std::set<size_t>& Fields() { return _fields; }
			const std::set<size_t>& Fields() const { return _fields; }
			
			std::set<size_t>& Bands() { return _bands; }
			const std::set<size_t>& Bands() const { return _bands; }
		private:
			std::vector<std::string> _filenames;
			bool _readUVW;
			std::string _dataColumnName;
			bool _subtractModel;
			std::string _commandLineForHistory;
			bool _skipIfAlreadyProcessed;
			bool _loadOptimizedStrategy;
			BaselineIOMode _baselineIOMode;
			size_t _threadCount;
			std::set<size_t> _fields;
			std::set<size_t> _bands;
	};

}

#endif
