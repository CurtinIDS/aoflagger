#ifndef AOREMOTE__CLUSTERED_OBSERVATION_H
#define AOREMOTE__CLUSTERED_OBSERVATION_H

#include <string>
#include <vector>
#include <map>

#include "hostname.h"

namespace aoRemote
{

class ClusteredObservationItem
{
	public:
		ClusteredObservationItem() :
			_index(0), _localPath(), _hostName()
		{ }
		ClusteredObservationItem(unsigned index, const std::string &localPath, const Hostname &hostName) :
			_index(index), _localPath(localPath), _hostName(hostName)
		{ }
		ClusteredObservationItem(const ClusteredObservationItem &source) :
			_index(source._index), _localPath(source._localPath), _hostName(source._hostName)
		{ }
		ClusteredObservationItem &operator=(const ClusteredObservationItem &source)
		{
			_index = source._index;
			_localPath = source._localPath;
			_hostName = source._hostName;
			return *this;
		}
		bool operator<(const ClusteredObservationItem &rhs) const
		{
			return _index < rhs._index;
		}
		unsigned Index() const { return _index; }
		const std::string &LocalPath() const { return _localPath; }
		const Hostname &HostName() const { return _hostName; }
	private:
		unsigned _index;
		std::string _localPath;
		Hostname _hostName;
};

class ClusteredObservation
{
	public:
		ClusteredObservation();
		
		static bool IsClusteredFilename(const std::string &filename)
		{
			return IsVdsFilename(filename) || IsRefFilename(filename);
		}
		
		static bool IsVdsFilename(const std::string &filename);
		
		static bool IsRefFilename(const std::string &filename);
		
		static ClusteredObservation *Load(const std::string &filename);
		
		static ClusteredObservation *LoadFromVds(const std::string &vdsFilename);
		
		static ClusteredObservation *LoadFromRef(const std::string &refFilename);
		
		void AddItem(const ClusteredObservationItem &item)
		{
			_items.push_back(item);
		}
		
		const std::vector<ClusteredObservationItem> &GetItems() const
		{
			return _items;
		}
		
		size_t Size() const { return _items.size(); }
		
	private:
		// disable copying
		ClusteredObservation(const ClusteredObservation &source)
		{
		}
		// disable assignment
		void operator=(const ClusteredObservation &source)
		{
		}
		
		std::vector<ClusteredObservationItem> _items;
};


}

#endif
