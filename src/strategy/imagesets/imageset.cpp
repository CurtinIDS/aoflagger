/***************************************************************************
 *   Copyright (C) 2008 by A.R. Offringa   *
 *   offringa@astro.rug.nl   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "imageset.h"

#include "bhfitsimageset.h"
#include "filterbankset.h"
#include "fitsimageset.h"
#include "msimageset.h"
#include "parmimageset.h"
#include "pngreader.h"
#include "qualitystatimageset.h"

#include <boost/algorithm/string.hpp>

namespace rfiStrategy {
	ImageSet *ImageSet::Create(const std::string &file, BaselineIOMode ioMode, bool readUVW)
	{
		if(IsFitsFile(file))
			return new FitsImageSet(file);
		else if(IsBHFitsFile(file))
			return new BHFitsImageSet(file);
		else if(IsRCPRawFile(file))
			throw std::runtime_error("Don't know how to open RCP raw files");
		else if(IsTKPRawFile(file))
			throw std::runtime_error("Don't know how to open TKP raw files");
		else if(IsRawDescFile(file))
			throw std::runtime_error("Don't know how to open RCP desc files");
		else if(IsParmFile(file))
			return new ParmImageSet(file);
		else if(IsPngFile(file))
			return new PngReader(file);
		else if(IsFilterBankFile(file))
			return new FilterBankSet(file);
		else if(IsQualityStatSet(file))
			return new QualityStatImageSet(file);
		else {
			MSImageSet *set = new MSImageSet(file, ioMode);
			set->SetReadUVW(readUVW);
			return set;
		}
	}
	
	bool ImageSet::IsBHFitsFile(const std::string &file)
	{
		const std::string uppFile(boost::to_upper_copy(file));
		return
		  (uppFile.size() > 7 && uppFile.substr(file.size() - 7) == ".BHFITS" );
	}
	
	bool ImageSet::IsFitsFile(const std::string &file)
	{
		const std::string uppFile(boost::to_upper_copy(file));
		return
		(uppFile.size() > 4 && uppFile.substr(file.size()- 4) == ".UVF")
		||
		(uppFile.size() > 5 && uppFile.substr(file.size() - 5) == ".FITS" )
		||
		(uppFile.size() > 7 && uppFile.substr(file.size() - 7) == ".UVFITS" )
		||
		(uppFile.size() > 7 && uppFile.substr(file.size() - 7) == ".SDFITS" ); // Parkes raw files are named like this
	}
	
	bool ImageSet::IsRCPRawFile(const std::string &file)
	{
		return file.size() > 4 && file.substr(file.size()-4) == ".raw";
	}
	
	bool ImageSet::IsTKPRawFile(const std::string &file)
	{
		return file.size() > 4 && file.substr(file.size()-4) == ".1ch";
	}
	
	bool ImageSet::IsRawDescFile(const std::string &file)
	{
		return file.size() > 8 && file.substr(file.size()-8) == ".rawdesc";
	}
	
	bool ImageSet::IsParmFile(const std::string &file)
	{
		return file.size() >= 10 && file.substr(file.size()-10) == "instrument";
	}
	
	bool ImageSet::IsTimeFrequencyStatFile(const std::string &file)
	{
		return
		(file.size()>=24 && file.substr(file.size()-24) == "counts-timefreq-auto.txt")
		||
		(file.size()>=25 && file.substr(file.size()-25) == "counts-timefreq-cross.txt");
	}
	
	bool ImageSet::IsNoiseStatFile(const std::string &file)
	{
		return
		file.find("noise-statistics-tf") != std::string::npos &&
		file.find("txt") != std::string::npos;
	}
	
	bool ImageSet::IsHarishFile(const std::string &file)
	{
		return file.substr(file.size()-4) == ".har";
	}
	
	bool ImageSet::IsPngFile(const std::string &file)
	{
		return file.size()>=4 && file.substr(file.size()-4) == ".png";
	}
	
	bool ImageSet::IsFilterBankFile(const std::string& file)
	{
		return file.size()>=4 && file.substr(file.size()-4) == ".fil";
	}
	
	bool ImageSet::IsQualityStatSet(const string& file)
	{
		if(file.empty()) return false;
		std::string copy(file);
		if(*copy.rbegin() == '/')
			copy.resize(copy.size()-1);
		boost::filesystem::path p(copy);
		return p.filename() == "QUALITY_TIME_STATISTIC";
	}
	
	bool ImageSet::IsMSFile(const std::string &file)
	{
	  return (!IsBHFitsFile(file)) && (!IsFitsFile(file)) && (!IsRCPRawFile(file)) && (!IsTKPRawFile(file)) && (!IsRawDescFile(file)) && (!IsParmFile(file)) && (!IsTimeFrequencyStatFile(file)) && (!IsNoiseStatFile(file)) && (!IsPngFile(file)) && (!IsFilterBankFile(file)) && (!IsQualityStatSet(file));
	}
}
