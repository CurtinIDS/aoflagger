#ifndef SVDMITIGATER_H
#define SVDMITIGATER_H

#include <iostream>

#include "../../structures/image2d.h"

#include "surfacefitmethod.h"

// Needs to be included LAST
#include "../../f2c.h"

class SVDMitigater : public SurfaceFitMethod {
	public:
		SVDMitigater();
		~SVDMitigater();

		virtual void Initialize(const TimeFrequencyData &data) {
			Clear();
			_data = data;
			_iteration = 0;
		}
		virtual unsigned TaskCount() { return 1; }
		virtual void PerformFit(unsigned)
		{
			_iteration++;
			RemoveSingularValues(_removeCount);
		}

		virtual void RemoveSingularValues(unsigned singularValueCount)
		{
			if(!IsDecomposed())
				Decompose();
			for(unsigned i=0;i<singularValueCount;++i)
				SetSingularValue(i, 0.0);
			Compose();
		}

		virtual TimeFrequencyData Background()
		{
			return *_background;
		}

		virtual enum TimeFrequencyData::PhaseRepresentation PhaseRepresentation() const
		{
			return TimeFrequencyData::ComplexRepresentation;
		}

		bool IsDecomposed() const throw() { return _singularValues != 0 ; }
		double SingularValue(unsigned index) const throw() { return _singularValues[index]; }
		void SetRemoveCount(unsigned removeCount) throw() { _removeCount = removeCount; }
		void SetVerbose(bool verbose) throw() { _verbose = verbose; }
		static void CreateSingularValueGraph(const TimeFrequencyData &data, class Plot2D &plot);
	private:
		void Clear();
		void Decompose();
		void Compose();
		void SetSingularValue(unsigned index, double newValue) throw() { _singularValues[index] = newValue; }

		TimeFrequencyData _data;
		TimeFrequencyData *_background;
		double *_singularValues;
		doublecomplex *_leftSingularVectors;
		doublecomplex *_rightSingularVectors;
		long int _m, _n;
		unsigned _iteration;
		unsigned _removeCount;
		bool _verbose;
};

#endif
