#ifndef ARRAYCOLUMNITERATOR_H
#define ARRAYCOLUMNITERATOR_H

//#define AIPS_NO_TEMPLATE_SRC
//#define CASACORE_NO_AUTO_TEMPLATES
#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/tables/Tables/RefRows.h>

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
template<typename T>
class ROArrayColumnIterator {
	public:
		ROArrayColumnIterator(class casacore::ROArrayColumn<T> &column, unsigned row) throw() :
			_column(column), _row(row)
		{
		}
		ROArrayColumnIterator(const ROArrayColumnIterator<T> &source) throw() :
			_column(source._column), _row(source._row)
		{
		}
		~ROArrayColumnIterator() throw()
		{
		}
		ROArrayColumnIterator<T> &operator++() throw() {
			_row++;
			return *this;
		}
		casacore::Array<T> *operator->() const throw() {
			return &_column(_row);
		}
		casacore::Array<T> operator*() const throw() {
			return _column(_row);
		}
		void Set(const casacore::Array<T> &values) throw() {
			_column.putColumnCells(casacore::RefRows(_row, _row), values);
		}
		bool operator!=(const ROArrayColumnIterator<T> &other) const throw() {
			return _row!=other._row;
		}
		bool operator==(const ROArrayColumnIterator<T> &other) const throw() {
			return _row==other._row;
		}
		static ROArrayColumnIterator First(casacore::ROArrayColumn<T> &column)
		{
			return ROArrayColumnIterator<T>(column, 0);
		}
	protected:
		casacore::ROArrayColumn<T> &_column;
		unsigned _row;
};

template<typename T>
class ArrayColumnIterator : public ROArrayColumnIterator<T> {
	public:
		ArrayColumnIterator(class casacore::ArrayColumn<T> &column, unsigned row) throw() :
			ROArrayColumnIterator<T>(column, row)
		{
		}
		ArrayColumnIterator(const ArrayColumnIterator<T> &source) throw() :
			ROArrayColumnIterator<T>(source)
		{
		}
		~ArrayColumnIterator() throw()
		{
		}
		void Set(const casacore::Array<T> &values) throw() {
			casacore::ArrayColumn<T> *col = static_cast<casacore::ArrayColumn<T>* >(&this->_column);
			col->basePut(this->_row, values);
		}
		static ArrayColumnIterator First(casacore::ArrayColumn<T> &column)
		{
			return ArrayColumnIterator<T>(column, 0);
		}
};


// I'm using the casa header files with a different version
// of the casa library. It, of course, is highly hacky work,
// but it was the only way it has worked for me so far. This however
// makes the following two definitions necessary. 

/*
namespace casacore {//#Begin casa namespace
template<class T>
Array<T>::ConstIteratorSTL::ConstIteratorSTL (const Array<T>& arr)
: itsLineIncr (0),
  itsCurPos   (arr.ndim(), 0),
  itsArray    (&arr),
  itsContig   (arr.contiguousStorage())
{
  // An empty array has to be handled.
  if (arr.nelements() == 0) {
    itsPos = 0;
    itsContig = True;
  } else {
    // Set the last cursor position.
    // Handle the case for the end iterator.
    itsLastPos = arr.shape() - 1;
    // If the array is not contiguous, we iterate "line by line" in
    // the increment function. Optimize for the case where the length
    // of the lower dimensions is 1. All such dimensions can be included
    // in the "line".
    // At the end itsLineAxis gives the axis where the next "line" starts.
    itsPos = &((*itsArray)(itsCurPos));
    if (!itsContig) {
      itsLineAxis = 0;
      while (itsLineAxis < arr.ndim()-1
             &&  itsLastPos(itsLineAxis) == 0) {
        itsLineAxis++;
      }
      itsCurPos(itsLineAxis) = 1;
      itsLineIncr = itsArray->steps()(itsLineAxis) - 1;
      ///&((*itsArray)(itsCurPos)) - itsPos - 1;
      itsLineEnd = itsPos + itsLastPos(itsLineAxis) * (itsLineIncr+1);
      itsCurPos(itsLineAxis) = 0;
    }
  }
}

template<class T>
void Array<T>::ConstIteratorSTL::increment()
{
  uInt axis;
  for (axis=itsLineAxis+1; axis<itsCurPos.nelements(); axis++) {
    if (itsCurPos(axis) < itsLastPos(axis)) {
      itsCurPos(axis)++;
      itsLineEnd += itsArray->steps()(axis);
      break;
    }
    itsCurPos(axis) = 0;
    itsLineEnd -= itsLastPos(axis) * itsArray->steps()(axis);
  }
  if (axis == itsCurPos.nelements()) {
    itsPos = itsArray->end();
  } else {
    itsPos = itsLineEnd - itsLastPos(itsLineAxis) * (itsLineIncr+1);
  }
}

};
*/

#endif
