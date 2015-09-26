#ifndef RNG_H
#define RNG_H

#include "../structures/types.h"

class RNG{
	public:
		static double Gaussian();
		static double GaussianProduct();
		static double GaussianPartialProduct();
		static double GaussianComplex();
		static double Rayleigh();
		static double Uniform();
		static double EvaluateRayleigh(double x, double sigmaSquared);
		static long double EvaluateGaussian(long double x, long double sigmaSquared);
		static double EvaluateUnnormalizedGaussian(double x, double sigmaSquared);
		static double EvaluateGaussian(double x, double sigmaSquared);
		static double EvaluateGaussian2D(long double x1, long double x2, long double sigmaX1, long double sigmaX2);
		static double IntegrateGaussian(long double upperLimit);
		static void DoubleGaussian(long double &a, long double &b);
		static void ComplexGaussianAmplitude(num_t &r, num_t &i);
	private: 
		RNG();
};

#endif
