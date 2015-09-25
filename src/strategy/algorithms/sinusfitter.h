#ifndef SINUSFITTER_H
#define SINUSFITTER_H

#include <cstring>
#include <cmath>

#include "../../msio/types.h"

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
class SinusFitter {
	public:
		SinusFitter();
		~SinusFitter();
		void FindPhaseAndAmplitude(num_t &phase, num_t &amplitude, const num_t *dataX, const num_t *dataT, const size_t dataSize, const num_t frequency) const throw();
		void FindPhaseAndAmplitudeComplex(num_t &phase, num_t &amplitude, const num_t *dataR, const num_t *dataI, const num_t *dataT, const size_t dataSize, const num_t frequency) const throw();


		num_t FindMean(const num_t phase, const num_t amplitude, const num_t *dataX, const num_t *dataT, const size_t dataSize, const num_t frequency);

		static num_t Value(const num_t phase, const num_t amplitude, const num_t t, const num_t frequency, num_t mean)
		{
			return cosn(phase + t * frequency) * amplitude + mean;
		}

		/*template<typename T> static T Phase(T real, T imaginary)
		{
			if(real==0.0L)
			{
				if(imaginary==0.0L)
					return 0.0L;
				else if(imaginary > 0.0L)
					return M_PIn*0.5;
				else
					return -M_PIn*0.5;
			}
			else if(real>0.0L)
			{
				if(imaginary>=0.0L) // first 
					return atannl(imaginary/real);
				else // fourth
					return atannl(imaginary/real)+2.0*M_PIn;
			}
			else
			{
				if(imaginary>=0.0L) // second
					return atannl(imaginary/real) + 1.0*M_PIn;
				else // third
					return atannl(imaginary/real) + 1.0*M_PIn;
			}
		}*/
		static num_t Phase(num_t real, num_t imaginary)
		{
			return atan2n(imaginary, real);
		}
		static numl_t Phase(numl_t real, numl_t imaginary)
		{
			return atan2nl(imaginary, real);
		}
	private:
		
};

#endif
