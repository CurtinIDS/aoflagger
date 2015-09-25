#ifndef COMPRESS_H
#define COMPRESS_H

#include <fstream>

#include "../msio/timefrequencydata.h"

class Compress
{
	public:
		Compress(const TimeFrequencyData &data) : _data(data), _isInitialized(false)
		{
		}
		~Compress()
		{
			Deinitialize();
		}
		void Initialize();
		void Deinitialize();
		
		unsigned long RawSize();
		unsigned long FlacSize();
		unsigned long ZipSize();
		unsigned long GzSize();
		unsigned long Bz2Size();
		void AllToStdOut()
		{
			RawSize();
			FlacSize();
			ZipSize();
			GzSize();
			Bz2Size();
		}
	private:
		void ExecuteCmd(const std::string &str);

		const TimeFrequencyData _data;
		bool _isInitialized;

		unsigned long Size(const std::string &file);
		void Write(std::ofstream &stream, Image2DCPtr image, Mask2DCPtr mask);
		void WriteSubtractFrequencies(std::ofstream &stream, Image2DCPtr image, Mask2DCPtr mask);
		Image2DPtr Read(std::ifstream &stream, Image2DPtr image, Mask2DCPtr mask);
};

#endif // COMPRESS_H

