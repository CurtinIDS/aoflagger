#ifndef STOKESIMAGER_H
#define STOKESIMAGER_H

#include "image2d.h"

class StokesImager{
	public:
		static Image2DPtr CreateSum(Image2DCPtr left, Image2DCPtr right);
		static Image2DPtr CreateDifference(Image2DCPtr left, Image2DCPtr right);
		static Image2DPtr CreateNegatedSum(Image2DCPtr left, Image2DCPtr right);

		static Image2DPtr CreateStokesIAmplitude(Image2DCPtr realXX, Image2DCPtr imaginaryXX, Image2DCPtr realYY, Image2DCPtr imaginaryYY);
		static Image2DPtr CreateStokesI(Image2DCPtr xx, Image2DCPtr yy) { return CreateSum(xx, yy); }
		static Image2DPtr CreateStokesQ(Image2DCPtr xx, Image2DCPtr yy) { return CreateDifference(xx, yy); }

		static Image2DPtr CreateAvgPhase(Image2DCPtr xx, Image2DCPtr yy);
	private:
		StokesImager() { }
		~StokesImager() { }
};

#endif
