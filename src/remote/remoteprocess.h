#ifndef AOREMOTE__REMOTE_PROCESS_H
#define AOREMOTE__REMOTE_PROCESS_H

#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>

#include <sstream>
#include <iostream>
#include <stdexcept>

#include <boost/function.hpp>

#include <boost/thread/thread.hpp>

#include "clusteredobservation.h"

namespace aoRemote
{

class RemoteProcess
{
	public:
		RemoteProcess(const std::string &clientHostName, const std::string &serverHostName)
		: _clientHostName(clientHostName), _serverHostName(serverHostName),  _running(false)
		{
		}
		
		~RemoteProcess()
		{
			Join();
		}
		
		void Join()
		{
			if(_running)
			{
				_thread->join();
				delete _thread;
				_running = false;
			}
		}
		
		void Start()
		{
			_running = true;
			_thread = new boost::thread(ThreadFunctor(*this));
		}
		
		boost::function<void(RemoteProcess &/*process*/, bool /*error*/, int /*status*/)> &SignalFinished()
		{
			return _onFinished;
		}
		
		const std::string &ClientHostname() const
		{
			return _clientHostName;
		}
	private:
		RemoteProcess(const RemoteProcess &source) { }
		void operator=(const RemoteProcess &source) { }
		
		struct ThreadFunctor
		{
			ThreadFunctor(RemoteProcess &process) : _remoteProcess(process) { }
			RemoteProcess &_remoteProcess;
			void operator()()
			{
				std::ostringstream commandLine;
				commandLine
					<< "ssh " << _remoteProcess._clientHostName << " -x \"bash --login -c \\\"aoremoteclient connect "
					<< _remoteProcess._serverHostName << "\\\" \"";
				std::cout << commandLine.str() << std::endl;
				int pid = vfork();
				switch (pid) {
					case -1: // Error
						throw std::runtime_error("Could not vfork() new process for executing remote client");
					case 0: // Child
						execl("/bin/sh", "sh", "-c", commandLine.str().c_str(), NULL);
						_exit(127);
				}
				
				// Wait for process to terminate
				int pStatus;
				do {
					int pidReturn;
					do {
						pidReturn = waitpid(pid, &pStatus, 0);
					} while (pidReturn == -1 && errno == EINTR);
				} while(!WIFEXITED(pStatus) && !WIFSIGNALED(pStatus));
				if(WIFEXITED(pStatus))
				{
					const int exitStatus = WEXITSTATUS(pStatus);
					_remoteProcess._onFinished(_remoteProcess, exitStatus!=0, exitStatus);
				} else {
					_remoteProcess._onFinished(_remoteProcess, true, 0);
				}
			}
		};
		
		const ClusteredObservationItem _item;
		const std::string _clientHostName, _serverHostName;
		boost::thread *_thread;
		bool _running;
		
		boost::function<void(RemoteProcess &/*process*/, bool /*error*/, int /*status*/)> _onFinished;
};

}

#endif
