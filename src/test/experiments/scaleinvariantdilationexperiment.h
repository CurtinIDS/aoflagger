#ifndef AOFLAGGER_SCALEINVARIANTDILATIONEXPERIMENT_H
#define AOFLAGGER_SCALEINVARIANTDILATIONEXPERIMENT_H

#include <fstream>

#include <AOFlagger/test/testingtools/asserter.h>
#include <AOFlagger/test/testingtools/unittest.h>

#include <AOFlagger/strategy/algorithms/siroperator.h>
#include <AOFlagger/strategy/algorithms/statisticalflagger.h>

#include <AOFlagger/util/rng.h>

class ScaleInvariantDilationExperiment : public UnitTest {
	public:
		ScaleInvariantDilationExperiment() : UnitTest("Scale invariant dilation experiments")
		{
			AddTest(TestTimingN(), "Timing O(N) algorithm");
			AddTest(TestTimingNlogN(), "Timing O(N x log N) algorithm");
			AddTest(TestTimingNsq(), "Timing O(N^2) algorithm");
		}
		
	private:
		static const unsigned _repeatCount;
		
		struct TestTimingN : public Asserter
		{
			void operator()();
		};
		struct TestTimingNlogN : public Asserter
		{
			void operator()();
		};
		struct TestTimingNsq : public Asserter
		{
			void operator()();
		};
};

const unsigned ScaleInvariantDilationExperiment::_repeatCount = 100;

inline void ScaleInvariantDilationExperiment::TestTimingN::operator()()
{
	const double maxX = 7;
	bool *prototypeFlags = new bool[(unsigned) round(exp10(maxX))];
	for(unsigned i=0;i<(unsigned) round(exp10(maxX));++i)
	{
		prototypeFlags[i] = RNG::Uniform() > 0.9;
	}
	for(unsigned e=0;e<6;++e)
	{
		std::stringstream s;
		s << "scale-invariant-dilation-timing" << e << ".txt";
		std::ofstream file(s.str().c_str());
		const double eta = e * 0.2;
		
		for(double x=3.0;x<=maxX;x+=0.05)
		{
			const unsigned n = (unsigned) round(exp10(x));
			bool *flags = new bool[n];
			Stopwatch watch(true);
			for(unsigned repeat=0;repeat<_repeatCount;++repeat)
			{
				for(unsigned i=0;i<n;++i) flags[i] = prototypeFlags[i];
				SIROperator::Operate(flags, n, eta);
			}
			file << n << '\t' << (watch.Seconds()/(double) _repeatCount) << '\t' << x << std::endl;
			delete[] flags;
		}
	}
	delete[] prototypeFlags;
}

inline void ScaleInvariantDilationExperiment::TestTimingNlogN::operator()()
{
	const double maxX = 6.5;
	bool *prototypeFlags = new bool[(unsigned) round(exp10(maxX))];
	for(unsigned i=0;i<(unsigned) round(exp10(maxX));++i)
	{
		prototypeFlags[i] = RNG::Uniform() > 0.9;
	}
	std::ofstream file("scale-invariant-dilation-timing-nlogn.txt");
	const double eta = 0.2;
	
	for(double x=0.0;x<=maxX;x+=0.05)
	{
		const unsigned n = (unsigned) round(exp10(x));
		bool *flags = new bool[n];
		Stopwatch watch(true);
		for(unsigned repeat=0;repeat<_repeatCount;++repeat)
		{
			for(unsigned i=0;i<n;++i) flags[i] = prototypeFlags[i];
			StatisticalFlagger::ScaleInvDilationQuick(flags, n, eta);
		}
		file << n << '\t' << (watch.Seconds()/(double) _repeatCount) << '\t' << x << std::endl;
		delete[] flags;
	}
	delete[] prototypeFlags;
}

inline void ScaleInvariantDilationExperiment::TestTimingNsq::operator()()
{
	const double maxX = 5;
	bool *prototypeFlags = new bool[(unsigned) round(exp10(maxX))];
	for(unsigned i=0;i<(unsigned) round(exp10(maxX));++i)
	{
		prototypeFlags[i] = RNG::Uniform() > 0.9;
	}
	std::ofstream file("scale-invariant-dilation-timing-nsq.txt");
	const double eta = 0.2;
	
	for(double x=0.0;x<=maxX;x+=0.05)
	{
		const unsigned n = (unsigned) round(exp10(x));
		bool *flags = new bool[n];
		Stopwatch watch(true);
		for(unsigned repeat=0;repeat<_repeatCount;++repeat)
		{
			for(unsigned i=0;i<n;++i) flags[i] = prototypeFlags[i];
			StatisticalFlagger::ScaleInvDilationFull(flags, n, eta);
		}
		file << n << '\t' << (watch.Seconds()/(double) _repeatCount) << '\t' << x << std::endl;
		delete[] flags;
	}
	delete[] prototypeFlags;
}

#endif
