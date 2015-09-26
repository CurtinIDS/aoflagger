#ifndef IMAGE_H
#define IMAGE_H

#include "../../structures/image2d.h"
#include "../../structures/timefrequencydata.h"

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
