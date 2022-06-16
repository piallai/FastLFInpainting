/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/***        GNU Affero General Public License version 3			  */
/******************************************************************/

#pragma once

template <class T>
void Images4D_base::resize(Tarray<T>& _array, const unsigned int& _N1, const unsigned int& _N2) {

	_array.resize(_N1);
	for (unsigned int u = 0; u < _N1; u++) {
		_array[u].resize(_N2);
	}

}

template <class T>
void Images4D_base::resize(Tarray<T>& _array, const cv::Size& _size) {

	resize(_array, _size.width, _size.height);

}

template <class T>
unsigned int Images4D_base::get_N1(const Tarray<T>& _array) {

	return (unsigned int)_array.size();

}

template <class T>
unsigned int Images4D_base::get_N2(const Tarray<T>& _array) {

	if (!_array.empty()) {
		return (unsigned int)_array[0].size();
	} else {
		return 0;
	}

}
