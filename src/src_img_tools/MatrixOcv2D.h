/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/

#pragma once

#include "ocv.h"

template <class Tvec>
class MatrixOcv2D {

	Tvec vec11;
	Tvec vec12;
	Tvec vec21;
	Tvec vec22;

public:
	MatrixOcv2D();
	~MatrixOcv2D();

	const Tvec& operator() (unsigned int i, unsigned int j) const;
	Tvec& operator() (unsigned int i, unsigned int j);

	MatrixOcv2D<Tvec>& operator*=(const ocv::Tvalue& _value);
	MatrixOcv2D<Tvec>& operator/=(const ocv::Tvalue& _value);
	MatrixOcv2D<Tvec>& operator*=(const Tvec& _value);
	MatrixOcv2D<Tvec>& operator/=(const Tvec& _value);

	template <class Tvec2>
	friend std::ostream& operator<< (std::ostream& _os, const MatrixOcv2D<Tvec>& _matrix);
};

template <class Tvec>
MatrixOcv2D<Tvec>::MatrixOcv2D() {

}

template <class Tvec>
MatrixOcv2D<Tvec>::~MatrixOcv2D() {

}

template <class Tvec>
const Tvec& MatrixOcv2D<Tvec>::operator() (unsigned int i, unsigned int j) const {

	return (*const_cast<MatrixOcv2D<Tvec>*>(this))(i, j);
}

template <class Tvec>
Tvec& MatrixOcv2D<Tvec>::operator() (unsigned int i, unsigned int j) {

	switch (i) {
	case 0:
		switch (j) {
		case 0:
			return vec11;
		case 1:
			return vec12;
		default:
			break;
		}
	case 1:
		switch (j) {
		case 0:
			return vec21;
		case 1:
			return vec22;
		default:
			break;
		}
	default:
		return vec11;
		DEBUG_BREAK;
		break;
	}
}

template <class Tvec>
MatrixOcv2D<Tvec>& MatrixOcv2D<Tvec>::operator*=(const ocv::Tvalue& _value) {

	vec11 *= _value;
	vec12 *= _value;
	vec21 *= _value;
	vec22 *= _value;

	return *this;
}

template <class Tvec>
MatrixOcv2D<Tvec>& MatrixOcv2D<Tvec>::operator/=(const ocv::Tvalue& _value) {

	vec11 /= _value;
	vec12 /= _value;
	vec21 /= _value;
	vec22 /= _value;

	return *this;
}

template <class Tvec>
MatrixOcv2D<Tvec>& MatrixOcv2D<Tvec>::operator*=(const Tvec& _value) {

	vec11 *= _value;
	vec12 *= _value;
	vec21 *= _value;
	vec22 *= _value;

	return *this;
}

template <class Tvec>
MatrixOcv2D<Tvec>& MatrixOcv2D<Tvec>::operator/=(const Tvec& _value) {

	cv::divide(vec11, _value, vec11);
	cv::divide(vec12, _value, vec12);
	cv::divide(vec21, _value, vec21);
	cv::divide(vec22, _value, vec22);

	return *this;
}

template <class Tvec>
inline std::ostream& operator<< (std::ostream& _os, const MatrixOcv2D<Tvec>& _matrix) {

	_os << "(" << _matrix(0, 0) << ", " << _matrix(0, 1) << ")" << std::endl;
	_os << "(" << _matrix(1, 0) << ", " << _matrix(1, 1) << ")" << std::endl;

	return _os;
}

