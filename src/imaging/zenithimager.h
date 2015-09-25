#ifndef ZENITH_IMAGER_H
#define ZENITH_IMAGER_H

#include <complex>

#include "../msio/image2d.h"

class ZenithImager
{
	public:
		ZenithImager();
		
		void Initialize(size_t resolution);
		
		void Clear();
		
		void Add(const class BandInfo &band, const std::complex<float> *samples, const bool *isRFI, double u, double v, double w, double phaseRotation);
		
		void FourierTransform(Image2DPtr &real, Image2DPtr &imaginary);
		
		Image2DCPtr UVReal() const { return _real; }
		Image2DCPtr UVImaginary() const { return _imaginary; }
	private:
		void add(const class BandInfo &band, double r, double i, double u, double v, double w, double phaseRotation, double wavelength);
		
		static long double Wavelength(long double frequency)
		{
			return 299792458.0L / frequency;
		}
		
		size_t _resolution, _totalCount, _outsideCount;
		
		Image2DPtr _real, _imaginary, _weights;
};

#endif
