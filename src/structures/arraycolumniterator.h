#ifndef ARRAYCOLUMNITERATOR_H
#define ARRAYCOLUMNITERATOR_H

#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/tables/Tables/RefRows.h>

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

#endif
