#include "parmimageset.h"

#include <set>

#include "../../msio/parmtable.h"

namespace rfiStrategy
{
	ParmImageSet::~ParmImageSet()
	{
		if(_parmTable != 0)
			delete _parmTable;
	}
			
	void ParmImageSet::Initialize()
	{
		_parmTable = new ParmTable(_path);
		const std::set<std::string> antennaSet = _parmTable->GetAntennas();
		for(std::set<std::string>::const_iterator i=antennaSet.begin();i!=antennaSet.end();++i)
			_antennas.push_back(*i);
	}
	
	TimeFrequencyData *ParmImageSet::LoadData(const ImageSetIndex &index)
	{
		const ParmImageSetIndex parmIndex = static_cast<const ParmImageSetIndex &>(index);
		const std::string antenna = _antennas[parmIndex.AntennaIndex()];
		return new TimeFrequencyData(_parmTable->Read(antenna));
	}
}

