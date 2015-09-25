/** @file
 * This is the header file for the FitsFile and FitsIOException class.
 * @author André Offringa <offringa@gmail.com>
 */
#ifndef FITSFILE_H
#define FITSFILE_H

#include "../baseexception.h"

#include <fitsio.h>
#include <string>
#include <exception>

#include <math.h>

#include "types.h"

/**
 * This class represents an exception that occurred during reading/writing of a FITS file.
 * @see FitsFile
 */
class FitsIOException : public BaseException {
	public:
		FitsIOException() : BaseException() { }
		FitsIOException(const std::string &description) : BaseException(description) { }
		~FitsIOException() throw() { }
};

/**
 * This class wraps the cfitsio library. It can be used to read or write a FITS file with
 * a (two dimensional) image. The fits file supports some other things, like tables, which
 * are not supported in this class. The FITS files generated by the MeqTrees process can
 * be read by this file. The FITS file can store multiple images in one file.
 *
 * This class works closely together with the Image2D class.
 * @see Image2D
 */
class FitsFile {
	public:
		/**
		 * Specifies how a FITS file should be opened
		 */
		enum FileMode {
			/**
			 * Only open for reading
			 */
			ReadOnlyMode,
	 		/**
			 * Open for reading and writing
			 */
	 		ReadWriteMode
		};
		
		/**
		 * The HDU is an entity inside a FITS file. This enum defines the possible entity types.
		 */
		enum HDUType {
			/**
			 * One or more two dimensional images
			 */
			ImageHDUType,
	 		/**
			 * An ASCII table
			 */
			ASCIITableHDUType,
			/**
			 * A binary table
			 */
			BinaryTableHDUType
		};
		
		/**
		 * This enum defines the possible image types in an image HDU.
		 */
		enum ImageType {
			/**
			 * Image consisting of 8 bit integers.
			 */
			Int8ImageType,
			/**
			 * Image consisting of 16 bit integers.
			 */
			Int16ImageType,
			/**
			 * Image consisting of 32 bit integers.
			 */
			Int32ImageType,
			/**
			 * Image consisting of 32 bit floats.
			 */
			Float32ImageType,
			/**
			 * Image consisting of 64 bit doubles.
			 */
			Double64ImageType
		};
		
		/**
		 * Construct a new FitsFile class associated with the specified filename.
		 * @param filename The file name of the fits file, to be opened with Open() or
		 * created with Create().
		 */
		FitsFile(const std::string &filename);
		
		/**
		 * Destructor.
		 * @throws FitsIOException in case something failed due to an IO error.
		 */
		~FitsFile();
		
		/**
		 * Open the file.
		 * @param mode In which way the file should be opened.
		 * @throws FitsIOException in case opening failed due to an IO error.
		 */
		void Open(FileMode mode = ReadOnlyMode);
		
		/**
		 * Create a new FITS file.
		 * @throws FitsIOException in case writing failed due to an IO error.
		 */
		void Create();
		
		/**
		 * Close the file, releasing resources.
		 * @throws FitsIOException in case something happent due to an IO error.
		 */
		void Close();
		
		/**
		 * Determine whether the file is ready for reading and/or writing.
		 * @return @c true if the file has been opened.
		 */
		bool IsOpen() const { return _isOpen; }
		
		/**
		 * Retrieve the number of HDU blocks inside the file.
		 * @return The number of HDU blocks inside the file.
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		int GetHDUCount();
		
		/**
		 * Retrieve the index of the current HDU block.
		 * @return The index of the current HDU block.
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		int GetCurrentHDU();
		
		/**
		 * Retrieve the type of the current HDU block.
		 * @return The type of the current HDU block.
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		enum HDUType GetCurrentHDUType();
		
		/**
		 * Start reading another HDU block, specified by its index.
		 * @param hduNumber Index of the HDU block to move to.
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		void MoveToHDU(int hduNumber);
		
		/**
		 * Retrieve what kind of image this HDU image block is. Only call this
		 * method if GetCurrentHDUType() returned ImageHDUType.
		 * @return The image type.
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		FitsFile::ImageType GetCurrentImageType();
		
		/**
		 * Retrieve the number of dimensions of an image. This is often more than
		 * 2, although the FITS file actually contains (several) 2D images.
		 * @return The number of dimensions.
		 * @see GetCurrentImageSize()
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		int GetCurrentImageDimensionCount();
		
		/**
		 * Retrieve the size of a specific dimension.
		 * @param dimension The dimension to retrieve the size for (first dimension = 0)
		 * @return The size of the dimension.
		 * @see GetCurrentImageDimensionCount()
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		long GetCurrentImageSize(int dimension);

		/**
		 * Writes a new image HUD to the FITS file. Does not write the data itself, only the headers. The WriteImage()
		 * call should be called next.
		 * @param imageType Type of image
		 * @param width Width of image
		 * @param height Height of image
		 * @throws FitsIOException in case writing failed due to an IO error.
		 */
		void AppendImageHUD(enum FitsFile::ImageType imageType, long width, long height);

		/**
		 * Reads one image into a buffer. The image will be converted to @c long @c doubles.
		 * @param startPos This specifies where to start reading. If several images are stored in the 3rd or 4th dimension,
		 * they can be read one by one by starting at different start positions.
		 * @param buffer The buffer where the image will be stored.
		 * @param bufferSize Size of the buffer. Reading will stop once the buffer is full. It makes sense to use buffer the size
		 * of the 1st x 2nd dimension.
		 * @param nullValue What value should be used to represent null values.
		 * @throws FitsIOException in case reading failed due to an IO error.
		 */
		void ReadCurrentImageData(long startPos, num_t *buffer, long bufferSize, long double nullValue = nan("Unset value"));
		
		/**
		 * Writes an image to the FITS file.
		 * @param startPos Where inside the image to start writing.
		 * @param buffer Buffer containing the image data
		 * @param bufferSize Size of the buffer
		 * @param nullValue What value was used to represent null values.
		 * @see ReadCurrentImageData
		 * @throws FitsIOException in case writing failed due to an IO error.
		 */
		void WriteImage(long startPos, double *buffer, long bufferSize, double nullValue = nan("Unset value"));
		void WriteImage(long startPos, float *buffer, long bufferSize, float nullValue = nan("Unset value"));
		
		int GetKeywordCount();
		bool HasKeyword(const std::string &keywordName);
		std::string GetKeyword(int keywordNumber);
		std::string GetKeywordValue(int keywordNumber);
		std::string GetKeywordValue(const std::string &keywordName);
		bool GetKeywordValue(const std::string &keywordName, std::string &value);
		std::string GetKeywordComment(int keywordNumber);
		int GetRowCount();
		int GetColumnCount();
		int GetColumnType(int colNumber);
		bool HasGroups();
		int GetIntKeywordValue(int keywordNumber);
		int GetIntKeywordValue(const std::string &keywordName);
		double GetDoubleKeywordValue(int keywordNumber);
		double GetDoubleKeywordValue(const std::string &keywordName);
		int GetGroupCount();
		int GetParameterCount();
		long GetImageSize();
		long GetGroupSize();
		void ReadGroup(long groupIndex, long double *groupData);
		void ReadGroupData(long groupIndex, long double *groupData);
		void ReadGroupParameters(long groupIndex, long double *parametersData);
		void ReadTableCell(int row, int col, long double *output, size_t size);
		void ReadTableCell(int row, int col, double *output, size_t size);
		void ReadTableCell(int row, int col, bool *output, size_t size);
		void ReadTableCell(int row, int col, char *output);
		void WriteTableCell(int row, int col, double *data, size_t size);
		void WriteTableCell(int row, int col, const bool *data, size_t size);
		bool HasTableColumn(const std::string &columnName, int columnIndex);
		int GetTableColumnIndex(const std::string &columnName);
		int GetTableColumnArraySize(int columnIndex);
		long GetTableDimensionSize(int columnIndex, int dimension);
		int GetGroupParameterIndex(const std::string &parameterName);
		int GetGroupParameterIndex(const std::string &parameterName, int number);
		bool HasGroupParameter(const std::string &parameterName);
		bool HasGroupParameter(const std::string &parameterName, int number);
		const std::string &Filename() const { return _filename; }
	private:
		const std::string _filename;
		fitsfile *_fptr;
		bool _isOpen;
		inline void CheckStatus(int status) const;
		inline void CheckOpen() const;
};
#endif
