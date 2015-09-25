#include <casacore/tables/Tables/Table.h>

#include <casacore/measures/Measures/UVWMachine.h>
#include <casacore/measures/Measures/MEpoch.h>
#include <casacore/measures/Measures/MBaseline.h>
#include <casacore/measures/Measures/MCBaseline.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>

#include "msio/antennainfo.h"
#include "msio/measurementset.h"

#include "imaging/zenithimager.h"
#include "msio/pngfile.h"

#include <vector>

const casacore::Unit radUnit("rad");
const casacore::Unit dayUnit("d");
const casacore::Unit degUnit("deg");

void repoint(casacore::MDirection &phaseDirection, casacore::MPosition &position, const casacore::MEpoch &obstime, double &u, double &v, double &w, double &phaseRotation, casacore::MPosition &a1, casacore::MPosition &a2, bool verbose)
{
	//MPosition location(MVPosition(Quantity(1, "km"), Quantity(150, "deg"), Quantity(20, "deg")), MPosition::WGS84);
	
	//casacore::MEpoch obstime(casacore::Quantity(t, dayUnit), casacore::MEpoch::UTC);
	//if(verbose)
	//	std::cout << "Time=" << obstime.getValue() << '\n';
	casacore::MeasFrame timeAndLocation(obstime, position);
	
	casacore::MDirection::Ref refApparent(casacore::MDirection::AZEL, timeAndLocation);
	
	// Calculate zenith
	casacore::MDirection outDirection(
		casacore::Quantity(0.0, radUnit),       // Az
		casacore::Quantity(0.5*M_PI, radUnit),  // El
	refApparent);
	casacore::MDirection j2000Direction =
		casacore::MDirection::Convert(outDirection, casacore::MDirection::J2000)();
	if(verbose)
		std::cout << "Zenith=" << j2000Direction.getAngle().getValue(degUnit) << '\n';
	
	// Construct a CASA UVW converter
	casacore::UVWMachine uvwConverter(j2000Direction, phaseDirection);
	casacore::Vector<double> uvwVector(3);
	uvwVector[0] = u;
	uvwVector[1] = v;
	uvwVector[2] = w;
	//std::cout << "In: " << u << ',' << v << ',' << w << '\n';
	phaseRotation = uvwConverter.getPhase(uvwVector);
	// std::cout << "Phase shift: " << phaseRotation << '\n';
	//uvwConverter.convertUVW(uvwVector); // getPhase already does that!
	u = uvwVector[0];
	v = uvwVector[1];
	w = uvwVector[2];
	//std::cout << "Out: " << u << ',' << v << ',' << w << '\n';
	//std::cout << "Phase centre: " << uvwConverter.phaseCenter().getValue() << '\n';
}


void repoint(casacore::MDirection &phaseDirection, casacore::MDirection &newDirection, double &u, double &v, double &w, double &phaseRotation)
{
	// Construct a CASA UVW converter
	casacore::UVWMachine uvwConverter(newDirection, phaseDirection);
	casacore::Vector<double> uvwVector(3);
	uvwVector[0] = u;
	uvwVector[1] = v;
	uvwVector[2] = w;
	phaseRotation = uvwConverter.getPhase(uvwVector);
	u = uvwVector[0];
	v = uvwVector[1];
	w = uvwVector[2];
}


int main(int argc, char *argv[])
{
	std::string filename(argv[1]);
	size_t integrationSteps, resolution, startTimeIndex;
	bool useFlags;
	if(argc >= 3)
		integrationSteps = atoi(argv[2]);
	else
		integrationSteps = 60;
	if(argc >= 4)
		resolution = atoi(argv[3]);
	else
		resolution = 1024;
	if(argc >= 5)
		startTimeIndex = atoi(argv[4]);
	else
		startTimeIndex = 0;
	if(argc >= 6)
		useFlags = atoi(argv[4])==1;
	else
		useFlags = true;
	
	std::cout << "Opening " << filename << "...\n";
	
	const unsigned polarizationCount = MeasurementSet::PolarizationCount(filename);
	const BandInfo band = MeasurementSet::GetBandInfo(filename, 0);
	
	const unsigned antennaIndex = 0;
	
	casacore::MeasurementSet table(argv[1]);
	casacore::MEpoch::ROScalarColumn timeColumn(table, "TIME");
	casacore::ROArrayColumn<double> uvwColumn(table, "UVW");
	casacore::ROScalarColumn<int> ant1Column(table, "ANTENNA1");
	casacore::ROScalarColumn<int> ant2Column(table, "ANTENNA2");
	casacore::ROArrayColumn<casacore::Complex> dataColumn(table, "DATA");
	casacore::ROArrayColumn<bool> flagColumn(table, "FLAG");
	
	casacore::Table antennaTable(table.antenna());
	casacore::MPosition::ROScalarColumn antPositionColumn(antennaTable, "POSITION");
	casacore::ROScalarColumn<casacore::String> antNameColumn(antennaTable, "NAME");
	std::vector<casacore::MPosition> antennaPositions(antennaTable.nrow());
	for(unsigned i = 0;i<antennaTable.nrow();++i)
	{
		antennaPositions[i] = antPositionColumn(i);
	}
	casacore::MPosition position = antennaPositions[0];
	std::cout << "Frame of reference of antennae: " << position.getRefString() << '\n';
	std::cout << "Imaging zenith of antenna " << antNameColumn(antennaIndex)
		<< ", pos=" << position.getValue() << '\n';
		
	casacore::Table fieldTable(table.field());
	casacore::MDirection::ROArrayColumn phaseDirColumn(fieldTable, "PHASE_DIR");
	casacore::MDirection phaseDirection = *phaseDirColumn(0).begin();
	std::cout << "Phase direction: " << phaseDirection.getAngle().getValue(degUnit) << '\n';

	std::complex<float> *samples[polarizationCount];
	bool *isRFI[polarizationCount];
	for(unsigned p = 0; p < polarizationCount; ++p)
	{
		isRFI[p] = new bool[band.channels.size()];
		samples[p] = new std::complex<float>[band.channels.size()];
	}

	unsigned row = 0;
	ZenithImager imager;
	imager.Initialize(resolution);
	size_t timeStep = 0;
	bool directionIsSet = false;
	
	casacore::MEpoch curT = timeColumn(0);
	while(row<table.nrow() && timeStep < startTimeIndex)
	{
		if(timeColumn(row).getValue() != curT.getValue())
		{
			++timeStep;
			curT = timeColumn(row);
		}
		++row;
	}
	
	while(row<table.nrow())
	{
		const casacore::MEpoch t = timeColumn(row);
		casacore::MDirection j2000Direction;
		
		if(!directionIsSet)
		{
			// Calculate zenith for this time range
			casacore::MeasFrame timeAndLocation(t, position);
			casacore::MDirection::Ref refApparent(casacore::MDirection::AZEL, timeAndLocation);
			casacore::MDirection outDirection(
				casacore::Quantity(0.0, radUnit),       // Az
				casacore::Quantity(0.5*M_PI, radUnit),  // El
			refApparent);
			j2000Direction =
				casacore::MDirection::Convert(outDirection, casacore::MDirection::J2000)();
			std::cout << "Zenith=" << j2000Direction.getAngle().getValue(degUnit) << '\n';
			directionIsSet = true;
		}
		
		do
		{
			int a1 = ant1Column(row), a2 = ant2Column(row);
			if(a1 != a2)
			{
				casacore::Array<double> uvw = uvwColumn(row);
				casacore::Array<double>::const_iterator uvw_i = uvw.begin();
				double u = *uvw_i; ++uvw_i;
				double v = *uvw_i; ++uvw_i;
				double w = *uvw_i;
				double phaseRotation;
				repoint(phaseDirection, j2000Direction, u, v, w, phaseRotation);
				
				const casacore::Array<casacore::Complex> dataArray = dataColumn(row);
				const casacore::Array<bool> flagArray = flagColumn(row);
				
				casacore::Array<casacore::Complex>::const_iterator dataIter = dataArray.begin();
				casacore::Array<bool>::const_iterator flagIter = flagArray.begin();
				
				for(unsigned channel = 0; channel<band.channels.size(); ++channel)
				{
					for(unsigned p = 0; p < polarizationCount; ++p)
					{
						samples[p][channel] = *dataIter;
						if(useFlags)
							isRFI[p][channel] = *flagIter;
						else
							isRFI[p][channel] = false;
						
						++dataIter;
						++flagIter;
					}
				}
				imager.Add(band, samples[0], isRFI[0], u, v, w, phaseRotation);
			}
			++row;
		} while(row<table.nrow() && timeColumn(row).getValue() == t.getValue());
		
		timeStep++;
		if(timeStep % integrationSteps == 0)
		{
			Image2DPtr real, imaginary;
			imager.FourierTransform(real, imaginary);
			
			std::stringstream s;
			if(timeStep < 10000) s << '0';
			if(timeStep < 1000) s << '0';
			if(timeStep < 100) s << '0';
			if(timeStep < 10) s << '0';
			s << timeStep << ".png";
			BlackRedMap map;
			std::cout << "Saving " << s.str() << "... " << std::flush;
			PngFile::Save(*real, std::string("zen/") + s.str(), map);
			PngFile::Save(*imager.UVReal(), std::string("zen-uv/") + s.str(), map);
			imager.Clear();
			std::cout << "Done.\n";
			directionIsSet = false;
		}
	}
}
