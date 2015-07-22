/**********************************************************************\
 @file labeled_array.hpp
 ------------------

 TODO <Insert file description here>

 **********************************************************************

 Copyright (C) 2014  Bryan R. Gillis

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

#ifndef _BRG_CONTAINER_LABELED_ARRAY_HPP_INCLUDED_
#define _BRG_CONTAINER_LABELED_ARRAY_HPP_INCLUDED_

#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <utility>
#include <type_traits>

#include <boost/bimap.hpp>
#include <boost/iterator.hpp>
#include <boost/lexical_cast.hpp>

#include "IceBRG_main/Eigen.hpp"

#include "IceBRG_main/container/coerce.hpp"
#include "IceBRG_main/container/comparison_type.hpp"
#include "IceBRG_main/container/insertion_ordered_map.hpp"
#include "IceBRG_main/container/table_typedefs.hpp"

#include "IceBRG_main/file_access/ascii_table.hpp"
#include "IceBRG_main/file_access/open_file.hpp"

#include "IceBRG_main/math/misc_math.hpp"

#include "is_stl_container.hpp"
#include "labeled_array/labeled_array_col_iterator.hpp"
#include "labeled_array/labeled_array_col_reference.hpp"
#include "labeled_array/labeled_array_element_iterator.hpp"
#include "labeled_array/labeled_array_iterator_chooser.hpp"
#include "labeled_array/labeled_array_raw_col_iterator.hpp"
#include "labeled_array/labeled_array_raw_row_iterator.hpp"
#include "labeled_array/labeled_array_row_iterator.hpp"
#include "labeled_array/labeled_array_row_reference.hpp"
#include "labeled_array/labeled_array_vecs.hpp"

namespace IceBRG {

template<typename T_value_type=flt_type, char T_major_tag = Eigen::RowMajor, typename T_label_type=std::string>
class labeled_array
{
public:

	// Public typedefs
#if(1)

	typedef typename Eigen::Array<T_value_type,Eigen::Dynamic,Eigen::Dynamic,T_major_tag> data_table_type;
	typedef typename Eigen::Array<const T_value_type,Eigen::Dynamic,Eigen::Dynamic,T_major_tag> const_data_table_type;

	typedef T_value_type value_type;
	typedef const value_type const_value_type;
	typedef value_type & reference;
	typedef const_value_type & const_reference;

	typedef T_label_type label_type;
	typedef const label_type const_label_type;

	typedef int_type size_type;
	typedef ptrdiff_t difference_type;

	// Col typedefs
#if(1)

	typedef typename data_table_type::ColXpr col_type;
	typedef typename data_table_type::ConstColXpr const_col_type;

	typedef labeled_array_col_reference<labeled_array<value_type,T_major_tag,label_type>,col_type> col_reference;
	typedef labeled_array_col_reference<const labeled_array<value_type,T_major_tag,label_type>,const_col_type> const_col_reference;

	typedef labeled_array_col_iterator<labeled_array<value_type,T_major_tag,label_type>,
		col_type,col_reference> col_iterator;
	typedef labeled_array_col_iterator<const labeled_array<value_type,T_major_tag,label_type>,
		const_col_type,const_col_reference> const_col_iterator;
	typedef typename boost::reverse_iterator<col_iterator> reverse_col_iterator;
	typedef typename boost::reverse_iterator<const_col_iterator> const_reverse_col_iterator;

	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		col_type,const_col_type,col_reference,const_col_reference,col_iterator,const_col_iterator> cols_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_col_type,const_col_type,const_col_reference,const_col_reference,const_col_iterator,const_col_iterator> const_cols_type;
	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		col_type,const_col_type,col_reference,const_col_reference,col_iterator,const_col_iterator,reverse_tag> reverse_cols_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_col_type,const_col_type,const_col_reference,const_col_reference,const_col_iterator,const_col_iterator,reverse_tag> const_reverse_cols_type;

	typedef labeled_array_raw_col_iterator<labeled_array<value_type,T_major_tag,label_type>,
		col_type,col_type> raw_col_iterator;
	typedef labeled_array_raw_col_iterator<const labeled_array<value_type,T_major_tag,label_type>,
		const_col_type,const_col_type> const_raw_col_iterator;
	typedef typename boost::reverse_iterator<raw_col_iterator> reverse_raw_col_iterator;
	typedef typename boost::reverse_iterator<const_raw_col_iterator> const_reverse_raw_col_iterator;

	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		col_type,const_col_type,col_reference,const_col_reference,
		raw_col_iterator,const_raw_col_iterator> raw_cols_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_col_type,const_col_type,const_col_reference,const_col_reference,
		const_raw_col_iterator,const_raw_col_iterator> const_raw_cols_type;
	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		col_type,const_col_type,col_reference,const_col_reference,
		raw_col_iterator,const_raw_col_iterator,reverse_tag> reverse_raw_cols_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_col_type,const_col_type,const_col_reference,const_col_reference,
		const_raw_col_iterator,const_raw_col_iterator,reverse_tag> const_reverse_raw_cols_type;

#endif // Col typedefs

	// Row typedefs
#if(1)

	typedef typename data_table_type::RowXpr row_type;
	typedef typename data_table_type::ConstRowXpr const_row_type;

	typedef labeled_array_row_reference<labeled_array<value_type,T_major_tag,label_type>,row_type> row_reference;
	typedef labeled_array_row_reference<const labeled_array<value_type,T_major_tag,label_type>,const_row_type> const_row_reference;

	typedef labeled_array_row_iterator<labeled_array<value_type,T_major_tag,label_type>,
		row_type,row_reference> row_iterator;
	typedef labeled_array_row_iterator<const labeled_array<value_type,T_major_tag,label_type>,
		const_row_type,const_row_reference> const_row_iterator;
	typedef typename boost::reverse_iterator<row_iterator> reverse_row_iterator;
	typedef typename boost::reverse_iterator<const_row_iterator> const_reverse_row_iterator;

	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		row_type,const_row_type,row_reference,const_row_reference,row_iterator,const_row_iterator> rows_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_row_type,const_row_type,const_row_reference,const_row_reference,const_row_iterator,const_row_iterator> const_rows_type;
	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		row_type,const_row_type,row_reference,const_row_reference,row_iterator,const_row_iterator,reverse_tag> reverse_rows_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_row_type,const_row_type,const_row_reference,const_row_reference,const_row_iterator,const_row_iterator,reverse_tag> const_reverse_rows_type;

	typedef labeled_array_raw_row_iterator<labeled_array<value_type,T_major_tag,label_type>,
		row_type,row_type> raw_row_iterator;
	typedef labeled_array_raw_row_iterator<const labeled_array<value_type,T_major_tag,label_type>,
		const_row_type,const_row_type> const_raw_row_iterator;
	typedef typename boost::reverse_iterator<raw_row_iterator> reverse_raw_row_iterator;
	typedef typename boost::reverse_iterator<const_raw_row_iterator> const_reverse_raw_row_iterator;

	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		row_type,const_row_type,row_reference,const_row_reference,
		raw_row_iterator,const_raw_row_iterator> raw_rows_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_row_type,const_row_type,const_row_reference,const_row_reference,
		const_raw_row_iterator,const_raw_row_iterator> const_raw_rows_type;
	typedef labeled_array_vecs<labeled_array<value_type,T_major_tag,label_type>,
		row_type,const_row_type,row_reference,const_row_reference,
		raw_row_iterator,const_raw_row_iterator,reverse_tag> reverse_raw_rows_type;
	typedef labeled_array_vecs<const labeled_array<value_type,T_major_tag,label_type>,
		const_row_type,const_row_type,const_row_reference,const_row_reference,
		const_raw_row_iterator,const_raw_row_iterator,reverse_tag> const_reverse_raw_rows_type;

#endif // Row typedefs

	// Iterator typedefs
#if(1)

	typedef value_type * iterator;
	typedef const_value_type * const_iterator;
	typedef boost::reverse_iterator<iterator> reverse_iterator;
	typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

	typedef typename labeled_array_iterator_chooser<value_type,T_major_tag>::col_element_iterator col_element_iterator;
	typedef typename labeled_array_iterator_chooser<const_value_type,T_major_tag>::col_element_iterator const_col_element_iterator;
	typedef typename boost::reverse_iterator<col_element_iterator> reverse_col_element_iterator;
	typedef typename boost::reverse_iterator<const_col_element_iterator> const_reverse_col_element_iterator;

	typedef typename labeled_array_iterator_chooser<value_type,T_major_tag>::row_element_iterator row_element_iterator;
	typedef typename labeled_array_iterator_chooser<const_value_type,T_major_tag>::row_element_iterator const_row_element_iterator;
	typedef typename boost::reverse_iterator<row_element_iterator> reverse_row_element_iterator;
	typedef typename boost::reverse_iterator<const_row_element_iterator> const_reverse_row_element_iterator;

#endif // Iterator typedefs

#endif // Public typedefs

private:

	// Private section
#if(1)

	// Private typedefs
#if(1)
	typedef typename boost::bimap<label_type,size_type> map_type;

	typedef typename Eigen::Array<value_type,Eigen::Dynamic,1> column_buffer_column_type;
	typedef typename IceBRG::insertion_ordered_map<label_type,column_buffer_column_type> column_buffer_type;
	typedef typename column_buffer_type::value_type column_buffer_labeled_column_type;

	typedef typename std::vector<value_type> row_buffer_row_type;
	typedef typename std::vector<row_buffer_row_type> row_buffer_type;
	typedef typename row_buffer_type::value_type row_buffer_labeled_row_type;
#endif // Private typedefs

	// Private members
#if(1)
	mutable data_table_type _data_table_;
	mutable map_type _label_map_;
	mutable column_buffer_type _column_buffer_;
	mutable row_buffer_type _row_buffer_;
#endif // Private members

	// Friend classes
#if(1)
	friend row_reference;
	friend const_row_reference;
	friend col_reference;
	friend const_col_reference;

	friend cols_type;
	friend const_cols_type;
	friend reverse_cols_type;
	friend const_reverse_cols_type;
	friend rows_type;
	friend const_rows_type;
	friend reverse_rows_type;
	friend const_reverse_rows_type;

	friend raw_cols_type;
	friend const_raw_cols_type;
	friend reverse_raw_cols_type;
	friend const_reverse_raw_cols_type;
	friend raw_rows_type;
	friend const_raw_rows_type;
	friend reverse_raw_rows_type;
	friend const_reverse_raw_rows_type;
#endif // Friend classes

	// Private methods
#if(1)

	// Private methods to dump the buffer to the data table
#if(1)

	void _add_buffer_to_data_table() const
	{
		_add_column_buffer_to_data_table();
		_add_row_buffer_to_data_table();
	}

	void _add_column_buffer_to_data_table() const
	{
		// Check that we actually do have columns to add
		if(_column_buffer_.empty()) return;

		// Get the number of columns
		const size_type num_cols_to_add = ssize(_column_buffer_);
		const size_type old_num_cols = _data_table_.cols();
		const size_type new_num_cols = _data_table_.cols() + num_cols_to_add;
		size_type num_rows;

		// If the data table is initially empty, we'll get the number of rows from the first
		// buffered column
		if(ssize(_data_table_)==0)
		{
			num_rows = ssize(_column_buffer_.begin()->second);
		}
		else // Otherwise use the current number of rows
		{
			num_rows = _data_table_.rows();
		}

		// Check that all columns we'll be adding are the proper size before we change anything

		for( const auto & column : _column_buffer_)
		{
			// Check that the column is the proper size
			if(ssize(column.second)!=num_rows)
			{
				throw std::logic_error("All columns added to a labeled_array must have the same size.\n");
			}
		}

		// Resize the data table, conserving existing elements
		_data_table_.conservativeResize(num_rows, new_num_cols);

		// Go through the buffer and add each column to the data table. If the label already exists,
		// replace that column instead
		size_type current_column = old_num_cols;
		size_type num_preexisting_cols = 0; // number of labels which we find already exist
		for( const auto & column : _column_buffer_)
		{
			const_label_type & label = column.first;
			auto & column_data = column.second;

			// Check if the label already exists
			if(_label_map_.left.count(label)==1)
			{
				// Instead of adding a new column, we'll replace the existing one
				const size_type & column_index = _label_map_.left.at(label);
				for(size_type i=0; i<ssize(column_data); ++i)
				{
					_data_table_(i,column_index) = column_data[i];
				}
				++num_preexisting_cols;
			}
			else // New column to add
			{
				// Add this to the label map
				_label_map_.left.insert(std::make_pair(label,current_column));
				for(size_type i=0; i<ssize(column_data); ++i)
				{
					_data_table_(i,current_column) = column_data[i];
				}

				// Increment the current column index
				++current_column;
			}
		}

		// If we overwrote any preexisting columns, resize the data table now to the proper size
		if(num_preexisting_cols>0)
		{
			_data_table_.conservativeResize(Eigen::NoChange_t(), new_num_cols-num_preexisting_cols);
		}

		// Clear the buffer, and we're done
		_clear_column_buffer();

	}

	void _add_row_buffer_to_data_table() const
	{
		// Check that we actually do have rows to add
		if(_row_buffer_.empty()) return;

		// Get the number of rows
		const size_type num_rows_to_add = ssize(_row_buffer_);
		const size_type old_num_rows = _data_table_.rows();
		const size_type new_num_rows = _data_table_.rows() + num_rows_to_add;
		size_type num_cols;

		// If the data table is initially empty, we'll get the number of columns from the first
		// buffered row
		if(ssize(_data_table_)==0)
		{
			num_cols = ssize(_row_buffer_.front());
		}
		else // Otherwise use the current number of columns
		{
			num_cols = _data_table_.cols();
		}

		// Check that all rows we'll be adding are the proper size before we change anything

		for( const auto & row : _row_buffer_)
		{
			// Check that the row is the proper size
			if(static_cast<size_type>(ssize(row))!=num_cols)
			{
				throw std::logic_error("All rows added to a labeled_array must have the same size.\n");
			}
		}

		// If this is the first row being added, check if we need to create a dummy label map
		if(ssize(_data_table_)==0)
		{
			if(static_cast<size_type>(ssize(_label_map_.left))!=num_cols)
			{
				_generate_dummy_label_map(num_cols);
				assert(static_cast<size_type>(ssize(_label_map_.left))==num_cols);
			}
		}

		// Resize the data table, conserving existing elements
		_data_table_.conservativeResize(new_num_rows, num_cols);

		size_type current_row = old_num_rows;
		// Go through the buffer and add each row to the data table
		for( const auto & row : _row_buffer_)
		{
			for(typename row_buffer_row_type::size_type i=0; i<row.size(); ++i)
			{
				_data_table_(current_row,i) = row[i];
			}
			++current_row;
		}

		// Clear the buffer, and we're done
		_clear_row_buffer();

	}

#endif // Private methods to dump the buffer to the data table

	// Private label map methods
#if(1)

	void _generate_dummy_label_map(const size_type & num_cols) const
	{
		_clear_label_map();
		for(size_type i=0; i<num_cols; ++i)
		{
			std::stringstream ss("col_");
			ss << i;
			_label_map_.left.insert(typename map_type::left_value_type(ss.str(),i));
		}
	}

	template< typename other_map_type,
	typename std::enable_if<std::is_convertible<typename other_map_type::value_type,label_type>::value,char>::type = 0>
	void _set_label_map(const other_map_type & new_labels)
	{
		_clear_label_map();
		size_type i = 0;
		for(const auto & label : new_labels)
		{
			_label_map_.left.insert(typename map_type::left_value_type(label,i++));
		}
	}

	template< typename other_map_type,
	typename std::enable_if<std::is_convertible<typename other_map_type::value_type,label_type>::value,char>::type = 0>
	void _set_label_map(other_map_type && new_labels)
	{
		_clear_label_map();
		typename IceBRG::ct<typename map_type::left_value_type::second_type>::type i = 0;
		for(auto & label : new_labels)
		{
			_label_map_.left.insert(typename map_type::left_value_type(std::move(label),i++));
		}
	}

	template< typename other_map_type,
	typename std::enable_if<std::is_convertible<typename other_map_type::value_type,typename map_type::left_value_type>::value,char>::type = 0>
	void _set_label_map(const other_map_type & other_map)
	{
		_clear_label_map();
		for(const auto & value : other_map)
		{
			_label_map_.left.insert(typename map_type::left_value_type(value));
		}
	}

	template< typename other_map_type,
	typename std::enable_if<std::is_convertible<typename other_map_type::value_type,typename map_type::left_value_type>::value,char>::type = 0>
	void _set_label_map(other_map_type && other_map)
	{
		_clear_label_map();
		for(auto & value : other_map)
		{
			_label_map_.left.insert(typename map_type::left_value_type(std::move(value)));
		}
	}

	template< typename other_map_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<other_map_type>::type,map_type>::value,char>::type = 0>
	void _set_label_map(other_map_type && other_map)
	{
		_label_map_ = std::forward<other_map_type>(other_map);
	}

#endif // Private label map methods

	// Private clearing methods
#if(1)

	// Clear the column buffer
	void _clear_column_buffer() const
	{
		_column_buffer_.clear();
	}

	// Clear the row buffer
	void _clear_row_buffer() const
	{
		_row_buffer_.clear();
	}

	// Clear both buffers
	void _clear_buffer() const
	{
		_clear_column_buffer();
		_clear_row_buffer();
	}

	void _clear_data_table()
	{
		_data_table_.resize(0,0);
	}

	void _clear_label_map() const
	{
		_label_map_.left.clear();
	}

#endif // Private clearing methods

	// Private range-checking methods
#if(1)

	void _check_valid_col_index(const size_type & i) const
	{
		_add_buffer_to_data_table();

		if((i<0) || (i>=num_cols()))
			throw std::out_of_range(std::string("Invalid column index requested from labeled_array: ") + std::to_string(i));
	}

	void _check_valid_row_index(const size_type & i) const
	{
		_add_buffer_to_data_table();

		if((i<0) || (i>=num_rows()))
			throw std::out_of_range(std::string("Invalid row index requested from labeled_array: ") + std::to_string(i));
	}

	void _check_valid_label(const label_type & label) const
	{
		_add_buffer_to_data_table();

		if(count(label)==0)
			throw std::out_of_range(std::string("Invalid label requested from labeled_array: ") + boost::lexical_cast<std::string>(label));
	}

#endif

	// Other private methods
#if(1)

	// Private implementation of loading - doesn't clear first
    void _load(std::istream & fi)
    {
    	set_labels(load_header(fi));
    	set_rows(load_table<T_value_type>(fi,Eigen::RowMajor,T_value_type(),ssize(_label_map_)));
    }

#endif // Other private methods

#endif // Private methods

#endif // Private section

public:

	// Constructors
#if(1)

	/// Default constructor
	labeled_array() {}

	/// Copy/move from table_map
	template< typename other_table_map_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<other_table_map_type>::type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<other_table_map_type>::type::mapped_type>::value,char>::type = 0>
	explicit labeled_array(other_table_map_type && init_column_buffer)
	: _column_buffer_(std::forward<other_table_map_type>(init_column_buffer))
	{
	}

	/// Copy/move from vector of vectors
	template< typename other_table_map_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<other_table_map_type>::type,row_buffer_type>::value,char>::type = 0>
	explicit labeled_array(other_table_map_type && init_row_buffer)
	: _row_buffer_(std::forward<other_table_map_type>(init_row_buffer))
	{
	}

	/// Copy/move from vector of vectors and copy label map
	template< typename other_table_map_type,
	typename init_label_map_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<other_table_map_type>::type,row_buffer_type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<init_label_map_type>::type::value_type,label_type>::value ||
	                std::is_convertible<typename IceBRG::ct<init_label_map_type>::type::value_type,typename map_type::left_value_type>::value ||
					std::is_convertible<typename IceBRG::ct<init_label_map_type>::type,map_type>::value,char>::type = 0>
	explicit labeled_array(other_table_map_type && init_row_buffer,
						   init_label_map_type && init_label_map)
	: _row_buffer_(std::forward<other_table_map_type>(init_row_buffer))
	{
		set_labels(std::forward<init_label_map_type>(init_label_map));
	}

	/// Copy/move from array
	template< typename other_data_table_type,
	typename std::enable_if<IceBRG::is_eigen_container<typename IceBRG::ct<other_data_table_type>::type>::value,char>::type = 0>
	explicit labeled_array(other_data_table_type && other_data_table)
	: _data_table_(std::forward<other_data_table_type>(other_data_table))
	{
		_generate_dummy_label_map(_data_table_.cols());
	}

	/// Copy/move from array and copy label map
	template< typename other_data_table_type,
	typename init_label_map_type,
	typename std::enable_if<IceBRG::is_eigen_container<typename IceBRG::ct<other_data_table_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<init_label_map_type>::type::value_type,label_type>::value ||
	                std::is_convertible<typename IceBRG::ct<init_label_map_type>::type::value_type,typename map_type::left_value_type>::value ||
					std::is_convertible<typename IceBRG::ct<init_label_map_type>::type,map_type>::value,char>::type = 0>
	explicit labeled_array(other_data_table_type && other_data_table,
						   init_label_map_type && init_label_map)
	: _data_table_(std::forward<other_data_table_type>(other_data_table))
	{
		if(set_labels(std::forward<init_label_map_type>(init_label_map)))
			_generate_dummy_label_map(_data_table_.cols());
	}

	/// Load from file stream
	explicit labeled_array(std::istream & fi)
    {
    	_load(fi);
    }

	/// Load from file name
	explicit labeled_array(const std::string & file_name)
    {
    	std::ifstream fi;
    	open_file_input(fi,file_name);

    	_load(fi);
    }

#endif

	// Data access
#if(1)
	const data_table_type & data_table() const
	{
		_add_buffer_to_data_table();
		return _data_table_;
	}
#endif

	// Column access
#if(1)
	col_reference col(const size_type & index)
	{
		_check_valid_col_index(index);

		return col_reference(this,index,num_cols());
	}
	const_col_reference col(const size_type & index) const
	{
		_check_valid_col_index(index);

		return const_col_reference(this,index,num_cols());
	}
	col_type raw_col(const size_type & index)
	{
		_check_valid_col_index(index);

		return _data_table_.col(index);
	}
	const_col_type raw_col(const size_type & index) const
	{
		_check_valid_col_index(index);

		return const_cast<const data_table_type &>(_data_table_).col(index);
	}
	col_reference at_label(const label_type & label)
	{
		_check_valid_label(label);

		return col(_label_map_.left.at(label));
	}
	const_col_reference at_label(const label_type & label) const
	{
		_check_valid_label(label);

		return col(_label_map_.left.at(label));
	}
#endif

	// Iterable table access
#if(1)

	// Get an iterable set of columns
	const_cols_type cols() const
	{
		return const_cols();
	}
	cols_type cols()
	{
		return cols_type(this,num_cols(),num_rows());
	}
	const_cols_type const_cols() const
	{
		return const_cols_type(this,num_cols(),num_rows());
	}

	// Get a reverse iterable set of columns
	const_reverse_cols_type reverse_cols() const
	{
		return const_reverse_cols();
	}
	reverse_cols_type reverse_cols()
	{
		return reverse_cols_type(this,num_cols(),num_rows());
	}
	const_reverse_cols_type const_reverse_cols() const
	{
		return const_reverse_cols_type(this,num_cols(),num_rows());
	}

	// Get an iterable set of raw columns
	const_raw_cols_type raw_cols() const
	{
		return const_raw_cols();
	}
	raw_cols_type raw_cols()
	{
		return raw_cols_type(this,num_cols(),num_rows());
	}
	const_raw_cols_type const_raw_cols() const
	{
		return const_raw_cols_type(this,num_cols(),num_rows());
	}

	// Get a reverse iterable set of raw columns
	const_reverse_raw_cols_type reverse_raw_cols() const
	{
		return const_reverse_raw_cols();
	}
	reverse_raw_cols_type reverse_raw_cols()
	{
		return reverse_raw_cols_type(this,num_cols(),num_rows());
	}
	const_reverse_raw_cols_type const_reverse_raw_cols() const
	{
		return const_reverse_raw_cols_type(this,num_cols(),num_rows());
	}

	// Get an iterable set of rows
	const_rows_type rows() const
	{
		return const_rows();
	}
	rows_type rows()
	{
		return rows_type(this,num_rows(),num_cols());
	}
	const_rows_type const_rows() const
	{
		return const_rows_type(this,num_rows(),num_cols());
	}

	// Get a reverse iterable set of rows
	const_reverse_rows_type reverse_rows() const
	{
		return const_reverse_rows();
	}
	reverse_rows_type reverse_rows()
	{
		return reverse_rows_type(this,num_rows(),num_cols());
	}
	const_reverse_rows_type const_reverse_rows() const
	{
		return const_reverse_rows_type(this,num_rows(),num_cols());
	}

	// Get an iterable set of raw rows
	const_raw_rows_type raw_rows() const
	{
		return const_raw_rows();
	}
	raw_rows_type raw_rows()
	{
		return raw_rows_type(this,num_rows(),num_cols());
	}
	const_raw_rows_type const_raw_rows() const
	{
		return const_raw_rows_type(this,num_rows(),num_cols());
	}

	// Get a reverse iterable set of raw rows
	const_reverse_raw_rows_type reverse_raw_rows() const
	{
		return const_reverse_raw_rows();
	}
	reverse_raw_rows_type reverse_raw_rows()
	{
		return reverse_raw_rows_type(this,num_rows(),num_cols());
	}
	const_reverse_raw_rows_type const_reverse_raw_rows() const
	{
		return const_reverse_raw_rows_type(this,num_rows(),num_cols());
	}

#endif

    // Single column insertion
#if(1)

    template< typename new_column_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_column_type>::type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_column_type>::type,column_buffer_labeled_column_type>::value,char>::type = 0>
    void insert_col(new_column_type && new_column)
    {
    	_add_row_buffer_to_data_table();
    	_column_buffer_.insert(std::forward<new_column_type>(new_column));
    }

    template< typename new_column_type,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_column_type>::type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_column_type>::type,column_buffer_labeled_column_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_column_type>::type>::value,char>::type = 0>
    void insert_col(new_column_type && new_column)
    {
    	_add_row_buffer_to_data_table();
    	_column_buffer_.insert(IceBRG::coerce<column_buffer_column_type>(std::forward<new_column_type>(new_column)),_label_map_.left);
    }

    template< typename new_column_type,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_column_type>::type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_column_type>::type,column_buffer_labeled_column_type>::value),char>::type = 0,
	typename std::enable_if<!IceBRG::is_stl_container<typename IceBRG::ct<new_column_type>::type>::value,char>::type = 0>
    void insert_col(new_column_type && new_column)
    {
    	// For pairs with a coercible second type
    	_add_row_buffer_to_data_table();
    	_column_buffer_.insert(std::make_pair(new_column.first,IceBRG::coerce<column_buffer_column_type>(new_column.second)));
    }

#endif // Single column insertion

    // Multiple column insertion
#if(1)

    template< typename new_columns_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value,char>::type = 0>
    void insert_cols(const new_columns_type & new_columns)
    {
    	_add_row_buffer_to_data_table();

    	for(const auto & new_column : new_columns)
    	{
    		_column_buffer_.insert(new_column);
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value,char>::type = 0>
    void insert_cols(new_columns_type && new_columns)
    {
    	_add_row_buffer_to_data_table();

    	for(auto && new_column : new_columns)
    	{
    		_column_buffer_.insert(std::move(new_column));
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type::value_type>::value,char>::type = 0>
    void insert_cols(const new_columns_type & new_columns)
    {
    	_add_row_buffer_to_data_table();

    	for(const auto & new_column : new_columns)
    	{
    		_column_buffer_.insert(IceBRG::coerce<column_buffer_column_type>(new_column),_label_map_.left);
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_columns_type::value_type>::type>::value,char>::type = 0>
    void insert_cols(new_columns_type && new_columns)
    {
    	_add_row_buffer_to_data_table();

    	for(auto && new_column : new_columns)
    	{
    		_column_buffer_.insert(IceBRG::coerce<column_buffer_column_type>(std::move(new_column)),_label_map_.left);
    	}
    }

#endif // Multiple column insertion

    // Single row insertion
#if(1)

    template< typename new_row_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_row_type>::type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_row_type>::type,row_buffer_labeled_row_type>::value,char>::type = 0>
    void insert_row(new_row_type && new_row)
    {
    	_add_column_buffer_to_data_table();
    	_row_buffer_.push_back(std::forward<new_row_type>(new_row));
    }

    template< typename new_row_type,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_row_type>::type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_row_type>::type,row_buffer_labeled_row_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_container<typename IceBRG::ct<new_row_type>::type>::value,char>::type = 0>
    void insert_row(new_row_type && new_column)
    {
    	_add_column_buffer_to_data_table();
    	_row_buffer_.push_back(IceBRG::coerce<row_buffer_row_type>(std::forward<new_row_type>(new_column)));
    }

#endif // Single row insertion

    // Multiple row insertion
#if(1)

    template< typename new_rows_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_labeled_row_type>::value,char>::type = 0>
    void insert_rows(const new_rows_type & new_rows)
    {
    	_add_column_buffer_to_data_table();

    	for(const auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(new_row);
    	}
    }

    template< typename new_rows_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_labeled_row_type>::value,char>::type = 0>
    void insert_rows(new_rows_type && new_rows)
    {
    	_add_column_buffer_to_data_table();

    	for(auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(std::move(new_row));
    	}
    }

    template< typename new_rows_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_rows_type::value_type>::type>::value,char>::type = 0>
    void insert_rows(const new_rows_type & new_rows)
    {
    	_add_column_buffer_to_data_table();

    	for(const auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(IceBRG::coerce<row_buffer_row_type>(new_row));
    	}
    }

    template< typename new_rows_type,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_rows_type::value_type>::type>::value,char>::type = 0>
    void insert_rows(new_rows_type && new_rows)
    {
    	_add_column_buffer_to_data_table();

    	for(auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(IceBRG::coerce<row_buffer_row_type>(std::move(new_row)));
    	}
    }

#endif // Multiple row insertion

    // Column access/insertion
#if(1)
    col_type operator[](const label_type & label)
	{
    	// Check if the label is new
    	if(count(label)==0)
    	{
    		// The label is new, so insert a column for it
    		insert_col(std::make_pair(label,column_buffer_column_type(_data_table_.rows())));
    		_add_column_buffer_to_data_table();
    	}
    	// Return a reference to this label's column
		return base().col(_label_map_.left.at(label));
	}
#endif

	// Row access
#if(1)
	row_reference row(const size_type & index)
	{
		_check_valid_row_index(index);

		return row_reference(this,index,num_rows());
	}
	const_row_reference row(const size_type & index) const
	{
		_check_valid_row_index(index);

		return const_row_reference(this,index,num_rows());
	}
	row_reference operator[](const size_type & index)
	{
		return row(index);
	}
	const_row_reference operator[](const size_type & index) const
	{
		return row(index);
	}
	row_reference operator()(const size_type & index)
	{
		_check_valid_row_index(index);

		return row(index);
	}
	const_row_reference operator()(const size_type & index) const
	{
		_check_valid_row_index(index);

		return row(index);
	}
	row_type raw_row(const size_type & index)
	{
		_check_valid_row_index(index);

		return _data_table_.row(index);
	}
	const_row_type raw_row(const size_type & index) const
	{
		_check_valid_row_index(index);

		return const_cast<const data_table_type &>(_data_table_).row(index);
	}
#endif

	// Element access
#if(1)
	const_reference operator()(const size_type & row_index, const size_type & col_index) const
	{
		return base()(row_index,col_index);
	}
	reference operator()(const size_type & row_index, const size_type & col_index)
	{
		return base()(row_index,col_index);
	}
#endif

	// Data access
#if(1)
	// Base table access after adding in buffer
	const data_table_type & base() const
	{
		_add_buffer_to_data_table();
		return _data_table_;
	}
	data_table_type & base()
	{
		_add_buffer_to_data_table();
		return _data_table_;
	}
	const_data_table_type & raw() const
	{
		return base();
	}
	data_table_type & raw()
	{
		return base();
	}

	// Base table access without adding in buffer
	const_data_table_type & base_bypass_buffer() const noexcept
	{
		return _data_table_;
	}
	data_table_type & base_bypass_buffer() noexcept
	{
		return _data_table_;
	}
	const_data_table_type & raw_bypass_buffer() const noexcept
	{
		return base_bypass_buffer();
	}
	data_table_type & raw_bypass_buffer() noexcept
	{
		return base_bypass_buffer();
	}

	// Data pointer access
	const_value_type * data() const
	{
		return base().data();
	}
	value_type * data()
	{
		return base().data();
	}
	const_value_type * data_bypass_buffer() const noexcept
	{
		return _data_table_.data();
	}
	value_type * data_bypass_buffer() noexcept
	{
		return _data_table_.data();
	}

#endif

	// Full table modification
#if(1)

	void clear()
	{
		_clear_buffer();
		_clear_label_map();
		_clear_data_table();
	}

    template< typename new_rows_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_rows_type>::type,row_buffer_type>::value,char>::type = 0>
    void set_rows(new_rows_type && new_rows)
    {
		_clear_buffer();
		_clear_data_table();

    	_row_buffer_ = std::forward<new_rows_type>(new_rows);
    }

    template< typename new_rows_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_rows_type>::type,row_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_labeled_row_type>::value,char>::type = 0>
    void set_rows(const new_rows_type & new_rows)
    {
		_clear_buffer();
		_clear_data_table();

    	for(const auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(new_row);
    	}
    }

    template< typename new_rows_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_rows_type>::type,row_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_labeled_row_type>::value,char>::type = 0>
    void set_rows(new_rows_type && new_rows)
    {
		_clear_buffer();
		_clear_data_table();

    	for(auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(std::move(new_row));
    	}
    }

    template< typename new_rows_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_rows_type>::type,row_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_rows_type::value_type>::type>::value,char>::type = 0>
    void set_rows(const new_rows_type & new_rows)
    {
		_clear_buffer();
		_clear_data_table();

    	for(const auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(IceBRG::coerce<row_buffer_row_type>(new_row));
    	}
    }

    template< typename new_rows_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_rows_type>::type,row_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_rows_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_rows_type>::type::value_type,row_buffer_row_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_rows_type::value_type>::type>::value,char>::type = 0>
    void set_rows(new_rows_type && new_rows)
    {
		_clear_buffer();
		_clear_data_table();

    	for(auto & new_row : new_rows)
    	{
    		_row_buffer_.push_back(IceBRG::coerce<row_buffer_row_type>(std::move(new_row)));
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_columns_type>::type,column_buffer_type>::value,char>::type = 0>
    void set_cols(new_columns_type && new_cols)
    {
		_clear_buffer();
		_clear_data_table();

    	_column_buffer_ = std::forward<new_columns_type>(new_cols);
    }

    template< typename new_columns_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_columns_type>::type,column_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value,char>::type = 0>
    void set_cols(const new_columns_type & new_columns)
    {
		_clear_buffer();
		_clear_data_table();

    	for(const auto & new_column : new_columns)
    	{
    		_column_buffer_.insert(new_column);
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_columns_type>::type,column_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value,char>::type = 0>
    void set_cols(new_columns_type && new_columns)
    {
		_clear_buffer();
		_clear_data_table();

    	for(auto && new_column : new_columns)
    	{
    		_column_buffer_.insert(std::move(new_column));
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_columns_type>::type,column_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_columns_type::value_type>::type>::value,char>::type = 0>
    void set_cols(const new_columns_type & new_columns)
    {
		_clear_buffer();
		_clear_data_table();

    	for(const auto & new_column : new_columns)
    	{
    		_column_buffer_.insert(IceBRG::coerce<column_buffer_column_type>(new_column),_label_map_.left);
    	}
    }

    template< typename new_columns_type,
	typename std::enable_if<!std::is_convertible<typename IceBRG::ct<new_columns_type>::type,column_buffer_type>::value,char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<new_columns_type>::type>::value,char>::type = 0,
	typename std::enable_if<!(std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_column_type>::value ||
	std::is_convertible<typename IceBRG::ct<new_columns_type>::type::value_type,column_buffer_labeled_column_type>::value),char>::type = 0,
	typename std::enable_if<IceBRG::is_stl_container<typename IceBRG::ct<typename new_columns_type::value_type>::type>::value,char>::type = 0>
    void set_cols(new_columns_type && new_columns)
    {
		_clear_buffer();
		_clear_data_table();

    	for(auto && new_column : new_columns)
    	{
    		_column_buffer_.insert(IceBRG::coerce<column_buffer_column_type>(std::move(new_column)),_label_map_.left);
    	}
    }

#endif // Full table modification

	// Size information
#if(1)
	size_type nrow() const
	{
		return num_rows();
	}
	size_type nrows() const
	{
		return num_rows();
	}
	size_type num_rows() const
	{
		return base().rows();
	}
	size_type ncol() const
	{
		return num_cols();
	}
	size_type ncols() const
	{
		return num_cols();
	}
	size_type num_cols() const
	{
		return base().cols();
	}
	size_type size() const
	{
		return ssize(base());
	}
	bool empty() const
	{
		return size()==0;
	}
#endif

	// Label information
#if(1)
	char count(const label_type & k) const
    {
    	// Add in the buffer so the label map is fully set up
    	_add_buffer_to_data_table();

		return _label_map_.left.count(k);
    }

    const label_type & get_label_for_column(const size_type & index) const
    {
    	// Add in the buffer so the label map is fully set up
		_add_buffer_to_data_table();

    	_check_valid_col_index(index);
    	return _label_map_.right.at(index);
    }

    const size_type & get_index_for_label(const label_type & label) const
    {
    	// Add in the buffer so the label map is fully set up
		_add_buffer_to_data_table();

    	_check_valid_label(label);
    	return _label_map_.left.at(label);
    }

    template<typename T_res_label_type=label_type>
    std::vector<T_res_label_type> get_labels() const
	{
    	std::vector<T_res_label_type> res;

    	for( size_type i=0; i<num_cols(); ++i )
    	{
    		res.push_back(get_label_for_column(i));
    	}

    	return res;
	}
#endif

    // Label control
#if(1)

    /**
     * Changes a label to another, without altering its mapped data or position.
     *
     * @param init_label
     * @param new_label
     * @return char - 0 if successful
     *                1 if init_label doesn't exist in map
     *                2 if new_label already exists in map
     */
    template< typename new_label_type >
    char change_label(const label_type & init_label, new_label_type && new_label)
    {
    	// Add in the buffer so the label map is fully set up
		_add_buffer_to_data_table();

    	// Get the position of the value we're going to be altering
    	auto it = _label_map_.left.find(init_label);

    	if(it==_label_map_.left.end()) return 1;
    	if(_label_map_.left.count(new_label)>0) return 2;

    	auto pos = it->second;

    	// Alter the value in the label map by erasing old entry and adding new
    	_label_map_.left.erase(it);
    	_label_map_.left.insert(std::make_pair(std::forward<new_label_type>(new_label),pos));

    	return 0;
    }

	template< typename label_container_type,
	typename std::enable_if< std::is_convertible<typename IceBRG::ct<label_container_type>::type::value_type,label_type>::value ||
	                std::is_convertible<typename IceBRG::ct<label_container_type>::type::value_type,typename map_type::left_value_type>::value ||
					std::is_convertible<typename IceBRG::ct<label_container_type>::type,map_type>::value,char>::type = 0>
    char set_labels(label_container_type && new_labels)
    {
    	// Add in the buffer so the previous label map is fully set up
		_add_buffer_to_data_table();

		// Allow this if the new labels vector is equal in size to the number of
		// columns or if the table is empty
		if((static_cast<size_type>(ssize(new_labels))==ncols()) || (ncols()==0))
		{
			_set_label_map(std::forward<label_container_type>(new_labels));
			return 0;
		}

		if(static_cast<size_type>(ssize(new_labels))>ncols()) return 1;
    	return 2; // Implicitly ssize(new_labels)<ncols()
    }

#endif

    // Advanced operations
#if(1)

	void reverse_vertical()
	{
    	_add_buffer_to_data_table();
		for( auto col : raw_cols())
		{
			col.reverseInPlace();
		}
	}

    // Apply unit conversions
    template< typename unitconvs >
    void apply_unitconvs( const unitconvs & u_map )
    {
    	_add_buffer_to_data_table();
    	for(auto u_it=u_map.begin(); u_it!=u_map.end(); ++u_it)
    	{
			flt_type factor = 1./(u_it->second);
			if(isbad(factor)) factor = 1; // To catch user mistakes

    		auto d_it = _label_map_.left.find(u_it->first);
    		// Check if this value is actually in the data table
    		if(d_it!=_label_map_.left.end())
    		{
    			// It's in the table, so multiply its associated vector by the inverse unit conversion
    			raw_col(d_it->second) *= factor;
    		}
    	}
    }

    // Apply unit conversions
    template< typename unitconvs >
    void apply_inverse_unitconvs( const unitconvs & u_map )
    {
    	_add_buffer_to_data_table();
    	for(auto u_it=u_map.begin(); u_it!=u_map.end(); ++u_it)
    	{
			flt_type factor = u_it->second;
			if(isbad(factor)) factor = 1; // To catch user mistakes

    		auto d_it = _label_map_.left.find(u_it->first);
    		// Check if this value is actually in the data table
    		if(d_it!=_label_map_.left.end())
    		{
    			// It's in the table, so multiply its associated vector by the unit conversion
    			raw_col(d_it->second) *= factor;
    		}
    	}
    }

    // Fix bad values
    void fixbad()
    {
    	_add_buffer_to_data_table();
    	for(value_type * it = _data_table_.data(); it != _data_table_.data() + ssize(_data_table_); ++it)
    	{
    		IceBRG::fixbad(*it);
    	}
    }

#endif

    // Saving and loading in ascii format
#if(1)

    void save(std::ostream & fo, bool formatted=false) const
    {
    	header_t header = get_labels<std::string>();

    	if(formatted)
    	{
    		// To print formatted, we'll use the existing print_table function

			table_t<T_value_type> data;

			// Fill up the output table
			for( const auto & row : rows())
			{
				data.push_back(coerce<std::vector<T_value_type>>(row.raw()));
			}
	    	print_table(fo,data,header,Eigen::RowMajor);
    	}
    	else
    	{
    		// If not formatted, we'll use Eigen's output to simply print the table

    		// First, the header
    		fo << "# ";
    		for(const auto & label : header)
    		{
    			fo << label << "\t";
    		}
    		fo << std::endl;

    		// And now the data table
    		fo << data_table();
    	}

    	return;
    }

    void save(const std::string & file_name, bool formatted=false) const
    {
    	std::ofstream fo;
    	open_file_output(fo,file_name);

    	save(fo,formatted);
    }

    void load(std::istream & fi)
    {
    	clear();

    	_load(fi);
    }

    void load(const std::string & file_name)
    {
    	std::ifstream fi;
    	open_file_input(fi,file_name);

    	load(fi);
    }

#endif

};

}



#endif // _BRG_CONTAINER_LABELED_ARRAY_HPP_INCLUDED_
