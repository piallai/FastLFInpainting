/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/


#pragma once

#include <iostream>
#include <string>
#include <sstream> 
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>//find
#include "Macros.h"

/*! Namespace containing methods to conveniently call STL/boost functions. Sort of STL/boost API.*/
namespace Misc {

	EXTERN_CELF_API std::string data_path;

	/*! Hight level verbose.*/
	EXTERN_CELF_API bool l_verbose_high;
	/*! Low level verbose.*/
	EXTERN_CELF_API bool l_verbose_low;

	template <class T>
	std::string to_string(const T& _value, const int _Ndigits=-1);

	std::string get_file_extension(const std::string& _name);
	std::string remove_file_extension(const std::string& _name);
	bool is_directory(const std::string& _name);
	std::string concat_path_and_filename(const std::string& _path, const std::string& _file_name);
	std::string concat_paths(const std::string& _path1, const std::string& _path2);
	std::string concat_paths(const std::string& _path1, const std::string& _path2, const std::string& _path3);
	std::string concat_paths(const std::string& _path1, const std::string& _path2, const std::string& _path3, const std::string& _path4);
	std::string extract_name_from_path(const std::string& _path);
	std::string extract_directory_from_path(const std::string& _path);
	std::string to_data_path(const std::string& _file_name);

	bool create_directory(const std::string& _path);
	void copy_file(const std::string& _file_path, const std::string& _destination_path);
	void copy_file_in_directory(const std::string& _file_path, const std::string& _destination_directory_path);
	void delete_files_in_directory(const std::string& _directory_path, const std::string _file_extension="");

	enum FileType { file, directory, both };
	/*! Return file names relatively to _directory_path (doesn't return full path).*/
	std::vector<std::string> get_file_names(const std::string& _directory_path, FileType _file_type = FileType::file);

	bool is_image_file(const std::string& _file_name);
	bool is_video_file(const std::string& _file_name);

	/*! List of extensions corresponding to image files. Extensions can be added.*/
	EXTERN_CELF_API std::vector<std::string> image_extensions;
	/*! List of extensions corresponding to video files. Extensions can be added.*/
	EXTERN_CELF_API std::vector<std::string> video_extensions;

	std::vector<std::string> get_images_in_path_recursive(const std::string& _path, bool _l_recursive = true);
	std::vector<std::string> get_images_in_path(const std::string& _path);


	std::vector<std::string> get_sub_strings(const std::string& _string, const char _separator = ' ');

	/*! Ndigits up to 6 for now. A better implementation is easy to make.*/
	std::string int_to_string(const unsigned int i, const unsigned int _Ndigits = 3);

	std::vector<unsigned int> get_numbers(std::string _string);

	std::vector<unsigned int> get_count_vector(unsigned int _N);

	template <class T>
	void add_vector(std::vector<T>& _vector1, const std::vector<T>& _vector2);
	template <class T>
	typename std::vector<T>::const_iterator vector_find(const std::vector<T>& _vector, const T& _value);
	template <class T>
	typename std::vector<T>::iterator vector_find(std::vector<T>& _vector, const T& _value);
	template <class T>
	bool vector_find_bool(const std::vector<T>& _vector, const T& _value);

	template <class T>
	T gaussian(const T& _value, const T& _center, const T& _sigma);
	template <class T>
	double get_mean(const std::vector<T>& _values);
	template <class T>
	double get_var(const std::vector<T>& _values, const double _mean);
	template <class T>
	double get_var(const std::vector<T>& _values);
	template <class T>
	double get_std(const std::vector<T>& _values);
	template <class T>
	double get_std(const std::vector<T>& _values, const double _mean);

	template <class T>
	T str_to(const std::string& _string);
	template <class T>
	T str_to_value(const std::string& _string);

	bool is_verbose();

	template <class T>
	bool is_nan(const T& _value);

	template <class T>
	bool is_positive_odd(const T& _value);

	template <class T>
	std::pair<T, T> calc_minmax(const std::vector<T>& _values);

	void filter_strings(std::vector<std::string>& _strings, const std::vector<std::string>& _filter_strings);

	void display_progression(const unsigned int _i_loop, const unsigned int _Nloop, const float _ratio_modulo=25.);

	/*! Checks wether increasing \p _iterator by \p _iterator_offset is in range of \p _list.*/
	template <class Tlist, class Titerator>
	bool check_iterator_offset_plus(const Titerator& _iterator, const int _iterator_offset, const Tlist& _list);
	/*! Checks wether decreasing \p _iterator by \p _iterator_offset is in range of \p _list.*/
	template <class Tlist, class Titerator>
	bool check_iterator_offset_minus(const Titerator& _iterator, const int _iterator_offset, const Tlist& _list);
}

template <class Tlist, class Titerator>
bool Misc::check_iterator_offset_plus(const Titerator& _iterator, const int _iterator_offset, const Tlist& _list) {
	
	bool l_ok = std::distance(_list.begin(), _iterator) + _iterator_offset >= 0 && (std::distance(_iterator, _list.end()) - _iterator_offset) > 0;
	if (!l_ok) {
		DEBUG_BREAK;
	}
	return l_ok;
}

template <class Tlist, class Titerator>
bool Misc::check_iterator_offset_minus(const Titerator& _iterator, const int _iterator_offset, const Tlist& _list) {

	return std::distance(_list.begin(), _iterator) - _iterator_offset >= 0 && (std::distance(_iterator, _list.end()) + _iterator_offset) > 0;
}

template <class T>
std::string Misc::to_string(const T& _value, const int _Ndigits) {

	std::ostringstream strs;
	strs << _value;
	std::string str = strs.str();

	if (_Ndigits != -1) {

		unsigned int Nchar = (unsigned int)_Ndigits;
		if (str.find('.') != std::string::npos) {
			/*! If a dot was found.*/
			Nchar++;
		}

		if (str.size() > Nchar) {
			str = str.substr(0, Nchar);
		}

	}

	return str;
}

/*! Some specializations. Declared this way due to g++ bug. See https://stackoverflow.com/questions/25594644/warning-specialization-of-template-in-different-namespace*/
namespace Misc {
	template <>
	bool str_to_value<bool>(const std::string& _string);

	template <>
	unsigned int str_to_value<unsigned int>(const std::string& _string);

	template <>
	int str_to_value<int>(const std::string& _string);

	template <>
	float str_to_value<float>(const std::string& _string);

	template <>
	double str_to_value<double>(const std::string& _string);

	template <>
	std::string str_to_value<std::string>(const std::string& _string);
}


template <class T>
T Misc::str_to(const std::string& _string) {

	T value;

	try {
		value = str_to_value<T>(_string);
	} catch (const std::invalid_argument&) {
		std::cerr << "str_to_value : Argument is invalid\n";
		throw;
	} catch (const std::out_of_range&) {
		std::cerr << "str_to_value : Argument is out of range for a double\n";
		throw;
	}

	return value;
}

template <class T>
void Misc::add_vector(std::vector<T>& _vector1, const std::vector<T>& _vector2) {

	for (typename std::vector<T>::const_iterator iterator = _vector2.begin(); iterator != _vector2.end(); ++iterator) {

		_vector1.push_back(*iterator);

	}
}

template <class T>
typename std::vector<T>::const_iterator Misc::vector_find(const std::vector<T>& _vector, const T& _value) {

	return std::find(_vector.begin(), _vector.end(), _value);
}

template <class T>
typename std::vector<T>::iterator Misc::vector_find(std::vector<T>& _vector, const T& _value) {

	return std::find(_vector.begin(), _vector.end(), _value);
}

//////// Math functions ////////

template <class T>
T Misc::gaussian(const T& _value, const T& _center, const T& _sigma) {

	return std::exp(-std::pow((_value-_center)/_sigma, 2));

}

template <class T>
double Misc::get_mean(const std::vector<T>& _values) {

	double mean = 0.;
	for (typename std::vector<T>::const_iterator iterator = _values.begin(); iterator != _values.end(); ++iterator) {
		mean += *iterator;
	}
	mean /= _values.size();
	return mean;
}

template <class T>
double Misc::get_var(const std::vector<T>& _values) {

	return get_var(_values, get_mean(_values));
}

template <class T>
double Misc::get_var(const std::vector<T>& _values, const double _mean) {

	double var = 0.;
	for (typename std::vector<T>::const_iterator iterator = _values.begin(); iterator != _values.end(); ++iterator) {
		var += std::pow(*iterator - _mean, 2);
	}
	var /= _values.size();
	return var;
}

template <class T>
double Misc::get_std(const std::vector<T>& _values) {

	return sqrt(get_var(_values));
}

template <class T>
double Misc::get_std(const std::vector<T>& _values, const double _mean) {

	return sqrt(get_var(_values, _mean));
}

template <class T>
bool Misc::vector_find_bool(const std::vector<T>& _vector, const T& _value) {

	return vector_find(_vector, _value) != _vector.end();
}

template <class T>
bool Misc::is_nan(const T& _value) {

	if (_value != _value) {
		DEBUG_BREAK;
		return true;
	} else {
		return false;
	}

}

template <class T>
bool Misc::is_positive_odd(const T& _value) {

	return _value > 0 && (_value - 1) % 2 == 0;
}

template <class T>
std::pair<T, T> Misc::calc_minmax(const std::vector<T>& _values) {
	
	std::pair<T, T> minmax;
	minmax.first = std::numeric_limits<T>::max();
	minmax.second = std::numeric_limits<T>::lowest();
	for (typename std::vector<T>::const_iterator it = _values.begin(); it != _values.end(); ++it) {
		if (*it > minmax.second) {
			minmax.second = *it;
		}
		if (*it < minmax.first) {
			minmax.first = *it;
		}
	}

	return minmax;
}

/////////////////////////////
// Some ostream operators
////////////////////////////

template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& _vector) {
	os << "[";
	for (unsigned int i = 0; i < _vector.size(); i++) {
		os << _vector[i];
		if (i < _vector.size() - 1) {
			os << ", ";
		}
	}
	os << "]";
	return os;
}

template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& _pair) {
	os << "[" << _pair.first << " ; " << _pair.second << "]";
	return os;
}

template <class Tkey, class T>
std::ostream& operator<<(std::ostream& os, const std::map<Tkey, T>& _map) {
	os << "[";
	for (typename std::map<Tkey, T>::const_iterator it = _map.begin(); it != _map.end(); it++) {
		os << " ";
		os << "(" << it->first << "," << it->second << ")";
	}
	os << "]";
	return os;
}

template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& _array) {
	os << "[";
	for (unsigned int i = 0; i < _array.size(); i++) {
		os << _array[i];
		if (i < _array.size() - 1) {
			os << ", ";
		}
	}
	os << "]";
	return os;
}
