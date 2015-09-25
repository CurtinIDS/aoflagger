#ifndef SCALARCOLUMNITERATOR_H
#define SCALARCOLUMNITERATOR_H

#include <casacore/ms/MeasurementSets/MSColumns.h>
#include <casacore/tables/Tables/ScalarColumn.h>

/**
	@author A.R. Offringa <offringa@astro.rug.nl>
*/
template<typename T>
class ScalarColumnIterator {
	public:
		ScalarColumnIterator(class casacore::ScalarColumn<T> &column, unsigned row) :
			_column(&column), _row(row)
		{
		}
		ScalarColumnIterator(const ScalarColumnIterator<T> &source) :
			_column(source._column), _row(source._row)
		{ }
		~ScalarColumnIterator() { }
		ScalarColumnIterator &operator=(const ScalarColumnIterator<T> &source)
		{
			_column = source._column;
			_row = source._row;
			return *this;
		}
		ScalarColumnIterator<T> &operator++() {
			_row++;
			return *this;
		}
		T *operator->() const {
			return (*_column)(_row);
		}
		T operator*() const {
			return (*_column)(_row);
		}
		bool operator!=(const ScalarColumnIterator<T> &other) const {
			return _row!=other._row;
		}
		bool operator==(const ScalarColumnIterator<T> &other) const {
			return _row==other._row;
		}
		static ScalarColumnIterator First(casacore::ScalarColumn<T> &column)
		{
			return ScalarColumnIterator<T>(column, 0);
		}
	private:
		casacore::ScalarColumn<T> *_column;
		unsigned _row;
};

#endif
