/**********************************************************************\
 @file binary_archive.hpp
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

// body file: binary_archive.cpp

#ifndef _BRG_FILE_ACCESS_BINARY_ARCHIVE_HPP_INCLUDED_
#define _BRG_FILE_ACCESS_BINARY_ARCHIVE_HPP_INCLUDED_

#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "../container/is_stl_container.hpp"

#include "IceBRG_main/file_access/open_file.hpp"
#include "IceBRG_main/utility.hpp"

namespace IceBRG {

template<typename T, typename std::enable_if<!IceBRG::is_stl_container<T>::value,char>::type = 0>
void binary_save(boost::archive::binary_oarchive &ar, const T & obj)
{
	ar << obj;
}

template<typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value,char>::type = 0,
	typename std::enable_if<std::is_same<typename T::value_type,bool>::value,char>::type = 0>
void binary_save(boost::archive::binary_oarchive &ar, const T & obj)
{
	auto size = obj.size();
	ar << size;

	decltype(size) i;

	// Save most data
	if(size>CHAR_BIT-1)
	{
		for(i=0; i<size-CHAR_BIT; i+=CHAR_BIT)
		{
			unsigned char byte=0;
			for(unsigned char b=0;b<CHAR_BIT;++b)
			{
				byte |= (obj[i+b] << b);
			}

			ar << byte;
		}
	}

	if(size>i)
	{
		// Save any remaining data
		unsigned char byte=0;
		for(unsigned char b=0; b < size-i; ++b)
		{
			byte |= (obj[i+b] << b);
		}
		ar << byte;
	}
}

// Vector overload
template<typename T, typename std::enable_if<IceBRG::is_stl_container<T>::value,char>::type = 0,
	typename std::enable_if<!std::is_same<typename T::value_type,bool>::value,char>::type = 0>
void binary_save(boost::archive::binary_oarchive &ar, const std::vector<T> & obj)
{
	auto size = obj.size();
	ar << size;
	for(const auto & elem : obj)
	{
		binary_save<T>(ar,elem);
	}
}

template<typename T>
void binary_save(std::ostream &out, const T & obj)
{
	boost::archive::binary_oarchive ar(out);
	ar << obj;
}
template<typename T>
void binary_save(const std::string & filename, const T & obj)
{
	std::ofstream out;
	open_file_output(out,filename);
	binary_save(out,obj);
}

template<typename T, typename T_test=T, typename std::enable_if<!IceBRG::is_stl_container<T_test>::value,char>::type = 0 >
T binary_load(boost::archive::binary_iarchive & ar)
{
	T obj;
	ar >> obj;
	return obj;
}

// Vector<bool> overload
template<typename T, typename T_test=T,  typename std::enable_if<IceBRG::is_stl_container<T_test>::value,char>::type = 0,
	typename std::enable_if<std::is_same<typename T_test::value_type,bool>::value,char>::type = 0 >
T binary_load(boost::archive::binary_iarchive & ar)
{
	ssize_t size;
	ar >> size;
	T result(size);

	ssize_t i=0;
	// Load most of the data
	if(size>CHAR_BIT-1)
	{
		for(; i<size-CHAR_BIT; i+=CHAR_BIT)
		{
			unsigned char byte;
			ar >> byte;
			for(unsigned char b=0; b<CHAR_BIT; ++b)
			{
				result[i+b] = byte & (1 << b);
			}
		}
	}

	if(size>i)
	{
		// Load the remainder of the data
		unsigned char byte;
		ar >> byte;
		for(unsigned char b=0; b<size-i; ++b)
		{
			result[i+b] = byte & (1 << b);
		}
	}

	return result;
}

// Vector overload
template<typename T, typename T_test=T,  typename std::enable_if<IceBRG::is_stl_container<T_test>::value,char>::type = 0,
	typename std::enable_if<!std::is_same<typename T_test::value_type,bool>::value,char>::type = 0 >
T binary_load(boost::archive::binary_iarchive &ar)
{
	typename T::size_type size;
	ar >> size;
	T result(size);

	for(auto & val : result)
	{
		val = binary_load<typename T::value_type>(ar);
	}

	return result;
}

template<typename T>
T binary_load(std::istream & in)
{
	boost::archive::binary_iarchive ar(in);
	return binary_load<T>(ar);
}
template<typename T>
T binary_load(const std::string & filename)
{
	std::ifstream in;
	open_file_input(in,filename);
	return binary_load<T>(in);
}

} // end namespace IceBRG


#endif // _BRG_FILE_ACCESS_BINARY_ARCHIVE_HPP_INCLUDED_
