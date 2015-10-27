#include "clusteredobservation.h"

#include <stdexcept>

#include <memory> // for auto_ptr

#include "vdsfile.h"

#include "../ref/reffile.h"

namespace aoRemote
{

ClusteredObservation::ClusteredObservation()
{
}

ClusteredObservation *ClusteredObservation::LoadFromVds(const std::string &vdsFilename)
{
	VdsFile vdsFile(vdsFilename);
	std::unique_ptr<ClusteredObservation> cObs(new ClusteredObservation());
	const size_t nParts = vdsFile.NParts();
	for(size_t i=0;i!=nParts;++i)
	{
		cObs->AddItem(ClusteredObservationItem(cObs->Size(), vdsFile.Filename(i), vdsFile.Host(i)));
	}
	return cObs.release();
	
	
	/*LOFAR::CEP::VdsDesc vdsDesc(vdsFilename);
	const std::vector<LOFAR::CEP::VdsPartDesc> &parts = vdsDesc.getParts();
	
	std::auto_ptr<ClusteredObservation> cObs(new ClusteredObservation());
	
	for(std::vector<LOFAR::CEP::VdsPartDesc>::const_iterator i=parts.begin();i!=parts.end();++i)
	{
		const std::string &filename = i->getFileName();
		const std::string &filesystem = i->getFileSys();
		
		size_t separatorPos = filesystem.find(':');
		if(separatorPos == std::string::npos || separatorPos == 0)
			throw std::runtime_error("One of the file system descriptors in the VDS file has an unexpected format");
		const std::string hostname = filesystem.substr(0, separatorPos);
		
		ClusteredObservationItem newItem(cObs->Size(), filename, hostname);
		cObs->AddItem(newItem);
	}
	
	return cObs.release();*/
}

ClusteredObservation *ClusteredObservation::LoadFromRef(const std::string &refFilename)
{
	AOTools::RefFile refFile(refFilename);
	std::unique_ptr<ClusteredObservation> cObs(new ClusteredObservation());
	for(AOTools::RefFile::const_iterator i=refFile.begin();i!=refFile.end();++i)
	{
		const AOTools::RefFileEntry &entry = *i;
		cObs->AddItem(ClusteredObservationItem(cObs->Size(), entry.Path(), entry.Node()));
	}
	return cObs.release();
}

ClusteredObservation *ClusteredObservation::Load(const std::string &filename)
{
	if(IsVdsFilename(filename))
		return LoadFromVds(filename);
	else if(IsRefFilename(filename))
		return LoadFromRef(filename);
	else
		throw std::runtime_error("Could not determine type of specified filename for loading as clustered observation");
}

bool ClusteredObservation::IsVdsFilename(const std::string &filename)
{
	return
		(filename.size() > 4 && filename.substr(filename.size()-4) == ".vds")
		||
		(filename.size() > 4 && filename.substr(filename.size()-4) == ".gds")
		||
		(filename.size() > 5 && filename.substr(filename.size()-5) == ".gvds");
}

bool ClusteredObservation::IsRefFilename(const std::string &filename)
{
	return
		(filename.size() > 4 && filename.substr(filename.size()-4) == ".ref");
}

}
