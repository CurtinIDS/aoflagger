#ifndef MSIOSYSTEM_H
#define MSIOSYSTEM_H

#include <casacore/casa/OS/HostInfo.h>

class System
{
	public:
		static long TotalMemory()
		{
			return casacore::HostInfo::memoryTotal()*1024;
		}
		
		static unsigned ProcessorCount()
		{
			unsigned cpus = casacore::HostInfo::numCPUs();
			if(cpus == 0) cpus = 1;
			return cpus;
		}
};

#endif //MSIOSYSTEM_H
