#include <iostream>

#include <libgen.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/ms/MeasurementSets/MSTable.h>
#include <casacore/ms/MeasurementSets/MSColumns.h>

#include "structures/date.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <ms1>\n"
		"This program will list the RFI actions that have been performed on the measurement \n"
		"set, by reading the HISTORY table in the set.\n";
	}
	else
	{
		std::ostream &stream(std::cout);

		casacore::MeasurementSet ms(argv[1]);
		casacore::Table histtab(ms.history());
		casacore::ROScalarColumn<double>       time        (histtab, "TIME");
		casacore::ROScalarColumn<casacore::String> application (histtab, "APPLICATION");
		casacore::ROArrayColumn<casacore::String>  cli         (histtab, "CLI_COMMAND");
		casacore::ROArrayColumn<casacore::String>  parms       (histtab, "APP_PARAMS");
		for(unsigned i=0;i<histtab.nrow();++i)
		{
			if(application(i) == "AOFlagger")
			{
				stream << "====================\n"
					"Command: " << cli(i)[0] << "\n"
					"Date: " << Date::AipsMJDToDateString(time(i)) << "\n"
					"Time: " << Date::AipsMJDToTimeString(time(i)) << "\n"
					"Strategy: \n     ----------     \n";
				const casacore::Vector<casacore::String> appParamsVec = parms(i);
				for(casacore::Vector<casacore::String>::const_iterator j=appParamsVec.begin();j!=appParamsVec.end();++j)
				{
					stream << *j << '\n';
				}
				stream << "     ----------     \n";
			}
		}
	}
}
