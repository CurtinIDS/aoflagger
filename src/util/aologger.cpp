#include "aologger.h"

enum AOLogger::AOLoggerLevel AOLogger::_coutLevel = AOLogger::InfoLevel;

AOLogger::LogWriter<AOLogger::DebugLevel> AOLogger::Debug;

AOLogger::LogWriter<AOLogger::InfoLevel> AOLogger::Info;

AOLogger::LogWriter<AOLogger::WarningLevel> AOLogger::Warn;

AOLogger::LogWriter<AOLogger::ErrorLevel> AOLogger::Error;

AOLogger::LogWriter<AOLogger::FatalLevel> AOLogger::Fatal;

AOLogger::LogWriter<AOLogger::NoLevel, true> AOLogger::Progress;

void AOLogger::Init(const std::string &name, bool useLogger, bool verbose)
{
	Debug.SetUseLogger(useLogger && verbose);
	Info.SetUseLogger(useLogger);
	Warn.SetUseLogger(useLogger);
	Error.SetUseLogger(useLogger);
	Fatal.SetUseLogger(useLogger);
	Debug.SetUseLogger(useLogger && verbose);

	if(useLogger) {
		_coutLevel = ErrorLevel;
		//INIT_LOGGER(name);
	}
	else {
		if(verbose)
			_coutLevel = DebugLevel;
		else
			_coutLevel = InfoLevel;
	}
}
