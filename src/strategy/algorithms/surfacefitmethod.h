#ifndef IMAGE_H
#define IMAGE_H

#include "../../msio/image2d.h"
#include "../../msio/timefrequencydata.h"

#include "../../util/ffttools.h"

class SurfaceFitMethod {
	public:
		virtual void Initialize(const TimeFrequencyData &input) = 0;
		virtual unsigned TaskCount() = 0;
		virtual void PerformFit(unsigned taskNumber) = 0;
		virtual ~SurfaceFitMethod() { }
		virtual TimeFrequencyData Background() = 0;
		virtual enum TimeFrequencyData::PhaseRepresentation PhaseRepresentation() const = 0;
};

#endif
